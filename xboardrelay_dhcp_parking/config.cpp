#include "config.h"
#include <EEPROMTyped.h>

// esto es lo que hay que cambiar por cada cosa

byte MAC[6] = {0xDE, 0xAD, 0xBE, 0xEF, 0xEE, 0x39};
const char* HOSTNAME = "p3sx"; 
const char* PASSWORD = "maker";
const char* REQUEST_HOST = "192.168.3.2";   // numeric IP (no DNS)

// de aqui hacia abajo no cambia nada

byte IP[4] = {0, 0, 0, 0};
const int PORT = 80;
const char* FOLDER = "/api/";
const char* REQUEST_QUERY = "/parker/barrera_alive/";
unsigned int T_TEST_SERVIDOR = 30;
unsigned int T_ABIERTA_SIN_ESPERAR = 60;
unsigned int T_ABIERTA_LIBRE_ESPERAR = 3;
unsigned int T_REINTENTA_SIN_FC = 10;
unsigned int T_REINTENTA_CON_FC = 10;

#define ADDR_TEST_SERVIDOR 0
#define ADDR_ABIERTA_SIN_ESPERAR ADDR_TEST_SERVIDOR + EEPROMTyped.sizeOf(T_TEST_SERVIDOR)
#define ADDR_ABIERTA_LIBRE_ESPERAR ADDR_ABIERTA_SIN_ESPERAR + EEPROMTyped.sizeOf(T_ABIERTA_SIN_ESPERAR)
#define ADDR_REINTENTA_SIN_FC ADDR_ABIERTA_LIBRE_ESPERAR + EEPROMTyped.sizeOf(T_ABIERTA_LIBRE_ESPERAR)
#define ADDR_REINTENTA_CON_FC ADDR_REINTENTA_SIN_FC + EEPROMTyped.sizeOf(T_REINTENTA_SIN_FC)
#define ADDR_FIN ADDR_REINTENTA_CON_FC + EEPROMTyped.sizeOf(T_REINTENTA_CON_FC)

void config_default() {
	T_TEST_SERVIDOR = 30;
	T_ABIERTA_SIN_ESPERAR = 60;
	T_ABIERTA_LIBRE_ESPERAR = 3;
	T_REINTENTA_SIN_FC = 10;
	T_REINTENTA_CON_FC = 10;
}

void config_read() {
	EEPROMTyped.read(ADDR_TEST_SERVIDOR, T_TEST_SERVIDOR);
	EEPROMTyped.read(ADDR_ABIERTA_SIN_ESPERAR, T_ABIERTA_SIN_ESPERAR);
	EEPROMTyped.read(ADDR_ABIERTA_LIBRE_ESPERAR, T_ABIERTA_LIBRE_ESPERAR);
	EEPROMTyped.read(ADDR_REINTENTA_SIN_FC, T_REINTENTA_SIN_FC);
	EEPROMTyped.read(ADDR_REINTENTA_CON_FC, T_REINTENTA_CON_FC);
	char t;
	EEPROMTyped.read(ADDR_FIN, t);
	if (t != 'T') {
		config_default();
		config_write();
	}
}

void config_write() {
	EEPROMTyped.write(ADDR_TEST_SERVIDOR, T_TEST_SERVIDOR);
	EEPROMTyped.write(ADDR_ABIERTA_SIN_ESPERAR, T_ABIERTA_SIN_ESPERAR);
	EEPROMTyped.write(ADDR_ABIERTA_LIBRE_ESPERAR, T_ABIERTA_LIBRE_ESPERAR);
	EEPROMTyped.write(ADDR_REINTENTA_SIN_FC, T_REINTENTA_SIN_FC);
	EEPROMTyped.write(ADDR_REINTENTA_CON_FC, T_REINTENTA_CON_FC);
	char t = 'T';
	EEPROMTyped.write(ADDR_FIN, t);
}
