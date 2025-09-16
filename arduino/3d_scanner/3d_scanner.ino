// const byte button = 8;
// #define KI 1024

// PinStatus arr[KI];
// int counter = 0;


// void setup() {
//     pinMode(button, INPUT);
//     Serial.begin(115200);
//     Serial.setTimeout(1);
// }

// void loop() {
//     arr[counter] = digitalRead(button);
//     if (counter < 1025)
//         counter++;
//     if (counter == 1024) {
//         for (int i = 0; i < 1024; i += 8) {
//             Serial.print(arr[i]);
//         }
//         Serial.println();
//         for (int i = 0; i < 1024; i += 8) {
//             arr[i] = LOW;
//         }

//     }

    
// }

void setup() { 
	Serial.begin(115200); 
	Serial.setTimeout(1); 
} 
void loop() { 
	while (!Serial.available()); 
	String x = Serial.readString();
	Serial.print(x); 
} 

