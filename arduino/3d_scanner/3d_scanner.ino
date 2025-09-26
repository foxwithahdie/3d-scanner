#include <Servo.h>
#include <WiFiS3.h>
#include <WiFiUdp.h>

// credentials for wifi - create if you have your own wifi ssid/pswd
#include "arduino_secrets.h"

// defining a macro for dictating maximum allowable length of arrays (288 bytes)
#define MAX_DATA 288

// for ease of use
#define byte uint8_t

// declaring servo objects for panning and tilting servos
Servo servoPan;
Servo servoTilt;

const byte distanceSensor = A0;
const byte resetButton = 8;
const byte servoPanPin = 9;
const byte servoTiltPin = 10;

// wifi/udp values for sending data via wifi
WiFiUDP udp;
int port = 8182;
char packet[10];
int dataLength;

// setting minimum and maximum pan/tilt angles - changes for each situation
const byte minPan = 60;
const byte maxPan = 100;
const byte minTilt = 5;
const byte maxTilt = 70;

// setting arrays for storing sensor readings and servo angles
int dataArray[MAX_DATA];
int panArray[MAX_DATA];
int tiltArray[MAX_DATA];
int count = 0;

// meant for averaging the values detected at every point
const byte average_value_size = 10;
int average_value_sum = 0;

// conditions for our end-of-scan, so that we don't blow up wifi with packets every time it ends
bool once = false;
bool finished = false;

void setup() {
  // setting up servo pins and sensor pin as input
  servoPan.attach(servoPanPin);
  servoTilt.attach(servoTiltPin);

  // how we read data from distance sensor / detect button press
  pinMode(distanceSensor, INPUT);
  pinMode(resetButton, INPUT);

  // serial for ease of use / understanding end conditions
  Serial.begin(9600);
  Serial.setTimeout(0.000001);

  // prints ip once connected to wifi
  WiFi.begin(SSID, PWD);
  while (WiFi.status() != WL_CONNECTED) {
    delay(100);
    Serial.print("Connecting...");
  }
  Serial.print("IP = ");
  Serial.println(WiFi.localIP());

  udp.begin(8182);
}

void loop() {
  if (!finished) {
    // zeroing sensor to begin scanning
    servoTilt.write(maxTilt);
    servoPan.write(maxPan);
    // beginning raster scan
    for (int tiltAngle = minTilt; tiltAngle <= maxTilt; tiltAngle++) {
      if (tiltAngle % 2 == 0) {
        // If the tilt angle is even, pan from min to max
        for (int panAngle = minPan; panAngle <= maxPan; panAngle++) {
          servoTilt.write(tiltAngle);
          servoPan.write(panAngle);
          delay(20);
          // log sensor readings and tilt/pan angle
          for (int i = 0; i < average_value_size; i++) {
            average_value_sum += analogRead(distanceSensor);
          }
          dataArray[count] = average_value_sum / average_value_size;
          average_value_sum = 0;
          panArray[count] = panAngle;
          tiltArray[count] = tiltAngle;
          // if the counter is 287, sends data and resets the arryas
          if (count >= MAX_DATA - 1) {
            if (udp.parsePacket()) {
              // receives data as a check
              int data = udp.available();
              udp.read(packet, 255);
              packet[dataLength] = 0;

              // sends packet data
              udp.beginPacket(udp.remoteIP(), udp.remotePort());
              // sending array of distance sensor data
              udp.print("distance_sensor_data ");
              udp.print("<");
              for (int i = 0; i < MAX_DATA; i++) {
                udp.print(dataArray[i]);
                udp.print(",");
              }
              udp.print(">");
              udp.println();
              udp.endPacket();

              udp.beginPacket(udp.remoteIP(), udp.remotePort());
              // sending array of the pan servo data
              udp.print("servo_pan_data ");
              udp.print("<");
              for (int i = 0; i < MAX_DATA; i++) {
                udp.print(panArray[i]);
                udp.print(",");
              }
              udp.print(">");
              udp.println();
              udp.endPacket();

              udp.beginPacket(udp.remoteIP(), udp.remotePort());
              // sending data of the tilt servo data
              udp.print("servo_tilt_data ");
              udp.print("<");
              for (int i = 0; i < MAX_DATA; i++) {
                udp.print(tiltArray[i]);
                udp.print(",");
              }
              udp.print(">");
              udp.println();
              udp.endPacket();
            }
            // reset the arrays
            for (int i = 0; i < MAX_DATA; i++) {
              dataArray[i] = 0;
              panArray[i] = 0;
              tiltArray[i] = 0;
            }
          }
          count++;
          // reset count
          if (count == MAX_DATA) {
            count = 0;
          }
        }
      } else {
        // If the tilt angle is odd, pan from max to min
        for (int panAngle = maxPan; panAngle >= minPan; panAngle--) {
          servoTilt.write(tiltAngle);
          servoPan.write(panAngle);
          delay(20);
          dataArray[count] = analogRead(distanceSensor);
          panArray[count] = panAngle;
          tiltArray[count] = tiltAngle;
          if (count >= MAX_DATA - 1) {
            if (udp.parsePacket()) {
              int data = udp.available();
              udp.read(packet, 255);
              packet[dataLength] = 0;

              udp.beginPacket(udp.remoteIP(), udp.remotePort());
              udp.print("distance_sensor_data ");
              udp.print("<");
              for (int i = 0; i < MAX_DATA; i++) {
                udp.print(dataArray[i]);
                udp.print(",");
              }
              udp.print(">");
              udp.println();
              udp.endPacket();

              udp.beginPacket(udp.remoteIP(), udp.remotePort());
              udp.print("servo_pan_data ");
              udp.print("<");
              for (int i = 0; i < MAX_DATA; i++) {
                udp.print(panArray[i]);
                udp.print(",");
              }
              udp.print(">");
              udp.println();
              udp.endPacket();

              udp.beginPacket(udp.remoteIP(), udp.remotePort());
              udp.print("servo_tilt_data ");
              udp.print("<");
              for (int i = 0; i < MAX_DATA; i++) {
                udp.print(tiltArray[i]);
                udp.print(",");
              }
              udp.print(">");
              udp.println();
              udp.endPacket();
              
            }
            for (int i = 0; i < MAX_DATA; i++) {
              dataArray[i] = 0;
              panArray[i] = 0;
              tiltArray[i] = 0;
            }
          }
          count++;
          if (count == MAX_DATA) {
            count = 0;
          }
        }
      }
    }

    // if the data arrays aren't full or empty, send the remaining data.
    if (udp.parsePacket()) {
      int data = udp.available();
      udp.read(packet, 255);
      packet[dataLength] = 0;

      udp.beginPacket(udp.remoteIP(), udp.remotePort());
      udp.print("distance_sensor_data ");
      udp.print("<");
      for (int i = 0; i < count - 1; i++) {
        udp.print(dataArray[i]);
        udp.print(",");
      }
      udp.print(">");
      udp.println();
      udp.endPacket();

      udp.beginPacket(udp.remoteIP(), udp.remotePort());
      udp.print("servo_pan_data ");
      udp.print("<");
      for (int i = 0; i < count - 1; i++) {
        udp.print(panArray[i]);
        udp.print(",");
      }
      udp.print(">");
      udp.println();
      udp.endPacket();

      udp.beginPacket(udp.remoteIP(), udp.remotePort());
      udp.print("servo_tilt_data ");
      udp.print("<");
      for (int i = 0; i < count - 1; i++) {
        udp.print(tiltArray[i]);
        udp.print(",");
      }
      udp.print(">");
      udp.println();
      udp.endPacket();
    }
  }

  finished = true;

  while (finished) {
    // halts the program
    if (!once) {
      Serial.println("Scanning process finished!");
      if (udp.parsePacket()) {
        int data = udp.available();
        udp.read(packet, 255);
        packet[dataLength] = 0;

        udp.beginPacket(udp.remoteIP(), udp.remotePort());
        // end is final condition
        udp.print("end");
        udp.endPacket();
      }
      once = true;
    }
    // reruns the scan if you press the reset button
    if (digitalRead(resetButton) == HIGH) {
      finished = false;
      once = false;
    }
  }
}
