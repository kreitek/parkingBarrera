#include <Arduino.h>
#include "web.h"
#include "estados.h"
#include "hardware.h"
#include "config.h"

uHTTP server = uHTTP(PORT);

bool tiene_ip = false;
long unsigned int ethernet_millis = 99999999; // asegura que cargue dhcp al iniciar (si millis() == 0)

void web_setup() {
  server.begin();
}

void web_time(EthernetClient &client, long unsigned int time_millis) {
  long unsigned int segundos = (millis()-time_millis)/1000;
  long unsigned int minutos = segundos / 60;
  unsigned int horas = minutos / 60;
  unsigned int dias = horas / 24;
  segundos = segundos % 60;
  minutos = minutos % 60;
  horas = horas % 24;

  if (dias > 0) {
    client.print(dias);
    client.print(F("d "));
  }
  if (horas < 10)
    client.print(F("0"));
  client.print(horas);
  client.print(":");
  if (minutos < 10)
    client.print(F("0"));
  client.print(minutos);
  client.print(F(":"));
  if (segundos < 10)
    client.print(F("0"));
  client.print(segundos);
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

  client.print(F("<h2>Status</h2>"));

  client.print(F("<ul>"));

  client.print(F("<li>Estado: "));
  client.print(EstadoStr());
  client.print(F(" hace "));
  web_time(client, estado_millis);
  client.print(F("</li>"));

  client.print(F("<li>Ultima orden: "));
  client.print(OrdenStr());
  client.print(F(" hace "));
  web_time(client, ultima_orden_millis);
  client.print(F("</li>"));

  client.print(F("<li>Uptime: "));
  web_time(client, 0);
  client.print(F("</li>"));

  client.print(F("<li>HW Addr: "));
  for(unsigned int i=0; i<6; i++) {
    client.print(MAC[i]/16, HEX);
    client.print(MAC[i]%16, HEX);
    client.print(i==5 ? F("</li>") : F(":")); 
  }

  client.print(F("</ul>"));

  client.print(F("<h2>Outputs</h2>"));

  client.print(F("<ul>"));

  client.print(F("<li>D7 Rele1 (abrir): "));
  client.print(rele1());
  client.print(F("</li>"));

  client.print(F("<li>D8 Rele2 (cerrar): "));
  client.print(rele2());
  client.print(F("</li>"));

  client.print(F("</ul>"));

  client.print(F("<h2>Inputs</h2>"));

  client.print(F("<ul>"));

  client.print(F("<li>A0 Boton abrir automatico: "));
  client.print(boton_abrir_automatico());
  client.print(F("</li>"));

  client.print(F("<li>A1 Boton abrir manual: "));
  client.print(boton_abrir_manual());
  client.print(F("</li>"));

  client.print(F("<li>D4 Boton cerrar: "));
  client.print(boton_cerrar());
  client.print(F("</li>"));

  client.print(F("<li>D5 Obstaculo: "));
  client.print(obstaculo());
  client.print(F("</li>"));

  client.print(F("<li>D2 Final carrera abierta: "));
  client.print(final_carrera_abierta());
  client.print(F("</li>"));

  client.print(F("<li>D3 Final carrera cerrada: "));
  client.print(final_carrera_cerrada());
  client.print(F("</li>"));
  client.print(F("</ul>"));

  client.print(F("<h2>Actions</h2>"));

  client.print(F("<button onclick=\"location.reload()\">Refrescar</button>"));

  client.print(F( \
    "<form method=POST>" \
    "<div>" \
    "  <input type=submit name=abrir_cerrar value=\"abrir automatico\">" \
    "  <input type=submit name=abrir value=\"abrir siempre\">" \
    "  <input type=submit name=cerrar value=\"cerrar\">" \
    "</div>" \
    "<div>Password <input type=password name=key></div>" \
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
  if (!tiene_ip) {
    if (millis() - ethernet_millis > 30000)
      if (Ethernet.begin(MAC)) {
        serial_print_localip();
        tiene_ip = true;
      } else {
        Serial.println(F("Failed to configure Ethernet using dhcp. Sleeping 30sg..."));
        ethernet_millis = millis();
      }
  } else {
    // Esta funcion solo da verbosidad a la llamada
    // Ethernet.maintain() y se podria sustituir por ella.
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
