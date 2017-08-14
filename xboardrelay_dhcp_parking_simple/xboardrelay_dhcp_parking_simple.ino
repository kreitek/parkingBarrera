// La Xboard-relay es equivalente a un "arduino leonardo"
// Hay que instalar uHTTP de
// wget https://github.com/nomadnt/uHTTP/archive/master.zip -O /tmp/uHTTP.zip
// Hay que instalar bounce2 de
// wget https://github.com/thomasfredericks/Bounce2/archive/master.zip -O /tmp/bounce2.zip

#include <Bounce2.h>
#include "estados.h"
#include "hardware.h"
#include "web.h"
#include "config.h"

void setup() {
  Serial.begin(9600);
  hardware_setup();
  web_setup();
  ethernet_ip_setup();
}

void loop() {
  EthernetClient client = server.available();
  if (client) {
    serial_print_connect();
    //serial_print_localip();

    if (server.method(uHTTP_METHOD_GET)) {
      if (server.uri(FOLDER)) {
        webpage_form(client);
      } else {
        not_found(client);
      }
    } else if (server.method(uHTTP_METHOD_POST)) {
      if (server.uri(FOLDER)) {
        if (String(server.data("key")) == PASSWORD) {
          if (server.data("abrir_cerrar")) {
            orden_siguiente(ORDEN_ABRIR_AUTOMATICO);
            redirect(client, FOLDER);
          } else if (server.data("abrir")) {
            orden_siguiente(ORDEN_ABRIR_MANUAL);
            redirect(client, FOLDER);
          } else if (server.data("cerrar")) {
            orden_siguiente(ORDEN_CERRAR);
            redirect(client, FOLDER);
          } else {
            error(client);
          }
        } else {
          unauthorized(client);
        }
      } else {
        not_found(client);
      }
    } else {
      error(client);
    }
  }
  // cosas que hay que ejecutar a cada iteracion
  hardware_loop(); // para renovar los bounces
  // ethernet_loop(); // para renovar el dhcp lease cuando toque
  estado_loop(); // para chequear sensores y ejecutar maquina de estados
  led_loop(estado_mask()); // para hacer el parpadeo del led
}

// vim: ai:expandtab:ts=2:sw=2
