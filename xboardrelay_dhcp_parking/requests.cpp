#include <Arduino.h>
#include "requests.h"

// Initialize the Ethernet client library
// with the IP address and port of the server
// that you want to connect to (port 80 is default for HTTP):
EthernetClient client;

void client_setup() {

  // give the Ethernet shield a second to initialize:
  // delay(1000);
  Serial.println("connecting...");
  
  HttpClient http(client, REQUEST_HOST, 80);
  int err = http.get(REQUEST_QUERY);
  if (err)
    Serial.println("connection failed");
  else
    Serial.println("connected");
  int status_code = http.responseStatusCode();
  if (status_code == 200)
    Serial.println("OK");
  else {
    Serial.print("return error code ");
    Serial.println(status_code, DEC);
  }

  // String body = http.responseBody();
  http.stop();

}

void client_loop() {
  // if there are incoming bytes available
  // from the server, read them and print them:
  if (client.available()) {
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
