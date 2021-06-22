#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>

#include <uri/UriBraces.h>
#include <uri/UriRegex.h>

#include "index.h"

#ifndef STASSID
#define STASSID "SSID of the WLAN router"
#define STAPSK  "passphrase"
#endif

const char *ssid = STASSID;
const char *password = STAPSK;

ESP8266WebServer server(80);


// Displays the index/main page in the browser of the client
void displayIndex() {
 String s = MAIN_page; //Read HTML contents
 server.send(200, "text/html", s); //Send web page
}


void setup(void) {
  Serial.begin(115200);
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  Serial.println("");

  // Wait for connection
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.print("Connected to ");
  Serial.println(ssid);
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());

  if (MDNS.begin("esp8266")) {
    Serial.println("MDNS responder started");
  }

  server.on(F("/"), []() {
    displayIndex();
  });

  // Handles calls to the URI /motor/<string: action>/
  server.on(UriBraces("/motor/{}"), []() {
    String action = server.pathArg(0);
    Serial.println("/motor/" + action + " was called");

    if(action == "up"){
      Serial.println("Motor will go up - until stopped");
    }
    else if(action == "stop"){
      Serial.println("Motor will stop");
    }
    else if(action == "down"){
      Serial.println("Motor will go down - until stopped");
    }
    else {
      Serial.println("Error: Action is unknown");
    }
  });


  // Handles calls to the URI /mode/<string: mode>/ Note: would be better if this was an integer but I don't think thats possible..
  server.on(UriBraces("/mode/{}"), []() {
    String modeId = server.pathArg(0);
    Serial.println("/mode/" + modeId + " was called");

    if(modeId == "1"){
      Serial.println("Motor will go to 100 cm");
    }
    else if(modeId == "2"){
      Serial.println("Motor will go to 125 cm");
    }
    else if(modeId == "3"){
      Serial.println("Motor will go to 150 cm");
    }
    else {
      Serial.println("Error: Mode is unknown");
    }
  });


  server.begin();
  Serial.println("HTTP server started");
}


void loop(void) {
  server.handleClient();
}
