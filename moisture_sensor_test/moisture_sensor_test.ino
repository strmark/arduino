const int dry = 717; // dry sensor
const int wet = 300; // wet sensor

void setup() {
    Serial.begin(115200);
}

void loop() {
    int sensorVal = analogRead(A0);

    Serial.print(sensorVal);
    Serial.println("");

    // Translate this to a scale or 0% to 100%
    int percentageHumididy = map(sensorVal, wet, dry, 100, 0);

    Serial.print(percentageHumididy);
    Serial.println("%");

    delay(100);
}
