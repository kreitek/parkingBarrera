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
extern unsigned int T_TEST_SERVIDOR;
extern unsigned int T_ABIERTA_SIN_ESPERAR;
extern unsigned int T_ABIERTA_LIBRE_ESPERAR;
extern unsigned int T_REINTENTA_SIN_FC;
extern unsigned int T_REINTENTA_CON_FC;

void config_read();
void config_write();
