// La Xboard-relay es equivalente a un "arduino leonardo"
// Hay que instalar uHTTP de
// wget https://github.com/nomadnt/uHTTP/archive/master.zip -O /tmp/uHTTP.zip
// Hay que instalar bounce2 de
// wget https://github.com/thomasfredericks/Bounce2/archive/master.zip -O /tmp/bounce2.zip
// Hay que instalar ArduinoHttpClient de
// wget https://github.com/arduino-libraries/ArduinoHttpClient/archive/master.zip -O /tmp/ArduinoHttpClient.zip


#include <Bounce2.h>
#include "estados.h"
#include "hardware.h"
#include "web.h"
#include "config.h"
#include <avr/wdt.h>
#include <Ethernet.h>


void setup() {
  Serial.begin(57600);
  // while (!Serial) /* wait for serial chip */;
  if (Serial) Serial.println("Iniciando...");
  wdtLoop();
  hardware_setup();
  web_setup();
  wdtLoop();
  // ethernet_ip_setup();  // ip fija
  ethernet_loop(); // para renovar el dhcp lease cuando toque
  wdtLoop();
  delay(2000);
  if (Serial) Serial.println("loop...");
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
          } else if (server.data("abrir_siempre")) {
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
  ethernet_loop(); // para renovar el dhcp lease cuando toque
  estado_loop(); // para chequear sensores y ejecutar maquina de estados
  led_loop(estado_mask()); // para hacer el parpadeo del led

  wdtLoop();
  if (esta_cerrada() && !hayConexion(client)){
    apaga();
    systemReboot();
  }
}

void wdtLoop(){
  wdt_enable(WDTO_8S); // hay que activarlo periodicamente porque el USB lo desactiva
  wdt_reset();
}

void systemReboot(){
  Serial.println(F("El sistema se reiniciará en unos segundos."));
  Serial.flush();
  Serial.end();
  while(true){ // Bucle infinito que hace que salte el wdt
    wdt_enable(WDTO_1S);
    delay(2000); // Aqui deberia saltar el wdt si no lo desactiva el serial
  }
}

bool hayConexion(EthernetClient &client){
  static unsigned long proximo = FRECUENCIA_TEST_SERVIDOR;  // la primera al cabo de varios segundos
  if (millis() < proximo)
    return true;
  proximo = millis() + FRECUENCIA_TEST_SERVIDOR;
  // close any connection before send a new request.
  // This will free the socket on the WiFi shield
  client.stop();
  if (Serial) Serial.print("connecting... ");
  if (client.connect(REQUEST_HOST, 80)) {
    // send the HTTP GET request:
    client.print("GET "); 
    client.print(REQUEST_QUERY); 
	client.print("?mac=");
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
    if (Serial) Serial.println("red bien");
    return true;
  } else {
    if (Serial) Serial.println("red mal");
    return false;
  }
}

// vim: ai:expandtab:ts=2:sw=2
