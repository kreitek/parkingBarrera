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
const unsigned int FRECUENCIA_TEST_SERVIDOR = 30000;
