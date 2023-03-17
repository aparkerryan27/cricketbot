#pragma once
// GLOBALS
// timer interupt globals
volatile int interruptCounter;                         // shared between main loop and ISR - signals main loop that an interuppt has occurred
hw_timer_t * timer = NULL;                             // need to configure timer
portMUX_TYPE timerMux = portMUX_INITIALIZER_UNLOCKED;  // synchronization

// tick rate pid
int pwm_L = 0;
int current_L = 0;
int e_L = 0;
int eprev_L = 0;
int edot_L;

int pwm_R = 0;
int current_R = 0;
int e_R = 0;
int eprev_R = 0;
int edot_R;

float P = 0.5; // 1
float D = 3;

// setting PWM properties
#define freq  1600 
#define resolution 8


struct Command {
  int32_t left_tick, right_tick;
  int16_t speed;
  uint8_t data;
} command;


struct Data {
  bool puff, led0, led1, led2, increase, decrease, brake;
} data;


// Assign encoder pins
//Encoder Right = {4, 16};
//Encoder Left = {23, 17};

// FUNCTION PROTOTYPES
void rightBrake();
void leftBrake();


// WIFI SETUP
const char* ssid     = "mazenet";
const char* password = "Albifrons2020";
IPAddress local_IP(192, 168, 137, 155);
IPAddress gateway(192, 168, 137, 1);
IPAddress subnet(255, 255, 0, 0);
IPAddress primaryDNS(8, 8, 8, 8);   //optional
IPAddress secondaryDNS(8, 8, 4, 4); //optional
WiFiServer wifiServer(4500);


// Commands sent to robot
uint8_t *buffer = (uint8_t *)&command;
size_t buffer_size = sizeof(Command);
WiFiClient client;
