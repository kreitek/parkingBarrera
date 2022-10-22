#include "config.h"

// esto es lo que hay que cambiar por cada cosa

byte MAC[6] = {0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0x0D};
const char* HOSTNAME = "relaypuerta"; 
const char* PASSWORD = "1234";
byte IP[4] = {0, 0, 0, 0};
const int PORT = 80;
const char* FOLDER = "/api/";
const char* REQUEST_HOST = "192.168.1.10";   // numeric IP (no DNS)
const char* REQUEST_QUERY = "/parker/barrera_alive/";
const unsigned char T_TEST_SERVIDOR = 30;
const unsigned char T_ABIERTA_SIN_ESPERAR = 60;
const unsigned char T_ABIERTA_LIBRE_ESPERAR = 3;
const unsigned char T_REINTENTA_SIN_FC = 10;
const unsigned char T_REINTENTA_CON_FC = 10;
