//Analog pin (Moisture Sensor)
#define ANA A0
//Digital pin (Moisture Sensor)
#define DIGI 5
#define SENSOR 7

//Power pin (Pump via transistor)
#define POMPOUT 6

double analogValue = 0.0;
int digitalValue = 0;
unsigned long checkInterval = 5000;
unsigned long nextCheckAt = 5000;

void setup() {
    pinMode(ANA, INPUT);
    pinMode(DIGI, INPUT);
    pinMode(SENSOR, OUTPUT);
    pinMode(POMPOUT, OUTPUT);
    Serial.begin(115200);
}

void loop() {
    if (millis() > nextCheckAt) {
        digitalWrite(SENSOR, HIGH);
        delay(100);
        analogValue = analogRead(ANA);
        digitalValue = digitalRead(DIGI);
        digitalWrite(SENSOR, LOW);

        Serial.print("Analog raw: ");
        Serial.println(analogValue);
        Serial.print("Digital raw: ");
        Serial.println(digitalValue);
        Serial.println(" ");
        if (digitalValue == 0) {
            digitalWrite(POMPOUT, LOW);
        } else {
            Serial.println("Pumping");
            digitalWrite(POMPOUT, HIGH);
        }

        nextCheckAt = millis() + checkInterval;
    }
    yield();
}
