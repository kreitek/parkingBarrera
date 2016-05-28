#include <Arduino.h>
#include "hardware.h"

// pinout
#define RELE1            8   // D8 rele cable motor que a positivo abriria
#define RELE2            7   // D7 rele cable motor que a negativo abriria
#define ABRIR_AUTOMATICO 0   // A0 fotocelula que a 0V significa que algo está cortando el haz
#define ABRIR_MANUAL     1   // A1 fin carrera abierta/abierta con pullup que a 0V significa que llegó al final
#define CERRAR           4   // D4 fin carrera cerrada/cerrada con pullup que a 0V significa que llegó al final
#define LUX              5   // D5 boton que abre en modo automatico
#define FCA              2   // D2 boton que abre en modo manual
#define FCC              3   // D3 boton que cierra

// Nota: El equivalente digital de LUX y FCA (A0 y A1) depende del tipo de arduino (leonardo en este caso)

void hardware_setup() {
  pinMode(RELE1, OUTPUT);
  pinMode(RELE2, OUTPUT);
  pinMode(ABRIR_AUTOMATICO, INPUT_PULLUP);
  pinMode(ABRIR_MANUAL, INPUT_PULLUP);
  pinMode(CERRAR, INPUT_PULLUP);
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

bool rele1() {
  return digitalRead(RELE1) == HIGH;
}

bool rele2() {
  return digitalRead(RELE2) == HIGH;
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
  //return digitalRead(ABRIR_AUTOMATICO) == LOW;
  return analogRead(ABRIR_AUTOMATICO) < 512;
}

bool boton_abrir_manual() {
  //return digitalRead(ABRIR_MANUAL) == LOW;
  return analogRead(ABRIR_MANUAL) < 512;
}

bool boton_cerrar() {
  return digitalRead(CERRAR) == LOW;
}

// vim: ai:expandtab:ts=2:sw=2
