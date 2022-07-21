#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>
#include <uri/UriBraces.h>
#include <uri/UriRegex.h>
#include "index.h"
#include <HCSR04.h>
#include "CytronMotorDriver.h"

#define DEBUG

/* WiFi configuration */
#ifndef STASSID
#define STASSID "SSID of the WLAN router" // PUT YOUR "WIFI NAME" HERE
#define STAPSK  "passphrase" // PUT YOUR WIFI PASSWORD HERE
#define OPT_HOSTNAME  "table" // Optional hostname
#endif
const char *SSID = STASSID;
const char *PASSWORD = STAPSK;
const char *HOSTNAME = OPT_HOSTNAME;

/* Pin configuration */
#define MOTOR_DRIVER_PWM D4 // pwm pin for the motor driver board
#define MOTOR_DRIVER_DIR D3 // direction pin for the motor driver board
#define MOTOR_SPEED 255 // speed of the motor from 0-250
#define ULTRASONIC_SENSOR_ECHO_PIN D7 // HC-SR04 ultrasonic sensor - Echo Pin 
#define ULTRASONIC_SENSOR_TRIGGER_PIN D6 // HC-SR04 ultrasonic sensor - Trigger Pin

/* Configure ESP8266 web server */
ESP8266WebServer server(80); // use port 80

/* Configure the motor driver */
CytronMD motor(PWM_DIR, MOTOR_DRIVER_PWM, MOTOR_DRIVER_DIR);

/* Configure ultrasonic sensor */
HCSR04 hc(ULTRASONIC_SENSOR_TRIGGER_PIN, ULTRASONIC_SENSOR_ECHO_PIN); // initialisation HCSR04 (ultrasonic sensor) (trig pin , echo pin)

/* States of the system */
typedef enum {
  UP, // table is supposed to go up
  DOWN, // table is supposed to go down
  HOLD, // table is supposed to do nothing -> hold still
  CUSTOM_HEIGHT // table goes up/down and holds as soon as it reached the custom height
} state_t;

/* Global state of the system. In HOLD by default -> motor will not move in this state */
state_t g_system_state = HOLD; //

/* Maximum and minimum height of the table in cm */
const unsigned int MOTOR_CASE_HEIGHT = 5; /* 5 cm */
const unsigned int TABLE_THICKNESS = 2; /* approx. 2 cm */
const unsigned int HEIGHT_DIFFERENCE = MOTOR_CASE_HEIGHT + TABLE_THICKNESS;
const unsigned int MAX_HEIGHT = 120;/* 120 cm is the offical maximum height from the IKEA manual */
const unsigned int MIN_HEIGHT = 70; /* 70 cm is the offical minimum height from the IKEA manual */

/* Height tolerance (in cm) which is needed because the ultrasonic sensor is not really accurate */
const unsigned int HEIGHT_TOLERANCE = 2; /* This used to be 5 cm but was reduced to 2 because of how slow the table motor is */

/* Global variable which shall hold the wanted custom height when requested */
int g_custom_height;


/* 
 * Displays the index/main page in the browser of the client
 */
void display_index() {
 String s = MAIN_page; // read HTML contents from the MAIN_page variable which was stored in the flash (program) memory instead of SRAM, see index.h
 server.send(200, "text/html", s); // send web page
}


/*
 * The server sends a redirection response to the client so it will go back to the homepage after requesting a state change,
 * e.g. when motor up was clicked it shall go back to the homepage
 */
void send_homepage_redirection() {
  server.sendHeader("Location","/"); // Client shall redirect to "/"
  server.send(303);
}


/*
 * Handles calls to the URI /motor/<string: action>/ and does state transitions:
 * if for example /motor/up is called, then the system will transition from the previous state
 * to the state UP. 
 */
void handle_motor_requests() {
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

  // send response
  send_homepage_redirection();
}


/*
 * Handles calls to the URI /height/<string: height_in_cm>/
 * If a height is given, then the system shall transition into the CUSTOM_HEIGHT state.
 */
void handle_height_requests() {
  int height = atoi((server.pathArg(0)).c_str()); // convert string parameter to integer

  // only change the state if the given height is in the height boundaries
  if(height >= MIN_HEIGHT and height <= MAX_HEIGHT) {
    g_custom_height = height; // set the custom height
    g_system_state = CUSTOM_HEIGHT; // transition to the custom height state
  }

  // send response
  send_homepage_redirection();
}


/*
 * Handles calls to the URI /height/
 * Responds with the current height of the ultrasonic sensor
 */
void handle_read_height_requests() {
  int height = get_current_height();
  server.send(200, "text/plain", String(height));
}


/**
 * Setup the output pins
 */
void setup_pins() {
  // Pin setup for motor controller
  pinMode(MOTOR_DRIVER_PWM, OUTPUT);
  pinMode(MOTOR_DRIVER_DIR, OUTPUT);
}


/**
 * Takes care of the wifi configuration
 */
void setup_wifi() {
  WiFi.mode(WIFI_STA);
  WiFi.hostname(OPT_HOSTNAME); // set HOSTNAME
  WiFi.begin(SSID, PASSWORD);

   // Wait for wifi connection
    while (WiFi.status() != WL_CONNECTED) {
      delay(500);
    Serial.print(".");
  }

  // Start the mDNS responder for esp8266.local
  if (MDNS.begin("esp8266")) {
    Serial.println("MDNS responder started");
  }
}


/**
 * Print information about the wifi connection:
 * SSID, IP, HOSTNAME
 */
void print_connection_info() {
  // Print connection info
  Serial.print("Connected to ");
  Serial.println(SSID);
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
  Serial.print("HOSTNAME: ");
  Serial.println(WiFi.hostname().c_str());
}


/**
 * Register the routes of the server
 */
void register_server_routes() {
  server.on(F("/"), display_index); // route: /
  server.on(UriBraces("/motor/{}"), handle_motor_requests); // route: /motor/<string: action>/
  server.on(UriBraces("/height/{}"), handle_height_requests); // route: /height/<string: height_in_cm>/
  server.on(UriBraces("/height"), handle_read_height_requests); // route: /height/ - is being called from client javascript
}


/*
 * Login to the network, setup the server and register URI call handlers.
 */
void setup(void) {
  Serial.begin(115200);

  setup_wifi();

  #ifdef DEBUG
  print_connection_info();
  #endif

  register_server_routes();

  // Start the server
  server.begin();
  Serial.println("HTTP server started");
}


/*
 * Retrieves the current height of the table by getting the distance of the
 * ultrasonic sensor.
 */
int get_current_height() {
  delay(100); // > 60 ms measurement cycle to prevent trigger signal to the echo signal
  Serial.print("Current table height");
  Serial.println(hc.dist());
  return hc.dist() + HEIGHT_DIFFERENCE; // return height
}


/*
 * Raise the table until the max height is reached
 */
void raise_table() {
  if(MAX_HEIGHT >= get_current_height()) {
    motor.setSpeed(MOTOR_SPEED);
  }
  else {
    g_system_state = HOLD;  
  }
}


/*
 * Lower the table until the min height is reached
 */
void lower_table() {
  if(MIN_HEIGHT <= get_current_height()) {
    motor.setSpeed(-MOTOR_SPEED); // two's complement for negating the integer
  }
  else {
    g_system_state = HOLD;  
  }
}


/*
 * Stop the table at the current height
 */
void stop_table() {
   motor.setSpeed(0);
}


/*
 * Controls the motor based on the system state g_system_state. This is pretty much the core FSM implementation for the state transistions.
 */
void handle_output() {
  switch (g_system_state) {
        case UP:
            raise_table(); // motor go up
            break;
        case DOWN:
            lower_table(); // motor go down
            break;
        case HOLD:
            stop_table(); // stop the motor
            break;
        case CUSTOM_HEIGHT:
          // adjust the table height until the height tolerance is ok, e.g.: abs(150-130) = 20, abs(150-170) = 20
          if(abs(g_custom_height - get_current_height()) >= HEIGHT_TOLERANCE) {
            // check if the table is too high or too low and adjust
            if(g_custom_height < get_current_height()) {
              lower_table(); // lower the table
            }
            else{
              raise_table(); // raise the table
            }
            
          }
          else {
            // adjustment is finished, transistion to the hold state to stop the motor
            g_system_state = HOLD;
          }

          break;
        default:
            // stop the motor by transitioning to the hold state
            g_system_state = HOLD;
    }
}


/*
 * Main loop. Gets the current height, retrieves inputs and do state 
 * transitions and finally control the motor based on the state.
 */
void loop(void) {
  server.handleClient(); // gets input from a client
  handle_output(); // controls the height of the table based on the input
}
