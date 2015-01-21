/* Arduino Parking Barrera
Programa que implementa la maquina de estados preparada
para el funcionamiento logico de la barrera del parking
del Anden Sin Limite */

#include <SoftwareSerial.h>

#define DISPARADO HIGH

int pinBtn_S = 2;
int pinBtn_B = 3;
int pinFoto_1 = 4;
int pinFoto_2 = 5;
int pinFin_B = 6;
int pinFin_S = 7;
int pinExt_B = 8;
int pinExt_S = 9;

int pinR1 = 12;
int pinR2 = 13;

// Pines NewSoftwareSerial
int pinRX = 10;
int pinTX = 11;

SoftwareSerial mySerial(pinRX, pinTX); // RX, TX

// VAriables de la Maquina de estados
int estado;  

#define BAJADA 0
#define SUBIENDO 1
#define SUBIDA 2
#define BAJANDO 3

boolean tiempoS;
boolean tiempoB;

void setup () {
  pinMode(pinBtn_S, INPUT);
  pinMode(pinBtn_B, INPUT);
  pinMode(pinFoto_1, INPUT);
  pinMode(pinFoto_2, INPUT);
  pinMode(pinFin_B, INPUT);
  pinMode(pinFin_S, INPUT);
  pinMode(pinExt_B, INPUT);
  pinMode(pinExt_S, INPUT);

  mySerial.begin(57600);
  Serial.begin(9600);
 
  estado = BAJADA;

  tiempoS = false;
  tiempoB = false;
}


void loop() {

  switch(estado) {
    case BAJADA: 
        digitalWrite(pinR1, LOW);
        digitalWrite(pinR2, LOW);
        if (digitalRead(pinBtn_S) == DISPARADO)
          estado = SUBIENDO;
        break;
    case SUBIENDO:
        digitalWrite(pinR1, HIGH);
        digitalWrite(pinR2, LOW);
        if (digitalRead(pinFin_S) == DISPARADO || tiempoS)
          estado = SUBIDA;
        break;
    case SUBIDA: 
        digitalWrite(pinR1, LOW);
        digitalWrite(pinR2, LOW);
        if (digitalRead(pinBtn_B) == DISPARADO && \
            !digitalRead(pinFoto_1) == DISPARADO && \
            !digitalRead(pinFoto_2) == DISPARADO)
          estado = BAJANDO;
        break;
    case BAJANDO:
        digitalWrite(pinR1, LOW);
        digitalWrite(pinR2, HIGH);
        if (digitalRead(pinFoto_1 == DISPARADO) || \
            digitalRead(pinFoto_2 == DISPARADO) || \
            digitalRead(pinBtn_S == DISPARADO))
          estado = SUBIENDO;
        if (digitalRead(pinFin_B == DISPARADO || tiempoB))
          estado = BAJADA;
        break;
  }

  serialCmd();
} 

void serialCmd() {

  while(Serial.available() > 0) {
    switch (Serial.read()) {
      case '\n':
          Serial.print('[.]\n');
          break;
      default:
          // Nada
          break;
    }
  }
}	


