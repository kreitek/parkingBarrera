/* Arduino Parking Barrera
Programa que implementa la maquina de estados preparada
para el funcionamiento logico de la barrera del parking
del Anden Sin Limite */

#include <SoftwareSerial.h>

#define DISPARADO_FINCARRERA HIGH
#define DISPARADO LOW
#define INPUTMODE INPUT_PULLUP

#define MHIGH LOW
#define MLOW HIGH

int pinBtn_S = 2;
int pinBtn_B = 3;
int pinFoto_1 = 4;
int pinFoto_2 = 5;
int pinFin_B = 6;
int pinFin_S = 7;

int pinR2 = 8;
int pinR1 = 9;

int pinExt_B = 10;
int pinExt_S = 11;

// Pines NewSoftwareSerial
int pinRX = 10;
int pinTX = 11;

SoftwareSerial mySerial(pinRX, pinTX); // RX, TX

// VAriables de la Maquina de estados
int estado, estadoOld;  

#define BAJADA 0
#define SUBIENDO 1
#define SUBIDA 2
#define BAJANDO 3

char * ESTADOS_STR[] = {"bajada", "subiendo", "subida", "bajando"};

boolean tiempoS;
boolean tiempoB;

boolean presentaSerie;

void setup () {
  pinMode(pinBtn_S, INPUTMODE);
  pinMode(pinBtn_B, INPUTMODE);
  pinMode(pinFoto_1, INPUTMODE);
  pinMode(pinFoto_2, INPUTMODE);
  pinMode(pinFin_B, INPUTMODE);
  pinMode(pinFin_S, INPUTMODE);
  pinMode(pinExt_B, INPUTMODE);
  pinMode(pinExt_S, INPUTMODE);
  
  pinMode(pinR1, OUTPUT);
  pinMode(pinR2, OUTPUT);
  digitalWrite(pinR1, HIGH);
  digitalWrite(pinR2, HIGH);

  mySerial.begin(57600);
  Serial.begin(9600);
 
  estado = BAJADA;

  tiempoS = false;
  tiempoB = false;
  presentaSerie = true;
}

void loop() {
  if (presentaSerie) {
    printInputs();
    presentaSerie = false;
  }

  if (estado != estadoOld) {
    Serial.print("estado= ");
    Serial.println(estado);
    estadoOld = estado;
  }
  
  switch(estado) {
    case BAJADA: 
        digitalWrite(pinR1, MLOW);
        digitalWrite(pinR2, MLOW);
        if (digitalRead(pinBtn_S) == DISPARADO) {
          estado = SUBIENDO;
          presentaSerie = true;
        }
        break;
    case SUBIENDO:
        digitalWrite(pinR1, MHIGH);
        digitalWrite(pinR2, MLOW);
        if (digitalRead(pinFin_S) == DISPARADO_FINCARRERA || tiempoS) {
          estado = SUBIDA;
          presentaSerie = true;
        }
        break;
    case SUBIDA: 
        digitalWrite(pinR1, MLOW);
        digitalWrite(pinR2, MLOW);
        if (digitalRead(pinBtn_B) == DISPARADO && \
            !digitalRead(pinFoto_1) == DISPARADO && \
            !digitalRead(pinFoto_2) == DISPARADO) {
          estado = BAJANDO;
          presentaSerie = true;
        }
        break;
    case BAJANDO:
        digitalWrite(pinR1, MLOW);
        digitalWrite(pinR2, MHIGH);
        if (digitalRead(pinFoto_1) == DISPARADO || \
            digitalRead(pinFoto_2) == DISPARADO || \
            digitalRead(pinBtn_S) == DISPARADO)
          estado = SUBIENDO;
        if (digitalRead(pinFin_B) == DISPARADO_FINCARRERA || tiempoB) {
          estado = BAJADA;
          presentaSerie = true;
        }
        break;
  }

  serialCmd();
} 

void serialCmd() {

  while(Serial.available() > 0) {
    switch (Serial.read()) {
      case 'I':  // print inputs
          printInputs();
          break;
      case 'E':  // echo
          Serial.print("[OK]\n");
          break;
      default:
          // Nada
          break;
    }
  }
}	

void printInputs() {
  Serial.print("Estado: ");
  Serial.println(ESTADOS_STR[estado]);
  printInput(pinBtn_S, DISPARADO);
  printInput(pinBtn_B, DISPARADO);
  printInput(pinFoto_1, DISPARADO);
  printInput(pinFoto_2, DISPARADO);
  printInput(pinFin_B, DISPARADO_FINCARRERA);
  printInput(pinFin_S, DISPARADO_FINCARRERA);
  printInput(pinExt_B, DISPARADO);
  printInput(pinExt_S, DISPARADO);
  Serial.println();
}

void printInput(int pinX, int d) {
  Serial.print("Input ");
  Serial.print(pinX);
  Serial.print(" : ");
  int v = digitalRead(pinX);
  if (v == d)
    Serial.println("PULSADO");
  else
    Serial.println("-------");
}
