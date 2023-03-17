#include "controller.h"

// PD control for speed 
int get_pwm2(int pwm, int tick_error, float speed_error, float edot, float eint){
  
  pwm += P * speed_error + D * edot + I * eint;
  
  // Bound pwm  
  if (pwm>max_pwm)pwm = max_pwm; 
   
  // Assign direction 
  if (tick_error < 0)pwm*= -1;
  return pwm;
}


bool get_goal_state(int init_error, int tick_goal, int tick_count){
  if (init_error > 0){
    if (tick_count >= tick_goal)return true;
    else return false;
  } else  {
    if (tick_count <= tick_goal) return true;
    else return false;
  }  
}





// To do:
// worried about pwm overshoot 
// current method does not guarantee this will not happen 
