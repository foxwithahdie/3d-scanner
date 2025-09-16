const byte button = 8;
#define KI 1024

PinStatus arr[KI];
int counter = 0;
String dummy_receive;


void setup() {
    pinMode(button, INPUT);
    Serial.begin(115200);
    Serial.setTimeout(0.000001);
}

void loop() {
	while (!Serial.available());
    dummy_receive = Serial.readString();
    arr[counter] = digitalRead(button);
    if (counter < 1025) {
        counter++;
    } 
    if (counter == 1024) {
        Serial.print("<");
        for (int i = 0; i < KI; i++) {
            Serial.print(arr[i]);
            Serial.print(",");
        }
        Serial.print(">");

        for (int i = 0; i < 1024; i++) {
            arr[i] = LOW;
        }
    }
}
// }

// void setup() { 
// 	Serial.begin(115200); 
// 	Serial.setTimeout(1); 
// } 
// void loop() { 
// 	while (!Serial.available()); 
// 	String x = Serial.readString();
// 	Serial.print(x); 
// } 

