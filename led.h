#pragma once
#include <Esp.h>

#define LED0_PIN    27
#define LED0        6
#define LED1_PIN    12
#define LED1        7
#define LED2_PIN    14
#define LED2        8
#define LED_FREQ    5000
#define LED_RES     8

void led_on(int led);
void led_off(int led);
void led_all_on();
void led_all_off();
void led_setup();
