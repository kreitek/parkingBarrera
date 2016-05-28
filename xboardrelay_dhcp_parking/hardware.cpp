#include <Arduino.h>
#include "hardware.h"

// pinout
#define RELE1 8              // D8 rele cable motor que a positivo abriria
#define RELE2 7              // D7 rele cable motor que a negativo abriria
#define LUX 23               // A0 fotocelula que a 0V significa que algo está cortando el haz
#define FCA 24               // A1 fin carrera abierta/abierta con pullup que a 0V significa que llegó al final
#define FCC 4                // D4 fin carrera cerrada/cerrada con pullup que a 0V significa que llegó al final
#define ABRIR_AUTOMATICO 5   // D5 boton que abre en modo automatico
#define ABRIR_MANUAL 2       // D2 boton que abre en modo manual
#define CERRAR 3             // D3 boton que cierra

void hardware_setup() {
  pinMode(RELE1, OUTPUT);
  pinMode(RELE2, OUTPUT);
  pinMode(LUX, INPUT_PULLUP);
  pinMode(FCA, INPUT_PULLUP);
  pinMode(FCC, INPUT_PULLUP);
}

void apaga() {
  digitalWrite(RELE1, LOW);
  digitalWrite(RELE2, LOW);
}

void abre() {
  digitalWrite(RELE1, HIGH);
  digitalWrite(RELE2, LOW);
}

void cierra() {
  digitalWrite(RELE1, LOW);
  digitalWrite(RELE2, HIGH);
}

const __FlashStringHelper* rele1_str() {
  return (digitalRead(RELE1) == HIGH) ? F("encendido") : F("apagado");
}

const __FlashStringHelper* rele2_str() {
  return (digitalRead(RELE2) == HIGH) ? F("encendido") : F("apagado");
}

bool obstaculo() {
  return digitalRead(LUX) == LOW;
}

bool final_carrera_abierta() {
  return digitalRead(FCA) == LOW;
}

bool final_carrera_cerrada() {
  return digitalRead(FCC) == LOW;
}

bool boton_abrir_automatico() {
  return digitalRead(ABRIR_AUTOMATICO) == LOW;
}

bool boton_abrir_manual() {
  return digitalRead(ABRIR_MANUAL) == LOW;
}

bool boton_cerrar() {
  return digitalRead(CERRAR) == LOW;
}

// vim: ai:expandtab:ts=2:sw=2
