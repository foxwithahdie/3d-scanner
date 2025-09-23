#include <Servo.h>

// defining a macro for dictating maximum allowable length of arrays (288bits)
#define MAX_DATA 288
// declaring servo objects for panning and tilting servos
Servo servoPan;
Servo servoTilt;

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
  servoPan.attach(9);
  servoTilt.attach(10);
  pinMode(A0, INPUT);
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
        dataArray[count] = analogRead(A0);
        panArray[count] = panAngle;
        tiltArray[count] = tiltAngle;
        // if the counter is 277, reset the arrays
        if (count >= MAX_DATA - 1) {
          for (int i = 0; i < MAX_DATA; i++) {
            dataArray[i] = 0;
            panArray[i] = 0;
            tiltArray[i] = 0;
          }
        }
        count++;
      }
    } else {
      // If the tilt angle is odd, pan from max to min
      for (int panAngle = maxPan; panAngle >= minPan; panAngle--) {
        servoTilt.write(tiltAngle);
        servoPan.write(panAngle);
        delay(20);
        analogRead(A0);
        dataArray[count] = analogRead(A0);
        panArray[count] = panAngle;
        tiltArray[count] = tiltAngle;
        if (count >=  MAX_DATA - 1) {
          for (int i = 0; i < MAX_DATA; i++) {
            dataArray[i] = 0;
            panArray[i] = 0;
            tiltArray[i] = 0;
          }
        }
        count++;
      }
    } 
  }
  while (true) {
    // halts the program
    Serial.println("Scanning process finished!");
  } 

}