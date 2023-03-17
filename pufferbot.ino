
#include "encoder.h"
#include "motor.h"
#include "wifi.h"
#include "controller.h"
#include "ota.h"
#include "led.h"
#include "test.h"

// Setup
ENCODER (Left, 17, 23);
ENCODER (Right, 16, 4); //parker switched these two because the motors were not in directional agreement with the encoders
MOTOR (Left, 25, 26, 2, 3);
MOTOR (Right, 18, 19, 0, 1);
WI_FI(mazenet, Albifrons2020, 155, 4500);
OTA(5000);

void setup() {
  Serial.begin(115200);
  led_setup();
  led_all_on();
  delay(100);
  ENCODER_INIT(Left);
  ENCODER_INIT(Right);
  MOTOR_INIT(Left);
  MOTOR_INIT(Right);
  led_all_off();
  delay(100);
  Serial.print("connecting to wifi");
  WIFI_INIT(Serial.print("."); delay(500););
  OTA_INIT();
  Serial.println("Done");
  led_all_on();

  Serial.print("tick_error_L");
  Serial.print(" ");
  Serial.print("tick_error_R");
  Serial.print(" ");
  Serial.print("left_speed");
  Serial.print(" ");
  Serial.println("right_speed");
  delay(100);
  
}


// WIFI Recieve and store data 
struct MyData {
  int32_t left, right;
  int32_t speed;
} data, in_progress, queued;


// Variables
float max_speed_L = 0; // bound for desired speed
float max_speed_R = 0;
float desired_speed_L;
float desired_speed_R;
int left_pwm = 0;      
int right_pwm = 0;


int tick_error_L = 0;
int tick_error_R = 0;
float speed_error_L;
float speed_error_R;
float edot_L = 0;
float eprev_L = 0;
float edot_R = 0;
float eprev_R = 0;
float eint_L = 0;
float eint_R = 0;
unsigned long brake_time = 0;


#define P2 20    //3.5


// state machine
bool in_progress_empty = true;  // open for new data 
bool queue_empty = true;        // no data store
bool goal_reached_L = false;
bool goal_reached_R = false;
int direction_L;
int direction_R;
bool must_repeat = true;


void loop() {
  OTA_CHECK_UPDATES();
  WIFI_ATTEND_CLIENT();
  
  ENCODER_FILTER(Left);
  ENCODER_FILTER(Right);
  
  // load data to queue if data is available
  if (queue_empty && WIFI_DATA_AVAILABLE(data)) {
    queued = data;
    queue_empty = false;
  }

  // load queue into progress
  if (!queue_empty && in_progress_empty) {
    
    in_progress_empty = false;
    in_progress.left += queued.left;   
    in_progress.right += queued.right;
    in_progress.speed = queued.speed;
    queue_empty = true;
    
    // compute when 'filling' progress because its for overshoot prevention - set direction 
    direction_L = in_progress.left - get_encoder_tick_counter(Left);  // pos = fwd
    direction_R = in_progress.right - get_encoder_tick_counter(Right);
  }
  
  
  // repeat if desired speed drops
  while (must_repeat) {
    must_repeat = false;

    // compute desired speed 
    tick_error_L = in_progress.left - get_encoder_tick_counter(Left);
    desired_speed_L = P2 * abs(tick_error_L);
    tick_error_R = in_progress.right - get_encoder_tick_counter(Right);
    desired_speed_R = P2 * abs(tick_error_R);

    // set left and right max speeds based on ticks
    if (abs(tick_error_L) > abs(tick_error_R)){
      max_speed_L = in_progress.speed;
      max_speed_R = in_progress.speed * ( abs((float)tick_error_R/ (float)tick_error_L));
    } else {
      max_speed_R = in_progress.speed;
      max_speed_L = in_progress.speed * ( abs((float)tick_error_L/ (float)tick_error_R));
    }
   
    // catch speed drop 
    if (desired_speed_L > max_speed_L){ 
      desired_speed_L = max_speed_L;
    } else {
      if (!queue_empty){
        test4(get_encoder_tick_counter(Left), get_encoder_tick_counter(Right));
        in_progress.speed = queued.speed;
        in_progress.left += queued.left;
        in_progress.right += queued.right;
        direction_L = in_progress.left - get_encoder_tick_counter(Left);  // pos = fwd
        direction_R = in_progress.right - get_encoder_tick_counter(Right);
        queue_empty = true;
        must_repeat = true;
      }
    }
    if (desired_speed_R > max_speed_R){
      desired_speed_R = max_speed_R;
    } else {
      if (!queue_empty){
        in_progress.speed = queued.speed;
        in_progress.left += queued.left;
        in_progress.right += queued.right;
        direction_L = in_progress.left - get_encoder_tick_counter(Left);  // pos = fwd
        direction_R = in_progress.right - get_encoder_tick_counter(Right);
        queue_empty = true;
        must_repeat = true;
      }
    }
  }
  must_repeat = true;
  
  // check whether ticks have exceeded desired ticks (convert to macro later)
  goal_reached_L = get_goal_state(direction_L, in_progress.left, get_encoder_tick_counter(Left));
  goal_reached_R = get_goal_state(direction_R, in_progress.right, get_encoder_tick_counter(Right));


if (goal_reached_L || goal_reached_R){
    // brake
    left_pwm = 0;
    right_pwm = 0;
    in_progress_empty = true;

    //only use short circuit breaking mode for 1 second, then return to coast
    if (brake_time == 0) {
      brake_time = millis();
    } else if (brake_time - millis() > 1000) {
      left_pwm = 1;
      right_pwm = 1;
    }
  } else {
    //reset brake time
    brake_time = 0;
    
    // compute error for PD control Left
    speed_error_L = desired_speed_L - abs(get_encoder_filtered_speed(Left));
    edot_L = speed_error_L - eprev_L;
    eprev_L = speed_error_L;
    eint_L += speed_error_L;
  
  
    // compute error for PD control Right
    speed_error_R = desired_speed_R - abs(get_encoder_filtered_speed(Right));
    edot_R = speed_error_R - eprev_R;
    eprev_R = speed_error_R;
    eint_R += speed_error_R;
  
    left_pwm = get_pwm2(abs(left_pwm), tick_error_L, speed_error_L, edot_L, eint_L);
    right_pwm = get_pwm2(abs(right_pwm), tick_error_R, speed_error_R, edot_R, eint_R);
  }

 
  
  Serial.print(tick_error_L);
  Serial.print(" ");
  Serial.print(tick_error_R);
  Serial.print(" ");
  Serial.print(get_encoder_filtered_speed(Left));
  Serial.print(" ");
  Serial.println(get_encoder_filtered_speed(Right));


  

//  test1(get_encoder_filtered_speed(Left), get_encoder_filtered_speed(Right)); // measured speed 
 //  test2(tick_error_L, tick_error_R, get_encoder_filtered_speed(Left), get_encoder_filtered_speed(Right));       //tick error and measured speed
//    test4(left_pwm, right_pwm);
//  test4(get_encoder_tick_counter(Left), get_encoder_tick_counter(Right));
    
  
  
  // UPDATE MOTOR RATE
  MOTOR_PWM(Left, left_pwm);
  MOTOR_PWM(Right,right_pwm);
  delay(10);  // ms
}
