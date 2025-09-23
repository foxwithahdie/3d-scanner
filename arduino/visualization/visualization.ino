byte distance_sensor = A0;
byte button = 8;


void setup() {
  Serial.begin(9600);
  pinMode(distance_sensor, INPUT);
  pinMode(button, INPUT);
  Serial.print("On!");
}

void loop() {
  if (digitalRead(button) == HIGH) {
    int voltage = analogRead(distance_sensor);
    double read = (5 / 1024.0) * voltage;
    Serial.print("Voltage: ");
    Serial.println(read);
  }
}
