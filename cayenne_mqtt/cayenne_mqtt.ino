#include <CayenneMQTTESP8266.h>
#include <arduino_cayenne_secrets.h>
#include <arduino_wifi_secrets.h>

void setup() {
    Serial.begin(115200);
    Cayenne.begin(CAYENNE_USERNAME, CAYENNE_PASSWORD, CAYENNE_CLIENTID, WIFI_SSID, WIFI_PASSWORD);
}

void loop() {
    Cayenne.loop();
}


CAYENNE_OUT_DEFAULT() {
    // Write data to Cayenne
    Cayenne.virtualWrite(0, millis());
    //Cayenne.celsiusWrite(1, 22.0);
    //Cayenne.luxWrite(2, 700);
    //Cayenne.virtualWrite(3, 50, TYPE_PROXIMITY, UNIT_CENTIMETER);
}

CAYENNE_IN_DEFAULT() {
    CAYENNE_LOG("Channel %u, value %s", request.channel, getValue.asString());
}
