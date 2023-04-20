#include <Arduino.h>
#include "config.h"
#include "estados.h"
#include "hardware.h"

Estado estado = INICIAL;
bool automatica = true;
Orden orden = ORDEN_NINGUNA;
Orden ultima_orden = ORDEN_NINGUNA;
long unsigned int estado_millis = 0;
long unsigned int ultima_orden_millis = 0;

unsigned int tiempo_en_estado() {
	return (millis() - estado_millis) / 1000;
}

void orden_siguiente(Orden siguiente) {
  orden = siguiente;
  ultima_orden = siguiente;
  ultima_orden_millis = millis();
  automatica = (siguiente != ORDEN_ABRIR_MANUAL);
  if (Serial) { Serial.print("orden="); Serial.println(OrdenStr()); }
}

void estado_siguiente(Estado siguiente) {
  estado = siguiente;
  estado_millis = millis();
  if (Serial) { 
	Serial.print("estado="); 
    Serial.print(EstadoStr()); 
	Serial.print(" automatica="); 
    Serial.println(automatica); 
  }
  if (siguiente == ABRIENDO_PULSO || siguiente == CERRANDO_PULSO)
	toggle();
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
  if (orden == ORDEN_ABRIR_AUTOMATICO and not final_carrera_abierta())
    estado_siguiente(ABRIENDO);
  else if (orden == ORDEN_ABRIR_MANUAL and not final_carrera_abierta()) 
    estado_siguiente(ABRIENDO);
  else if (orden == ORDEN_CERRAR and not final_carrera_cerrada())
    estado_siguiente(CERRANDO);

  // atendida
  orden = ORDEN_NINGUNA;

  // maquina de estados: if transiciones else if transiciones else comando hardware
  switch (estado) {
    case INICIAL:
      if (final_carrera_cerrada())
        estado_siguiente(CERRADA);
      else if (final_carrera_abierta())
        estado_siguiente(ABIERTA);
      else
        estado_siguiente(CERRANDO_PULSO);
      break;

    case CERRADA:
      break;

    case ABRIENDO_PULSO:
	  if (!final_carrera_abierta() && !final_carrera_cerrada())
        estado_siguiente(ABRIENDO);
      else if (T_REINTENTA_SIN_FC && tiempo_en_estado() > T_REINTENTA_SIN_FC)
        estado_siguiente(ABRIENDO_PULSO);
      break;

    case ABRIENDO:
      if (final_carrera_abierta())
        estado_siguiente(ABIERTA);
      else if (final_carrera_cerrada())
        estado_siguiente(ABRIENDO_PULSO);
      else if (obstaculo())
        estado_siguiente(ABRIENDO_OCUPADA);
      else if (T_REINTENTA_CON_FC && tiempo_en_estado() > T_REINTENTA_CON_FC)
        estado_siguiente(ABRIENDO_PULSO);
      break;

    case ABRIENDO_OCUPADA:
      if (final_carrera_abierta())
        estado_siguiente(ABIERTA_OCUPADA);
      else if (final_carrera_cerrada())
        estado_siguiente(ABRIENDO_PULSO);
      else if (T_REINTENTA_CON_FC && tiempo_en_estado() > T_REINTENTA_CON_FC)
        estado_siguiente(ABRIENDO_PULSO);
      break;

    case ABIERTA:
      if (automatica && obstaculo())
        estado_siguiente(ABIERTA_OCUPADA);
      else if (automatica && T_ABIERTA_SIN_ESPERAR && tiempo_en_estado() > T_ABIERTA_SIN_ESPERAR)
        estado_siguiente(CERRANDO_PULSO);
      break;

    case ABIERTA_OCUPADA:
      if (!automatica && !obstaculo()) {
        estado_siguiente(ABIERTA);
      } else if (automatica && !obstaculo()) {
        estado_siguiente(ABIERTA_LIBRE);
      }
      break;

    case ABIERTA_LIBRE:
      if (obstaculo())
        estado_siguiente(ABIERTA_OCUPADA);
      else if (T_ABIERTA_LIBRE_ESPERAR && tiempo_en_estado() > T_ABIERTA_LIBRE_ESPERAR)
        estado_siguiente(CERRANDO_PULSO);
      break;

    case CERRANDO_PULSO:
	  if (!final_carrera_abierta() && !final_carrera_cerrada())
        estado_siguiente(CERRANDO);
      else if (T_REINTENTA_SIN_FC && tiempo_en_estado() > T_REINTENTA_SIN_FC)
        estado_siguiente(CERRANDO_PULSO);
      break;

    case CERRANDO:
      if (final_carrera_cerrada())
        estado_siguiente(CERRADA);
      else if (final_carrera_abierta())
        estado_siguiente(CERRANDO_PULSO);
      else if (T_REINTENTA_CON_FC && tiempo_en_estado() > T_REINTENTA_CON_FC)
        estado_siguiente(CERRANDO_PULSO);
      break;
  }
}

bool esta_cerrada(){
  return estado == CERRADA;
}

const __FlashStringHelper* EstadoStr() {
    switch (estado) {
        case INICIAL:                return F("Inicial");
        case CERRADA:                return F("Cerrada");
        case ABRIENDO_PULSO:    	 return F("Pulso apertura");
        case ABRIENDO:    			 return F("Abriendo");
        case ABRIENDO_OCUPADA:    	 return F("Abriendo ocupada");
        case ABIERTA:                return F("Abierta");
        case ABIERTA_OCUPADA:        return F("Abierta ocupada");
        case ABIERTA_LIBRE:          return F("Abierta libre");
        case CERRANDO_PULSO:    	 return F("Pulso cierre");
        case CERRANDO:    			 return F("Cerrando");
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

#ifdef CON_TIMBRE

bool timbre_web(EthernetClient &client){
  // close any connection before send a new request.
  // This will free the socket on the WiFi shield
  client.stop();
  if (Serial) Serial.println("connecting timbre");
  if (client.connect(REQUEST_HOST, 80)) {
    // send the HTTP GET request:
    client.print("GET /parker/timbre/"); 
	for(unsigned int i=0; i<6; i++) {
		client.print(MAC[i]/16, HEX);
		client.print(MAC[i]%16, HEX);
    }
	client.print("&t="); client.print(millis()/1000); 
    client.println(" HTTP/1.1");
    client.print("Host: "); client.println(REQUEST_HOST);
    client.println("User-Agent: arduino-ethernet");
    client.println("Connection: close");
    client.println();
    client.stop();
  }
}

void timbre_loop(EthernetClient &client) {
	if (trigger_abrir_automatico()) {
        if (boton_abrir_automatico()) {
            Serial.println("Timbre!");
            timbre_web(client);
        }
	}
}
#endif

// vim: ai:expandtab:ts=2:sw=2
