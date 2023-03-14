#ifndef _LED_H
#define _LED_H

#include <Arduino.h>

#define BUILTIN_LED1 2  // BUILTIN_LED GPIO2

#define LED_OFF 0
#define LED_ON 1
#define LED_SLOW_BLINK 2
#define LED_FAST_BLINK 3

#define SLOW_BLINK_DELAY 500  // uint ms
#define FAST_BLINK_DELAY 100

void builtin_led_init();
void builtin_led_enable();
void builtin_led_disable();
void builtin_led_off();
void builtin_led_on();
void builtin_led_slow_blink();
void builtin_led_fast_blink();
void builtin_led_status_handler();

#define LED_STATUS_DISCONNECTED() builtin_led_slow_blink()
#define LED_STATUS_CONNECTED() builtin_led_on()
#define LED_STATUS_ERROOR() builtin_led_fast_blink()

#endif
