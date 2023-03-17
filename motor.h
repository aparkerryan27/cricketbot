#pragma once
#define MOTOR_FREQ 1600
#define MOTOR_RES  10

#define MOTOR_PWM_MIN 700
#define MOTOR_PWM_MAX 1023

#define MOTOR(NAME, FPIN, BPIN, FCHANNEL, BCHANNEL) \
int motor_ ##NAME## _F_shape_counter = 0;\
int motor_ ##NAME## _F_pin = FPIN;\
int motor_ ##NAME## _B_pin = BPIN;\
int motor_ ##NAME## _FCHANNEL = FCHANNEL; \
int motor_ ##NAME## _BCHANNEL = BCHANNEL; \
hw_timer_t * motor_ ##NAME## _timer = NULL; \
int motor_ ##NAME## _pwm = 0; \
void IRAM_ATTR motor_ ##NAME##_onTimer() { \
  if (abs(motor_ ##NAME## _pwm) >= MOTOR_PWM_MIN) { \
    if (motor_ ##NAME## _pwm >= 0) { \
      ledcWrite(motor_ ##NAME## _FCHANNEL, motor_ ##NAME## _pwm); \
      ledcWrite(motor_ ##NAME## _BCHANNEL, 0); \
    } else { \
      ledcWrite(motor_ ##NAME## _BCHANNEL, -motor_ ##NAME## _pwm); \
      ledcWrite(motor_ ##NAME## _FCHANNEL, 0); \
    }\
  } else if (motor_ ##NAME## _pwm > 0) { \
    if (abs(motor_ ##NAME## _pwm) / 10 > motor_ ##NAME## _F_shape_counter % (MOTOR_PWM_MIN/10)) {\
      if (motor_ ##NAME## _pwm >= 0) { \
        ledcWrite(motor_ ##NAME## _FCHANNEL, MOTOR_PWM_MIN);\
        ledcWrite(motor_ ##NAME## _BCHANNEL, 0); \
      } else { \
        ledcWrite(motor_ ##NAME## _BCHANNEL, -MOTOR_PWM_MIN); \
        ledcWrite(motor_ ##NAME## _FCHANNEL, 0); \
      }\
    } else {\
      ledcWrite(motor_ ##NAME## _FCHANNEL, 0 ); \
      ledcWrite(motor_ ##NAME## _BCHANNEL, 0 ); \
    }\
  } else {\
    ledcWrite(motor_ ##NAME## _FCHANNEL, MOTOR_PWM_MAX/2); \
    ledcWrite(motor_ ##NAME## _BCHANNEL, MOTOR_PWM_MAX/2); \
  }\
  motor_ ##NAME## _F_shape_counter++;\
}

int motor_count = 0; // need to initialize a timer for each motor
#define MOTOR_INIT(NAME) \
  ledcSetup(motor_ ##NAME## _FCHANNEL, MOTOR_FREQ, MOTOR_RES);\
  ledcSetup(motor_ ##NAME## _BCHANNEL, MOTOR_FREQ, MOTOR_RES);\
  ledcAttachPin(motor_ ##NAME## _F_pin, motor_ ##NAME## _FCHANNEL); \
  ledcAttachPin(motor_ ##NAME## _B_pin, motor_ ##NAME## _BCHANNEL); \
  motor_ ##NAME## _timer = timerBegin(motor_count, 80, true); \
  timerAttachInterrupt(motor_ ##NAME## _timer, &motor_ ##NAME##_onTimer, true); \
  timerAlarmWrite(motor_ ##NAME## _timer, 100, true); \
  timerAlarmEnable(motor_ ##NAME## _timer);\
  motor_count ++;

#define MOTOR_PWM(NAME, PWM_VALUE) \
{ \
  motor_ ##NAME## _pwm = PWM_VALUE;\
}
