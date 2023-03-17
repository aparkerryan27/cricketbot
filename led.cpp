#include "led.h"
void led_on(int led){
   ledcWrite(led, 255);
}
void led_off(int led){
   ledcWrite(led, 0);
}
void led_all_on() {
  led_on(LED0);
  led_on(LED1);
  led_on(LED2);
}
void led_all_off(){
  led_off(LED0);
  led_off(LED1);
  led_off(LED2);
}
void led_setup(){
  ledcSetup(LED0, LED_FREQ, LED_RES);
  ledcSetup(LED1, LED_FREQ, LED_RES);
  ledcSetup(LED2, LED_FREQ, LED_RES);
  ledcAttachPin(LED0_PIN, LED0);
  ledcAttachPin(LED1_PIN, LED1);
  ledcAttachPin(LED2_PIN, LED2);  
}
