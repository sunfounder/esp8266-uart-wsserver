#include "ws_server.hpp"
#include <WebSocketsServer.h>
#include <ArduinoJson.h>
#include "led.hpp"
#include "Ticker.h"

uint32_t test_time = 0;

void onWebSocketEvent(uint8_t cn, WStype_t type, uint8_t* payload, size_t length);

WebSocketsServer ws = WebSocketsServer(8765);
uint8_t client_num = 0;
uint8_t ws_send_mode = 0; // 0:send original text; 1:send simplified text
bool ws_connected = false;

bool auto_send_check = false;
String ws_name = "ESP01S";
String ws_type = "ESP01S";
String ws_check = "SC";

Ticker pingPongTimer; // timer for checing ping_pong

bool isPingPongOK = true;
uint32_t lastPingPong = 0;
uint32_t last_pong_time = 0;
uint16_t PONG_INTERVAL = 200;

String intToString(uint8_t* value, size_t length) {
  String buf;
  for (int i = 0; i < length; i++) {
    buf += (char)value[i];
  }
  return buf;
}

void checkPingPong(){
  
  if (ws_connected == false) {
    return;
  }
  if (millis() - lastPingPong > TIMEOUT) {
    lastPingPong = millis();
    isPingPongOK = false;
    #ifdef DEBUG
    Serial.println("[DEBUG] [WS] PingPong timeout");
    #endif
    Serial.println("[DISCONNECTED] timeout");
  }
}

WS_Server::WS_Server() {}

void WS_Server::close() {
  ws_connected = false;
  ws.close();
  delay(10);
}

void WS_Server::begin(int port) {
  ws = WebSocketsServer(port);
  ws.begin();
  ws.onEvent(onWebSocketEvent);
  pingPongTimer.attach_ms(20, checkPingPong);

  #ifdef DEBUG
    Serial.println("[DEBUG] WebSocket server started");
  #endif
}

void WS_Server::loop() {
  ws.loop();
}

void WS_Server::send(String data) {
  ws.sendTXT(client_num, data);
}

void WS_Server::set_name(String name) {
  ws_name = name;
  auto_send_check = true;
}

void WS_Server::set_type(String type) {
  ws_type = type;
  auto_send_check = true;
}

void WS_Server::set_check(String check) {
  ws_check = check;
  auto_send_check = true;
}

void WS_Server::set_send_mode(uint8_t mode) {
  ws_send_mode = mode;
}

void onWebSocketEvent(uint8_t cn, WStype_t type, uint8_t* payload, size_t length) {
  String out;
  client_num = cn;

  // send pong
  if (ws_connected == true) {
    uint32_t _time = millis();
    if (_time - last_pong_time > PONG_INTERVAL) {
      String msg = "pong "+String(_time);
      ws.sendTXT(client_num, msg);
      last_pong_time = millis();
      #ifdef DEBUG
      Serial.println("[DEBUG] [WS] send PONG");
      #endif
      // Serial.print("[DEBUG] PONG ");Serial.println(_time);
    }
  }

  #ifdef DEBUG
    Serial.println("[DEBUG] onWebSocketEvent");
  #endif
  switch (type) {
    // Client has disconnected
    case WStype_DISCONNECTED: {
      #ifdef DEBUG
        Serial.println("[DEBUG] [WS] Disconnected!");
      #endif
      IPAddress remoteIp = ws.remoteIP(client_num);
      LED_STATUS_DISCONNECTED();
      Serial.print("[DISCONNECTED] ");Serial.println(remoteIp.toString());
      ws_connected = false;
      break;
    }
    // New client has connected
    case WStype_CONNECTED: {
      LED_STATUS_CONNECTED();
      lastPingPong = millis();

      IPAddress remoteIp = ws.remoteIP(client_num);
      #ifdef DEBUG
        Serial.print("[DEBUG] [WS] Connection from ");
        Serial.println(remoteIp.toString());
      #endif
      Serial.print("[CONNECTED] ");Serial.println(remoteIp.toString());
      // Send check_info  to client
      if (auto_send_check) {
        String check_info = "{\"Name\":\"" + ws_name
                          + "\",\"Type\":\"" + ws_type
                          + "\",\"Check\":\"" + ws_check
                          + "\"}";
        Serial.println(check_info);
        ws.sendTXT(client_num, check_info);
        delay(100);
        ws.sendTXT(client_num, check_info);
      }
      ws_connected = true;
      break;
    }
    // receive text
    case WStype_TEXT: {
      out = intToString(payload, length);
      #ifdef DEBUG
        Serial.print("[DEBUG] [WS] Received text: ");
        Serial.println(out);
      #endif

      // reset ping_pong time
      lastPingPong = millis();
      if (strcmp(out.c_str(), "ping") == 0) {
        Serial.println("[APPSTOP]");
        return;
      }

      // ------------- send simplified text -------------
      if(ws_send_mode == 1) {
        DynamicJsonDocument recvBuffer(WS_BUFFER_SIZE);
        deserializeJson(recvBuffer, out);
        String result = "WS+";
        String result_temp = "";

        // REGIONS
        for (int i=0; i<REGIONS_LENGTH; i++){
          String region = String(REGIONS[i]);
          String value;
          if (recvBuffer[region].is<JsonArray>()) {
            for (int j=0; j<recvBuffer[region].size(); j++) {
              value += recvBuffer[region][j].as<String>();
              if (j != recvBuffer[region].size()-1) value += ',';
            }
          } else {
            value = recvBuffer[region].as<String>();
          }
          if (value == "true") value = "1";
          else if (value == "false") value = "0";
          if (value != "null") result_temp += value;
          if (i != REGIONS_LENGTH - 1) result_temp += ';';
        }

        // length check
        int len = result_temp.length() + 3 + 3 + 1; // WS+000x;
        if (len > 99) {
          result += String(len);
        } else if (len > 9) {
          result += "0";
          result += String(len);
        } else {
          result += "00";
          result += String(len);
        }
        result += ";";
        result += result_temp;

        // sernd
        Serial.println(result);
        break;
      }
      // ------------- send original text -------------
      else {
        Serial.println(out);
        break;
      }
    }
    // For everything else: do nothing
    case WStype_BIN: {
      #ifdef DEBUG
        Serial.println("[DEBUG] [WS] WStype_BIN");
      #endif
      break;
    }
    case WStype_ERROR: {
      LED_STATUS_ERROOR();
      #ifdef DEBUG
        Serial.println("[DEBUG] [WS] WStype_ERROR");
      #endif
      break;
    }
    case WStype_FRAGMENT_TEXT_START: {
      #ifdef DEBUG
        Serial.println("[DEBUG] [WS] WStype_FRAGMENT_TEXT_START");
      #endif
      break;
    }
    case WStype_FRAGMENT_BIN_START: {
      #ifdef DEBUG
        Serial.println("[DEBUG] [WS] WStype_FRAGMENT_BIN_START");
      #endif
      break;
    }
    case WStype_FRAGMENT: {
      #ifdef DEBUG
        Serial.println("[DEBUG] [WS] WStype_FRAGMENT");
      #endif
        break;
    }
    case WStype_FRAGMENT_FIN: {
      #ifdef DEBUG
      Serial.println("[DEBUG] [WS] WStype_FRAGMENT_FIN");
      #endif
      break;
    }
    case WStype_PING: {
      #ifdef DEBUG
        Serial.println("[DEBUG] [WS] WStype_PING");
      #endif
      break;
    }
    case WStype_PONG: {
      #ifdef DEBUG
        Serial.println("[DEBUG] [WS] WStype_PONG");
      #endif
      break;
    }
    default: {
      #ifdef DEBUG
        Serial.print("[DEBUG] [WS] Event Type: [");
        Serial.print(type);
        Serial.println("]");
      #endif
      break;
    }
  }
}

