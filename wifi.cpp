#include "wifi.h"

bool wifi_read_data(WiFiClient &client, uint8_t *buffer, size_t buffer_size, bool wait){
  if (client) { 
    if (client.connected()){ 
      size_t i = 0;
      if (!wait) {
        if (client.peek()==-1) return false;
      }
      while (i < buffer_size){
        i += client.read( buffer + i, buffer_size - i); 
      }
      return (i == buffer_size);
    } 
  }
  return false;
}
