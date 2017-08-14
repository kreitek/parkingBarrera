#include <Arduino.h>
#include "estados.h"
#include "hardware.h"

#define ABIERTA_LIBRE_ESPERAR 3000
#define ABIERTA_MANUAL_ESPERAR 600
#define ESPERAR_ON 1000

Estado estado = INICIAL;
Orden orden = ORDEN_NINGUNA;
Orden ultima_orden = ORDEN_NINGUNA;
long unsigned int estado_millis = 0;
long unsigned int ultima_orden_millis = 0;
long unsigned int abierta_libre_millis = 0;
long unsigned int abierta_manual_millis = 0;

bool orden_siguiente(Orden siguiente) {
  orden = siguiente;
  ultima_orden = siguiente;
  ultima_orden_millis = millis();
}

bool estado_siguiente(Estado siguiente) {
  estado = siguiente;
  estado_millis = millis();
}

bool esperar_estado() {
  return (millis() - estado_millis) > ESPERAR_ON;
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
  else if (orden == ORDEN_ABRIR_MANUAL) {
    estado_siguiente(ABRIENDO_MANUAL);
  }
  else if (orden == ORDEN_CERRAR)
    estado_siguiente(CERRANDO_MANUAL);

  // atendida
  orden = ORDEN_NINGUNA;

  // maquina de estados: if transiciones else if transiciones else comando hardware
  switch (estado) {
    case INICIAL:
      // iniciamos barrera y no hacemos nada
      apaga();
      break;

    case CERRADA:
      apaga();
      break;

    case ABRIENDO_AUTOMATICO:
      if (esperar_estado())
        estado_siguiente(ABIERTA_SIN_OCUPAR);
      else
        abre();
      break;

    case REABRIENDO_AUTOMATICO:
      if (esperar_estado())
        estado_siguiente(ABIERTA_OCUPADA);
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
        estado_siguiente(REABRIENDO_AUTOMATICO);
      else if (esperar_estado())
        estado_siguiente(CERRADA);
      else
        cierra();
      break;

    case ABRIENDO_MANUAL:
      if (esperar_estado())
        estado_siguiente(ABIERTA_MANUAL);
      else
        abre();
      break;

    case ABIERTA_MANUAL:
      abre();
      if (obstaculo()) {
        abierta_manual_millis = millis();
        estado_siguiente(ABIERTA_MANUAL_OCUPADA);
      }
      break;

    case ABIERTA_MANUAL_OCUPADA:
      if (millis() - abierta_manual_millis > ABIERTA_MANUAL_ESPERAR) {
        estado_siguiente(ABIERTA_MANUAL_A_LIBERAR);
      }
      if (!obstaculo())
        estado_siguiente(ABIERTA_MANUAL);
      break;

    case ABIERTA_MANUAL_A_LIBERAR:
      if (!obstaculo())
        estado_siguiente(ABIERTA_MANUAL);
      break;


    case CERRANDO_MANUAL:
      if (obstaculo())
        estado_siguiente(ABRIENDO_MANUAL);
      else if (esperar_estado()) 
        estado_siguiente(CERRADA);
      else
        cierra();
      break;
  }
}

#define RAYA B0111
#define PUNTO B0010
#define MORSE(x, y, z) ((x*256) + (y*16) + z)

unsigned int estado_mask() {
  switch (estado) {
      case INICIAL:                return 0xff00;
      case CERRADA:                return MORSE(PUNTO, PUNTO, PUNTO);

      case ABIERTA_MANUAL:         return MORSE(RAYA, PUNTO, PUNTO);
      case ABIERTA_LIBRE:          return MORSE(RAYA, PUNTO, RAYA);
      case ABIERTA_SIN_OCUPAR:     return MORSE(RAYA, RAYA, PUNTO);
      case ABIERTA_OCUPADA:        return MORSE(RAYA, RAYA, RAYA);

      case ABRIENDO_MANUAL:        return MORSE(PUNTO, PUNTO, RAYA);
      case ABRIENDO_AUTOMATICO:    return MORSE(PUNTO, RAYA, PUNTO);
      case REABRIENDO_AUTOMATICO:  return MORSE(PUNTO, RAYA, PUNTO);
      case CERRANDO_MANUAL:        return MORSE(PUNTO, RAYA, RAYA);
      case CERRANDO_AUTOMATICO:    return MORSE(PUNTO, RAYA, RAYA);
      default:                     return 0;
  }
}

const __FlashStringHelper* EstadoStr() {
    switch (estado) {
        case INICIAL:                return F("Inicial");
        case CERRADA:                return F("Cerrada");
        case ABRIENDO_AUTOMATICO:    return F("Abriendo automatico");
        case REABRIENDO_AUTOMATICO:  return F("Reabriendo automatico");
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
