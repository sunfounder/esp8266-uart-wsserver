#ifndef _WS_SERVER_H
#define _WS_SERVER_H

#include <Arduino.h>

// #define DEBUG

#define REGIONS (char[26]){'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L', 'M', 'N', 'O', 'P', 'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X', 'Y', 'Z'}
#define REGIONS_LENGTH 26
#define WS_BUFFER_SIZE 1024

#define TIMEOUT 2000

class WS_Server {
  public:
    WS_Server();
    void begin(int port);
    void close();
    void loop();
    void send(String data);

    void set_name(String name);
    void set_type(String type);
    void set_check(String check);
    void set_send_mode(uint8_t mode);
};


#endif
