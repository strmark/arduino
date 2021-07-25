#define LED D4
#define TIMEOUT 1000

void setup() {
    pinMode(LED, OUTPUT);
    Serial.begin(115200);
}

void loop() {
    // LED active is low
    digitalWrite(LED, HIGH);
    Serial.println("OFF");
    delay(TIMEOUT);
    digitalWrite(LED, LOW);
    Serial.println("ON");
    delay(TIMEOUT);
}
