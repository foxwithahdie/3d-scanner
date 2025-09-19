#include <WiFiS3.h>
#include <WiFiUdp.h>

#include "arduino_secrets.h"

const byte button = 8;
#define KI 288

int port = 8182;

int arr[KI];
char packet[255];
// char packet_buffer[256];
int counter = 0;
String dummy_receive;

// int status;

WiFiUDP udp;
int data_length;

void setup() {
    pinMode(button, INPUT);
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
    arr[counter] = (digitalRead(button) == HIGH ? 1 : 0) * random(0, 1024));

    if (counter < KI) {
        counter++;
    }

    if (udp.parsePacket()) {
        data_length = udp.available();

        // receive
        udp.read(packet, 255);
        packet[data_length] = 0;

        // send
        udp.beginPacket(udp.remoteIP(), udp.remotePort());
            if (counter >= KI - 1) {
                udp.print("<");
                Serial.print("<");
                for (int i = 0; i < KI; i++) {
                    udp.print(arr[i]);
                    Serial.print(arr[i]);
                    udp.print(",");
                    Serial.print(",");
                }
                udp.print(">");
                Serial.print(">");
                udp.println();
                Serial.println();
                counter = 0;
                for (int i = 0; i < KI; i++) {
                    arr[i] = LOW;
                }
            }
        udp.endPacket();

    }
}
