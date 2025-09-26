#include <Servo.h>
#include <WiFiS3.h>
#include <WiFiUdp.h>

#include "arduino_secrets.h"

// defining a macro for dictating maximum allowable length of arrays (288bits)
#define MAX_DATA 288
// declaring servo objects for panning and tilting servos
Servo servoTilt;

byte distanceSensor = A0;
byte resetButton = 8;
byte servoTiltPin = 10;

WiFiUDP udp;
int port = 8182;
char packet[10];
int dataLength;

// setting minimum and maximum pan/tilt angles
const int minTilt = 0;
const int maxTilt = 90;

// setting arrays for storing sensor readings and servo angles
int dataArray[MAX_DATA];
int tiltArray[MAX_DATA];
int count = 0;
int average_value_size = 10;
int average_value_sum = 0;

void setup() {
  // setting up servo pins and sensor pin as input
  servoTilt.attach(servoTiltPin);
  pinMode(distanceSensor, INPUT);
  pinMode(resetButton, INPUT);
  Serial.begin(115200);
  Serial.setTimeout(0.000001);
  WiFi.begin(SSID, PWD);
  while (WiFi.status() != WL_CONNECTED) {
    delay(100);
    Serial.print("Connecting...");
  }
  Serial.print("IP = ");
  Serial.println(WiFi.localIP());
  udp.begin(8182);
}

void loop() {   }
