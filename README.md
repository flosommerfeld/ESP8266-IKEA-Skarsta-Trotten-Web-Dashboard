# ESP8266-Ikea-Skarsta-WebDashboard
![arduino build workflow](https://github.com/flosommerfeld/ESP8266-Ikea-Skarsta-WebDashboard/actions/workflows/build-arduino-sketches.yml/badge.svg)

## Dependencies
Used dependencies for this project are:
- [Arduino Library for Cytron Motor Drivers](https://github.com/CytronTechnologies/CytronMotorDriver), used to control the motor
- [HCSR04 ultrasonic sensor Library for Arduino](https://github.com/gamegine/HCSR04-ultrasonic-sensor-lib), used to read the distance from the ultrasonic sensor

The dependencies can be installed via the [standard procedure for installing Arduino libraries](https://docs.arduino.cc/software/ide-v1/tutorials/installing-libraries):

1. Open the Arduino IDE, select `Sketch` -> `Include Library` -> `Manage Libraries...`.
2. Search for `Cytron Motor Drivers Library`/`HCSR04 ultrasonic sensor Library`.
3. Click `Install` to install the library.
4. Restart the Arduino IDE.
