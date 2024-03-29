/*******************************************************************
  This is a firmware read convert uart command and create a websocket
  server. It's originally for Arduino or Raspberry Pi Pico to connect
  to SunFounder Controller. Send command and data over UART in
  boardrate 115200.

  Dependent libraries:
    - ArduinoJson (by Benoit Blanchon)
    - WebSockets (by Markus Sattler)

  Board tools:
    - Generic ESP8266 Module

  Version: 2.0.0
    -- https://github.com/sunfounder/esp8266-uart-wsserver.git

  Author: Sunfounder
  Website: http://www.sunfounder.com
           https://docs.sunfounder.com
 *******************************************************************/

#include <Arduino.h>
#include "led.hpp"
#include "wifi_helper.hpp"
#include "ws_server.hpp"

#define VERSION "2.0.0"

/* Set the Debug Level
  for production, set "DEBUG_LEVEL" to "CAM_DEBUG_LEVEL_INFO"
*/
#define DEBUG_LEVEL CAM_DEBUG_LEVEL_INFO
// #define DEBUG_LEVEL CAM_DEBUG_LEVEL_DEBUG
#define CAM_DEBUG_LEVEL_OFF 0
#define CAM_DEBUG_LEVEL_ERROR 1
#define CAM_DEBUG_LEVEL_INFO 2
#define CAM_DEBUG_LEVEL_DEBUG 3
#define CAM_DEBUG_LEVEL_ALL 4

#define SERIAL_TIMEOUT 100  // timeout 100ms
#define CHAR_TIMEOUT 5 // char timeout (ms)
String rxBuf = "";
bool rx_complete = false;

// Mode
#define NONE 0
#define STA 1
#define AP 2

WS_Server ws_server = WS_Server();
WiFiHelper wifi = WiFiHelper();

String WIFI_MODES[3] = {"None", "STA", "AP"};

// internal Variables
String ssid = "";
String password = "";
int port = 0;
int mode = NONE;

bool isConnected = false;
String ip = "";

void debug(String msg);
void debug(String msg, String data);
void error(String msg);

String serialRead();
void serial_received_handler();
void handleSet(String cmd);
void start();

/*--------------------- setup() & loop() ------------------------------*/
void setup() {
  Serial.begin(115200);
  Serial.setTimeout(SERIAL_TIMEOUT);
  builtin_led_init();
  LED_STATUS_DISCONNECTED();
  debug("Start !", " ");
  Serial.println("\r\n[OK] " VERSION);
}

void loop() {
  builtin_led_status_handler();
  if (wifi.is_connected) {
    if (mode == STA) {
      wifi.check_status();
    }
    ws_server.loop();
  }
  serial_received_handler();
}

/*--------------------------functions-------------------------------*/
void serial_received_handler() {
  rxBuf = serialRead();
  if (rxBuf.length() > 0) {
    debug("RX Receive: ", rxBuf);
    if (rxBuf.substring(0, 4) == "SET+") {
      handleSet(rxBuf.substring(4));
    } else if (rxBuf.substring(0, 3) == "WS+") {
      String out = rxBuf.substring(3);
      ws_server.send(out);
    }
  }
}

String serialRead() {
  String buf = "";
  char inChar;
  uint32_t char_time = millis();
  while (Serial.available() || millis() - char_time < CHAR_TIMEOUT) {
    inChar = (char)Serial.read();
    if (inChar == '\n') {
      break;
    } else if (inChar == '\r') {
      char_time = millis();
      continue;
    } else if ((int)inChar != 255) {
      buf += inChar;
      char_time = millis();
    }
  }
  return buf;
}

void handleSet(String cmd) {
  // ------------ 3 characters command  ------------
  String _3_chars_cmd = cmd.substring(0, 3);
  // PSK
  if (_3_chars_cmd == "PSK") {
    password = cmd.substring(3);
    debug("Set password: ", password);
    Serial.println("[OK]");
    return;
  }
  // LED
  else if (_3_chars_cmd == "LED") {
    int _led_enable = cmd.substring(3).toInt();
    if(_led_enable == 0) {
      debug("Set LED: disable");
      builtin_led_disable();
    } else if (_led_enable == 1) {
      debug("Set LED: enable");
      builtin_led_enable();
    }
    Serial.println("[OK]");
    return;
  }
  // SMD
  else if (_3_chars_cmd == "SMD") {
    ws_server.set_send_mode(cmd.substring(3).toInt());
    debug("Set ws_send_mode: ", cmd.substring(3));
    Serial.println("[OK]");
    return;
  }

  // ------------ 4 characters command  ------------
  String _4_chars_cmd = cmd.substring(0, 4);
  // SSID 
  if (_4_chars_cmd == "SSID") {
    ssid = cmd.substring(4);
    debug("Set SSID: ", ssid);
    Serial.println("[OK]");
    return;
  }
  // PORT
  else if (_4_chars_cmd == "PORT") {
    port = cmd.substring(4).toInt();
    debug("Set port: ", String(port));
    Serial.println("[OK]");
    return;
  }
  // MODE
  else if (_4_chars_cmd == "MODE") {
    mode = cmd.substring(4).toInt();
    debug("Set mode: ", WIFI_MODES[mode]);
    Serial.println("[OK]");
    return;
  }
  // NAME
  else if (_4_chars_cmd == "NAME") {
    ws_server.set_name(cmd.substring(4));
    debug("Set name: ", cmd.substring(4));
    Serial.println("[OK]");
    return;
  }
  // TYPE
  else if (_4_chars_cmd == "TYPE") {
    ws_server.set_type(cmd.substring(4));
    debug("Set type: ", cmd.substring(4));
    Serial.println("[OK]"); 
    return;
  }

  // ------------ 5 characters command  ------------
  String _5_chars_cmd = cmd.substring(0, 5);
  // CHECK
  if (_5_chars_cmd == "CHECK") {
    ws_server.set_check(cmd.substring(5));
    debug("Set check: ", cmd.substring(5));
    Serial.println("[OK]"); 
    return;
  }
  // RESET
  else if (_5_chars_cmd == "RESET") {
    debug("Reset", " ");
    delay(10);
    ESP.reset();
    return;
  }
  // START
  else if (_5_chars_cmd == "START") {
    if (ssid.length() == 0) {
      Serial.println("[ERROR] Please set ssid");
    } else if (password.length() == 0) {
      Serial.println("[ERROR] Please set password");
    } else if (mode == NONE) {
      Serial.println("[ERROR] Please set mode");
    } else if (port == 0) {
      Serial.println("[ERROR] Please set port");
    } else {
      start();
    }
    return;
  }

  // ----------- if no retrun before -----------
  Serial.println("[ERROR] SET+ Unknown command");

}

void start() {
  LED_STATUS_ERROOR();
  if (ssid.length() == 0) {
    error("Please set ssid");
  } else if (password.length() == 0) {
    error("Please set password");
  } else if (mode == NONE) {
    error("Please set mode");
  } else if (port == 0) {
    error("Please set port");
  } else{
    bool result = wifi.connect(mode, ssid, password);
    if (!result) {
      error("TIMEOUT");
      LED_STATUS_ERROOR();
    } else {
      LED_STATUS_DISCONNECTED();
      ws_server.close();
      ws_server.begin(port);
      debug("Websocket on!");
      Serial.print("[OK] ");Serial.println(wifi.ip);
    }
  }
}

/*---------------------------- debug print ---------------------------------*/
void debug(String msg) {
  #if (DEBUG_LEVEL >= CAM_DEBUG_LEVEL_DEBUG)
    Serial.print(F("[ESP01_D] "));
    Serial.println(msg);
  #endif
}

void debug(String msg, String data) {
  #if (DEBUG_LEVEL >= CAM_DEBUG_LEVEL_DEBUG)
    Serial.print(F("[ESP01_D] "));
    Serial.print(msg);
    Serial.println(data);
  #endif
}

void error(String msg) {
  #if (DEBUG_LEVEL >= CAM_DEBUG_LEVEL_ERROR)
    Serial.print(F("[ESP01_E] "));
    Serial.println(msg);
  #endif
}
