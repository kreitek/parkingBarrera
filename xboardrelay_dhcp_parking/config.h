#include <Arduino.h>
#include <Ethernet.h>

extern byte MAC[6];
extern byte IP[4];
extern const char* HOSTNAME;
extern const char* PASSWORD;
extern const int PORT;
extern const char* FOLDER;
extern const char* REQUEST_HOST;
extern const char* REQUEST_QUERY;
extern const unsigned char T_TEST_SERVIDOR;
extern const unsigned char T_ABIERTA_SIN_ESPERAR;
extern const unsigned char T_ABIERTA_LIBRE_ESPERAR;
extern const unsigned char T_REINTENTA_SIN_FC;
extern const unsigned char T_REINTENTA_CON_FC;
