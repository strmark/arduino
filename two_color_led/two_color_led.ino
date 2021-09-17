#define RED_PIN D5
#define GREEN_PIN D6

void setup() {
    pinMode(GREEN_PIN, OUTPUT);
    pinMode(RED_PIN, OUTPUT);
}

void loop() {
    analogWrite(GREEN_PIN, 0);
    analogWrite(RED_PIN, 250);
    delay(1000);

    analogWrite(GREEN_PIN, 250);
    analogWrite(RED_PIN, 250);
    delay(1000);

    analogWrite(GREEN_PIN, 250);
    analogWrite(RED_PIN, 0);
    delay(1000);
}