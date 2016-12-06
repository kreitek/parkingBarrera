#include <Arduino.h>
#include "hardware.h"

// pinout PCB
#define RELE1            8   // D8 rele cable motor que a positivo abriria
#define RELE2            7   // D7 rele cable motor que a negativo abriria
#define ABRIR_AUTOMATICO A0   // A0 boton que a 0V abre en modo automatico
#define ABRIR_MANUAL     A1   // A1 boton que a 0V abre en modo manual
#define CERRAR           4   // D4 boton que a 0V cierra
#define LUX              5   // D5 fotocelula que a 0V significa que algo está cortando el haz
#define FCA              2   // D2 fin carrera abierta con pullup que a 0V significa que llegó al final
#define FCC              3   // D3 fin carrera cerrada con pullup que a 0V significa que llegó al final
#define LED              13  // Se asume que el led es logica directa, con resistencia a 0V

long unsigned int led_millis = 99999999;

// Nota: El equivalente digital de ABRIR_AUTOMATICO y ABRIR_MANUAL (A0 y A1) depende del tipo de arduino (leonardo en este caso)

void hardware_setup() {
  pinMode(LED, OUTPUT);
  pinMode(RELE1, OUTPUT);
  pinMode(RELE2, OUTPUT);
  pinMode(ABRIR_AUTOMATICO, INPUT_PULLUP);
  pinMode(ABRIR_MANUAL, INPUT_PULLUP);
  pinMode(CERRAR, INPUT_PULLUP);
  pinMode(LUX, INPUT_PULLUP);
  pinMode(FCA, INPUT_PULLUP);
  pinMode(FCC, INPUT_PULLUP);
}

void led_on() {
  digitalWrite(LED, HIGH);
}

void led_off() {
  digitalWrite(LED, LOW);
}

void led_loop(unsigned int pattern) {
  // el pattern son 16 bits y vamos a definir que 4 bits son 1 segundo
  static unsigned int mask = 0x8000;
  if (millis() - led_millis > 250) {
    if (mask & pattern) 
      led_on();
    else
      led_off();
    mask = mask >> 1;
    if (!mask)
      mask = 0x8000;
    led_millis = millis();
  }
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
  return digitalRead(ABRIR_AUTOMATICO) == LOW;
}

bool boton_abrir_manual() {
  return digitalRead(ABRIR_MANUAL) == LOW;
}

bool boton_cerrar() {
  return digitalRead(CERRAR) == LOW;
}

bool trigger(bool actual, bool &memoria) {
  if (memoria == actual) {
    return false;
  } else {
    memoria = actual;
    return actual;
  }
}

bool trigger_abrir_automatico() {
  static bool memoria = false;
  return trigger(boton_abrir_automatico(), memoria);
}

bool trigger_abrir_manual() {
  static bool memoria = false;
  return trigger(boton_abrir_manual(), memoria);
}

bool trigger_cerrar() {
  static bool memoria = false;
  return trigger(boton_cerrar(), memoria);
}

// vim: ai:expandtab:ts=2:sw=2
