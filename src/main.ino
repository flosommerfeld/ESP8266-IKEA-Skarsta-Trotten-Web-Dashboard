#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>
#include <uri/UriBraces.h>
#include <uri/UriRegex.h>
#include "index.h"
#include <HCSR04.h>

/* WiFi configuration */
#ifndef STASSID
#define STASSID "SSID of the WLAN router" // PUT YOUR "WIFI NAME" HERE
#define STAPSK  "passphrase" // PUT YOUR WIFI PASSWORD HERE
#endif
const char *ssid = STASSID;
const char *password = STAPSK;

/* Pin configuration */
#define motor_driver_pwm D4 // pwm pin for the motor driver board
#define motor_driver_dir D3 // direction pin for the motor driver board
#define motor_speed 255 // speed of the motor from 0-250
#define echoPin D7 // HC-SR04 ultrasonic sensor - Echo Pin 
#define trigPin D6 // HC-SR04 ultrasonic sensor - Trigger Pin

ESP8266WebServer server(80); // server initialisation
HCSR04 hc(trigPin, echoPin); // initialisation HCSR04 (ultrasonic sensor) (trig pin , echo pin)

/* States of the system */
typedef enum {
  UP, // table is supposed to go up
  DOWN, // table is supposed to go down
  HOLD, // table is supposed to do nothing -> hold still
  CUSTOM_HEIGHT
} state_t;

/* Global state of the system. In HOLD by default -> motor will not move in this state */
state_t g_system_state = HOLD; //

/* Maximum and minimum height of the table in cm */
const MAX_HEIGHT = 160;
const MIN_HEIGHT = 60;

/* Global variable which shall hold the wanted custom height when requested */
int g_custom_height;


/* 
 * Displays the index/main page in the browser of the client
 */
void displayIndex() {
 String s = MAIN_page; // read HTML contents
 server.send(200, "text/html", s); // send web page
}


/*
 * Handles calls to the URI /motor/<string: action>/ and does state transitions:
 * if for example /motor/up is called, then the system will transition from the previous state
 * to the state UP. 
 */
void handleMotorRequests() {
  String action = server.pathArg(0); // retrieve the given argument/action

  if(action == "up"){
    g_system_state = UP;
  }
  else if(action == "stop"){
    g_system_state = HOLD;
  }
  else if(action == "down"){
    g_system_state = DOWN;
  }
  else {
    Serial.println("Error: Action is unknown"); // system will stay in its previous state
  }
}


/*
 * Handles calls to the URI /height/<string: height_in_cm>/
 * If a height is given, then the system shall transition into the CUSTOM_HEIGHT state.
 */
void handleHeightRequests() {
   int height = atoi(server.pathArg(0)); // convert string parameter to integer

    // only change the state if the given height is in the height boundaries
    if(height >= MIN_HEIGHT and height <= MAX_HEIGHT) {
      g_custom_height = height; // set the custom height
      g_system_state = CUSTOM_HEIGHT // transition to the custom height state
    }
}


/*
 * Login to the network, setup the server and register URI call handlers.
 */
void setup(void) {
  Serial.begin(115200);

  // Pin setup
  pinMode(motor_driver_pwm, OUTPUT);
  pinMode(motor_driver_dir, OUTPUT);

  // WiFi setup
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);

  // Wait for connection
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  // Print connection info
  Serial.print("Connected to ");
  Serial.println(ssid);
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());

  // Start the mDNS responder for esp8266.local
  if (MDNS.begin("esp8266")) {
    Serial.println("MDNS responder started");
  }

  // Server routes
  server.on(F("/"), displayIndex); // route: /
  server.on(UriBraces("/motor/{}"), handleMotorRequests); // route: /motor/<string: action>/
  server.on(UriBraces("/height/{}"), handleHeightRequests); // route: /height/<string: height_in_cm>/ 

  // Start the server
  server.begin();
  Serial.println("HTTP server started");
}


/*
 * Raise the table
 */
void raiseTable() {
  digitalWrite(motor_driver_dir, HIGH);
  if(MAX_HEIGHT > current_height){
    analogWrite(motor_driver_pwm, motor_speed);
  }
}


/*
 * Lower the table
 */
void lowerTable() {
  digitalWrite(motor_driver_dir, LOW);
  if(MIN_HEIGHT < current_height){
    analogWrite(motor_driver_pwm, motor_speed);
  }
}


/*
 * Stop the table at the current height
 */
void stopTable() {
  analogWrite(motor_driver_pwm, 0);
}


/*
 * Controls the motor based on the system state g_system_state.
 */
void handleOutput(){
  // helper variables to safely identify if we have (approx.) reached the desired custom height
  bool table_too_high = false;
  bool table_too_low = false;

  switch (g_system_state) {
        case UP:
            raiseTable(); // motor go up
            break;
        case DOWN:
            lowerTable(); // motor go down
            break;
        case HOLD:
            stopTable(); // stop the motor
            break;
        case CUSTOM_HEIGHT:
          // check if we've missed the custom height
          if(!table_too_high || !table_too_low){
            if(g_custom_height < current_height){
              // lower the table
              lowerTable();
              table_too_high = true;
            }
            else {
              // raise the table
              raiseTable();
              table_too_low = true;
            }
          }
          else{
            g_system_state = HOLD; // transistion to the hold state to stop the motor
            table_too_high = false;
            table_too_low = false;
          }
            break;
        default:
            // stop the motor
            stopTable();
            break;
    }
}


/*
 * Retrieves the current height of the table by getting the distance of the
 * ultrasonic sensor.
 */
void calculateCurrentHeight(){
  current_height = hc.dist(); // set global variable
}


/*
 * Main loop. Gets the current height, retrieves inputs and do state 
 * transitions and finally control the motor based on the state.
 */
void loop(void) {
  calculateCurrentHeight();
  server.handleClient(); // gets input from a client and also does state transitions
  handleOutput(); // does the output
}
