#include "config.h"

// esto es lo que hay que cambiar por cada cosa

byte MAC[6] = {0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0x0D};
const char* HOSTNAME = "relaypuerta"; 
const char* PASSWORD = "1234";
byte IP[4] = {192, 168, 0, 3};
const int PORT = 80;
const char* FOLDER = "/api/";
byte request_server[4] = {74, 125, 232, 128};   // numeric IP for Google (no DNS)
//char request_server[] = "www.google.com";   // name address for Google (using DNS)
