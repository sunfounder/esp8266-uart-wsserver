#ifndef _WS_SERVER_H
#define _WS_SERVER_H

#include <Arduino.h>

// #define DEBUG

#define REGIONS (char[26]){'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L', 'M', 'N', 'O', 'P', 'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X', 'Y', 'Z'}
#define REGIONS_LENGTH 26
#define WS_BUFFER_SIZE 1024

class WS_Server {
  public:
    WS_Server();
    void begin(int port);
    void loop();
    void set_send_mode(uint8_t mode);
    void send(String data);
};


#endif
