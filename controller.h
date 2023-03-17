#pragma once 
#include <Esp.h>

#define max_speed 2200.0
#define max_pwm 1023
#define min_pwm 500
#define P 0.02 // 0.005  // 0.01
#define D 0.3 //0.3
#define I 0 // 0.0001



int get_pwm2(int pwm, int tick_error, float speed_error, float edot, float);
bool get_goal_state(int tick_error, int tick_goal, int tick_count);




//  left_pwm = get_pwm(abs(left_pwm), tick_error_L, left_speed, abs(get_encoder_speed(Left)));
//  right_pwm = get_pwm(abs(right_pwm), tick_error_R, right_speed, abs(get_encoder_speed(Right)));
