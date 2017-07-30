#include <Arduino.h>
#include "requests.h"

// Initialize the Ethernet client library
// with the IP address and port of the server
// that you want to connect to (port 80 is default for HTTP):
EthernetClient client;

void client_setup() {

  client_stop();

  // give the Ethernet shield a second to initialize:
  delay(1000);
  Serial.println("connecting...");

  // if you get a connection, report back via serial:
  if (client.connect(request_server, 80)) {
    Serial.println("connected");
    // Make a HTTP request:
    String get_request = "GET " + RUTA_REQUEST + " HTTP/1.1";
    client.println(get_request);
    client.println("Connection: close");
    client.println();
  } else {
    // if you didn't get a connection to the server:
    Serial.println("connection failed");
  }
}

void client_loop() {
  // if there are incoming bytes available
  // from the server, read them and print them:
  if (client.available() && client.connected()) {
    char c = client.read();
    Serial.print(c);
  }
}

void client_stop() {
  // if the server's disconnected, stop the client:
  if (!client.connected()) {
    Serial.println();
    Serial.println("disconnecting.");
    client.stop();
  }
}
