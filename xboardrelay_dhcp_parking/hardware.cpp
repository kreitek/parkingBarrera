#include <Arduino.h>
#include <Bounce2.h>
#include "hardware.h"
#include "config.h"

// pinout PCB
#define RELE             7   // 8=D8 rele1 y 7=D7 rele2
#define ABRIR_AUTOMATICO A0   // A0 boton que a 0V abre en modo automatico
#define ABRIR_MANUAL     A1   // A1 boton que a 0V abre en modo manual
#define CERRAR           4   // D4 boton que a 0V cierra
#define LUX              5   // D5 fotocelula que a 0V significa que algo está cortando el haz
#define FCA              2   // D2 fin carrera abierta con pullup que a 0V significa que llegó al final
#define FCC              3   // D3 fin carrera cerrada con pullup que a 0V significa que llegó al final

long unsigned int led_millis = 99999999;

// Nota: El equivalente digital de ABRIR_AUTOMATICO y ABRIR_MANUAL (A0 y A1) depende del tipo de arduino (leonardo en este caso)

#define BOUNCE_INTERVAL 50 // milliseconds
Bounce abrir_automatico = Bounce(); 
Bounce abrir_manual = Bounce(); 
Bounce cerrar = Bounce(); 
Bounce lux = Bounce(); 
Bounce fca = Bounce(); 
Bounce fcc = Bounce(); 

void hardware_setup() {
  // salidas
  pinMode(RELE, OUTPUT);

  // entradas, con pullup y bounce2
  pinMode(ABRIR_AUTOMATICO, INPUT_PULLUP);
  abrir_automatico.attach(ABRIR_AUTOMATICO);
  abrir_automatico.interval(BOUNCE_INTERVAL);

  pinMode(ABRIR_MANUAL, INPUT_PULLUP);
  abrir_manual.attach(ABRIR_MANUAL);
  abrir_manual.interval(BOUNCE_INTERVAL);

  pinMode(CERRAR, INPUT_PULLUP);
  cerrar.attach(CERRAR);
  cerrar.interval(BOUNCE_INTERVAL);

  pinMode(LUX, INPUT_PULLUP);
  lux.attach(LUX);
  lux.interval(BOUNCE_INTERVAL);

  pinMode(FCA, INPUT_PULLUP);
  fca.attach(FCA);
  fca.interval(BOUNCE_INTERVAL);

  pinMode(FCC, INPUT_PULLUP);
  fcc.attach(FCC);
  fcc.interval(BOUNCE_INTERVAL);
}

void hardware_loop() {
  abrir_automatico.update();
  abrir_manual.update();
  cerrar.update();
  lux.update();
  fca.update();
  fcc.update();
}

void toggle() {
    if (Serial) Serial.println("Pulso rele");
    digitalWrite(RELE, HIGH);
    delay(300);
    digitalWrite(RELE, LOW);
    delay(300);
}

bool obstaculo() {
  return lux.read() == LOW;
}

bool final_carrera_abierta() {
  return fca.read() == LOW;
}

bool final_carrera_cerrada() {
  return fcc.read() == LOW;
}

bool boton_abrir_automatico() {
  return abrir_automatico.read() == LOW;
}

bool boton_abrir_manual() {
  return abrir_manual.read() == LOW;
}

bool boton_cerrar() {
  return cerrar.read() == LOW;
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
