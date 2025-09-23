#include <WiFiS3.h>
#include <WiFiUdp.h>
#include <Servo.h>

#include "arduino_secrets.h"


// defining a macro for dictating maximum allowable length of arrays (288bits)
#define MAX_DATA 288
// declaring servo objects for panning and tilting servos
Servo servoPan;
Servo servoTilt;

byte distanceSensor = A0;
byte servoPanPin = 9;
byte servoTiltPin = 10;

WiFiUDP udp;
int port = 8182;
char packet[255];

// setting minimum and maximum pan/tilt angles
const int minPan = 60;
const int maxPan = 120;
const int minTilt = 80;
const int maxTilt = 100;
// setting arrays for storing sensor readings and servo angles 
int dataArray[MAX_DATA];
int panArray[MAX_DATA];
int tiltArray[MAX_DATA];
int count = 0;

void setup() {
  // setting up servo pins and sensor pin as input
  servoPan.attach(servoPanPin);
  servoTilt.attach(servoTiltPin);
  pinMode(distanceSensor, INPUT);
  Serial.begin(9600);
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

void loop() {
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
        dataArray[count] = analogRead(distanceSensor);
        panArray[count] = panAngle;
        tiltArray[count] = tiltAngle;
        // if the counter is 277, sends data and resets the arryas
        if (count >= MAX_DATA - 1) {
          if (udp.parsePacket()) {
            // receives data as a check
            int data = udp.available();
            udp.read(packet, 255);
            packet[data_length] = 0;

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
        if (count >=  MAX_DATA - 1) {
          if (udp.parsePacket()) {
            int data = udp.available();
            udp.read(packet, 255);
            packet[data_length] = 0;
            
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
    packet[data_length] = 0;

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
  
  while (true) {
    // halts the program
    Serial.println("Scanning process finished!");
  }

}
