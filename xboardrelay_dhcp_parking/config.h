#include <Arduino.h>
#include <Ethernet.h>

extern byte MAC[6];
extern byte IP[4];
extern const char* HOSTNAME;
extern const char* PASSWORD;
extern const int PORT;
extern const char* FOLDER;
extern IPAddress request_server;
//extern char request_server[];
extern const String RUTA_REQUEST;
