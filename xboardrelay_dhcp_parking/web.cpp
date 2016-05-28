#include <Arduino.h>
#include "web.h"
#include "estados.h"
#include "hardware.h"
#include "config.h"

uHTTP server = uHTTP(PORT);

void ethernet_setup() {
  while(Ethernet.begin(MAC) == 0) {
    Serial.println(F("Failed to configure Ethernet using dhcp. Sleeping 30sg..."));
    delay(30000);
  }
  serial_print_localip();

  server.begin();
}

void webpage_form(EthernetClient &client) {
  client.println(F("HTTP/1.1 200 OK"));
  client.println(F("Content-Type: text/html"));
  client.println();

  client.println(F("<!DOCTYPE HTML>"));
  client.print(F("<html>"));

  client.print(F("<h1>xboard relay '"));
  client.print(HOSTNAME);
  client.print(F("'</h1>"));

  client.print(F("<p>uptime "));
  client.print(millis()/1000);
  client.print(F(" sg</p>"));

  client.print(F("<p>mac "));
  for(unsigned int i=0; i<6; i++) {
    client.print(MAC[i]/16, HEX);
    client.print(MAC[i]%16, HEX);
    client.print(i==5 ? F("</p>") : F(":")); 
  }

  client.print(F("<p>Rele1: "));
  client.print(rele1());
  client.print(F("</p>"));

  client.print(F("<p>Rele2: "));
  client.print(rele2());
  client.print(F("</p>"));

  client.print(F("<p>A0 Boton abrir automatico: "));
  client.print(boton_abrir_automatico());
  client.print(F("</p>"));

  client.print(F("<p>A1 Boton abrir manual: "));
  client.print(boton_abrir_manual());
  client.print(F("</p>"));

  client.print(F("<p>D4 Boton cerrar: "));
  client.print(boton_cerrar());
  client.print(F("</p>"));

  client.print(F("<p>D5 Obstaculo: "));
  client.print(obstaculo());
  client.print(F("</p>"));

  client.print(F("<p>D2 Final carrera abierta: "));
  client.print(final_carrera_abierta());
  client.print(F("</p>"));

  client.print(F("<p>D3 Final carrera cerrada: "));
  client.print(final_carrera_cerrada());
  client.print(F("</p>"));

  client.print(F("<p>Estado: "));
  client.print(EstadoStr());
  client.print(F("</p>"));

  client.print(F("<p>Orden: "));
  client.print(OrdenStr());
  client.print(F("</p>"));


  client.print(F( \
    "<form method=POST>" \
    "<div>Password <input type=password name=key></div>" \
    "<div>" \
    "  <input type=submit name=abrir_cerrar value=\"abrir automatico\">" \
    "  <input type=submit name=abrir value=\"abrir siempre\">" \
    "  <input type=submit name=cerrar value=\"cerrar\">" \
    "</div>" \
    "</form>"));

  client.print(F("</html>"));
  client.stop();
  Serial.println(" Mostrado formulario");
}

void redirect(EthernetClient &client, String uri) {
  client.println(F("HTTP/1.1 302 Moved Temporarily"));
  client.print(F("Location: "));
  client.println(uri);
  client.println();
  client.stop();
  Serial.println(" Redirect " + uri);
}

void http_response(EthernetClient &client, String message) {
  client.print(F("HTTP/1.1 "));
  client.println(message);
  client.println(F("Content-Type: text/html"));                                                                                                
  client.println();

  client.println(F("<html>"));
  client.println(HOSTNAME);
  client.println(message);
  client.println(F("<a href=\"javascript:history.back()\">(volver)</a>"));
  client.println(F("</html>"));
  client.stop();
  Serial.println(" Response " + message);
}

void unauthorized(EthernetClient &client) {
  http_response(client, F("401 Unauthorized"));
}

void not_found(EthernetClient &client) {
  http_response(client, F("404 Not found"));
}

void error(EthernetClient &client) {
  http_response(client, F("500 Internal server error"));
}

void ethernet_loop() {
  // Esta funcion solo da verbosidad a la llamada Ethernet.maintain()
  // y se podria sustituir por ella.
  switch (Ethernet.maintain()) {
    case 1:
      Serial.println(F("Error: renewed fail"));
      break;
    case 2:
      Serial.println(F("Renewed success"));
      serial_print_localip();
      break;
    case 3:
      Serial.println(F("Error: rebind fail"));
      break;
    case 4:
      Serial.println(F("Rebind success"));
      serial_print_localip();
      break;
    default:
      break; //nothing happened
  }
}

void serial_print_connect() {
  Serial.print(F("["));
  Serial.print(millis());
  Serial.print(F("] \""));
  Serial.print(server.uri());
  if (server.query()) {
    Serial.print(F("?"));
    Serial.print(server.query());
  }
  Serial.print(F("\""));
}

void serial_print_localip()
{
  Serial.print("Address ");
  Serial.print(Ethernet.localIP());
  Serial.print(":");
  Serial.println(PORT);
}

// vim: ai:expandtab:ts=2:sw=2
