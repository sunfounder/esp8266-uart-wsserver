#include "led.hpp"

bool _led_enable = true;  // Whether to enable LED
double led_startMillis = 0;      // LED state start time
uint8_t builtin_led_status = LED_OFF;
bool led_status_flag = false;

void builtin_led_init() {
  pinMode(BUILTIN_LED1, OUTPUT);     // Set LED pin as output
  digitalWrite(BUILTIN_LED1, HIGH);  // 1:turn off LED
}

void builtin_led_enable() {_led_enable = true;}

void builtin_led_disable() {_led_enable = false;}

void builtin_led_off() { builtin_led_status = LED_OFF; }

void builtin_led_on() { builtin_led_status = LED_ON; }

void builtin_led_slow_blink() { builtin_led_status = LED_SLOW_BLINK; }

void builtin_led_fast_blink() { builtin_led_status = LED_FAST_BLINK; }

void builtin_led_status_handler() {
  if (_led_enable) {
    switch (builtin_led_status) {
      case LED_OFF:
        digitalWrite(BUILTIN_LED1, HIGH);
        break;
      case LED_ON:
        digitalWrite(BUILTIN_LED1, LOW);
        break;
      case LED_SLOW_BLINK:
        if (millis() - led_startMillis > SLOW_BLINK_DELAY) {
          led_startMillis = millis();
          led_status_flag = !led_status_flag;
          digitalWrite(BUILTIN_LED1, led_status_flag);  // slow blink
        }
        break;
      case LED_FAST_BLINK:
        if (millis() - led_startMillis > FAST_BLINK_DELAY) {
          led_startMillis = millis();
          led_status_flag = !led_status_flag;
          digitalWrite(BUILTIN_LED1, led_status_flag);  // fast blink
        }
    }
  } else {
    digitalWrite(BUILTIN_LED1, HIGH);
  }
}
