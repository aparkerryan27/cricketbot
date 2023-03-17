# include "test.h"
#include <Esp.h>


void test1(float left_speed, float right_speed){
  Serial.print(left_speed);
  Serial.print(" ");
  Serial.println(right_speed);
}

void test2(int tick_error_L, int tick_error_R, float left_speed, float right_speed){
  Serial.print(tick_error_L);
  Serial.print(" ");
  Serial.print(tick_error_R);
  Serial.print(" ");
  Serial.print(left_speed);
  Serial.print(" ");
  Serial.println(right_speed);
}

void test3(float desired_speed_L, float desired_speed_R){
  Serial.print(desired_speed_L);
  Serial.print(" ");
  Serial.println(desired_speed_R);
}

void test4(int pwm_L, int pwm_R){
  Serial.print(pwm_L);
  Serial.print(" ");
  Serial.println(pwm_R);
}
