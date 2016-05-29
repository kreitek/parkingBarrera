#include <Arduino.h>
#include "estados.h"
#include "hardware.h"

#define ABIERTA_LIBRE_ESPERAR 5000

Estado estado = INICIAL;
Orden orden = ORDEN_NINGUNA;
Orden ultima_orden = ORDEN_NINGUNA;
long unsigned int estado_millis = 0;
long unsigned int ultima_orden_millis = 0;
long unsigned int abierta_libre_millis = 0;

bool orden_siguiente(Orden siguiente) {
  orden = siguiente;
  ultima_orden = siguiente;
  ultima_orden_millis = millis();
}

bool estado_siguiente(Estado siguiente) {
  estado = siguiente;
  estado_millis = millis();
}

void estado_loop() {
  // no sobreescribimos orden de la web
  if (orden == ORDEN_NINGUNA) {
    // triggers de botones evitan repetir la orden cuando mantenemos pulsado
    if (trigger_abrir_manual())
      orden_siguiente(ORDEN_ABRIR_MANUAL);
    else if (trigger_abrir_automatico())
      orden_siguiente(ORDEN_ABRIR_AUTOMATICO);
    else if (trigger_cerrar())
      orden_siguiente(ORDEN_CERRAR);
  }

  // atendiendo orden de boton (o de web)
  if (orden == ORDEN_ABRIR_AUTOMATICO)
    estado_siguiente(ABRIENDO_AUTOMATICO);
  else if (orden == ORDEN_ABRIR_MANUAL)
    estado_siguiente(ABRIENDO_MANUAL);
  else if (orden == ORDEN_CERRAR)
    estado_siguiente(CERRANDO_MANUAL);

  // atendida
  orden = ORDEN_NINGUNA; 
  
  // maquina de estados: if transiciones else if transiciones else comando hardware
  switch (estado) {
    case INICIAL:
      if (final_carrera_cerrada())
        estado_siguiente(CERRADA);
      else if (final_carrera_abierta())
        estado_siguiente(ABIERTA_MANUAL);
      else
        estado_siguiente(ABRIENDO_MANUAL);
      break;
      
    case CERRADA:
      apaga();
      break;
      
    case ABRIENDO_AUTOMATICO:
      if (final_carrera_abierta())
        estado_siguiente(ABIERTA_SIN_OCUPAR);
      else
        abre();
      break;
      
    case ABIERTA_SIN_OCUPAR:
      if (obstaculo())
        estado_siguiente(ABIERTA_OCUPADA);
      else
        apaga();
      break;

    case ABIERTA_OCUPADA:
      if (!obstaculo()) {
        estado_siguiente(ABIERTA_LIBRE);
        abierta_libre_millis = millis();
      } else {
        apaga();
      }
      break;

    case ABIERTA_LIBRE:
      if (obstaculo())
        estado_siguiente(ABIERTA_OCUPADA);
      else if (millis() - abierta_libre_millis > ABIERTA_LIBRE_ESPERAR)
        estado_siguiente(CERRANDO_AUTOMATICO);
      else
        apaga();
      break;

    case CERRANDO_AUTOMATICO:
      if (obstaculo()) 
        estado_siguiente(ABRIENDO_AUTOMATICO);
      else if (final_carrera_cerrada())
        estado_siguiente(CERRADA);
      else
        cierra();
      break;

    case ABRIENDO_MANUAL:
      if (final_carrera_abierta())
        estado_siguiente(ABIERTA_MANUAL);
      else
        abre();
      break;
      
    case ABIERTA_MANUAL:
      apaga();
      break;

    case CERRANDO_MANUAL:
      if (obstaculo())
        estado_siguiente(ABRIENDO_MANUAL);
      else if (final_carrera_cerrada())
        estado_siguiente(CERRADA);
      else
        cierra();
      break;
  }
}

const __FlashStringHelper* EstadoStr() {
    switch (estado) {
        case INICIAL:                return F("Inicial");
        case CERRADA:                return F("Cerrada");
        case ABRIENDO_AUTOMATICO:    return F("Abriendo automatico");
        case ABIERTA_SIN_OCUPAR:     return F("Abierta sin ocupar");
        case ABIERTA_OCUPADA:        return F("Abierta ocupada");
        case ABIERTA_LIBRE:          return F("Abierta libre");
        case CERRANDO_AUTOMATICO:    return F("Cerrando automático");
        case ABIERTA_MANUAL:         return F("Abierta manual");
        case ABRIENDO_MANUAL:        return F("Abriendo manual");
        case CERRANDO_MANUAL:        return F("Cerrando manual");
        default:                     return F("[Unknown]");
    }
}

const __FlashStringHelper* OrdenStr() {
    switch (ultima_orden) {
        case ORDEN_NINGUNA:          return F("Ninguna");
        case ORDEN_ABRIR_AUTOMATICO: return F("Abrir automatico");
        case ORDEN_ABRIR_MANUAL:     return F("Abrir manual");
        case ORDEN_CERRAR:           return F("Cerrar");
        default:                     return F("[Unknown]");
    }
}

// vim: ai:expandtab:ts=2:sw=2
