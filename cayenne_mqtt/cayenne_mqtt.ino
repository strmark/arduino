#define CAYENNE_PRINT Serial
#include <CayenneMQTTESP8266.h>
#include <arduino_cayenne_secrets.h>
#include <arduino_wifi_secrets.h>

char ssid[] = SECRET_SSID;
char wifiPassword[] = SECRET_PASS;
char username[] = CAYENNE_USERNAME;
char password[] = CAYENNE_PASSWORD;
char clientID[] = CAYENNE_CLIENTID;

unsigned long lastMillis = 0;

void setup() {
  Serial.begin(115200);
  Cayenne.begin(username, password, clientID, ssid, wifiPassword);
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
