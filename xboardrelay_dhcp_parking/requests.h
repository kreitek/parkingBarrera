#include <SPI.h>
#include <Ethernet.h>
#include <uHTTP.h>
#include "web.h"
#include "config.h"


// Initialize the Ethernet client library
// with the IP address and port of the server
// that you want to connect to (port 80 is default for HTTP):
extern EthernetClient client;

void client_setup();
void client_loop();
void client_stop();