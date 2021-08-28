#include <ESP8266WiFiMulti.h>
#include <InfluxDbClient.h>
#include <InfluxDbCloud.h>
#include <arduino_wifi_secrets.h>
#include <arduino_influxdb_secrets.h>

ESP8266WiFiMulti wifiMulti;
#define DEVICE "ESP8266"

#define ANA A0
#define DIGI D5
#define SENSOR_SWITCH D7

#define TZ_INFO "CET-1CEST,M3.5.0,M10.5.0/3"

InfluxDBClient client(INFLUXDB_URL, INFLUXDB_ORG, INFLUXDB_BUCKET, INFLUXDB_TOKEN, InfluxDbCloud2CACert);

Point sensor("MOISTURE");

double analogValue = 0.0;
int digitalValue = 0;
double analogVolts = 0.0;
unsigned long timeHolder = 0;

void setup() {
    pinMode(ANA, INPUT);
    pinMode(DIGI, INPUT);
    pinMode(SENSOR_SWITCH, OUTPUT);
    Serial.begin(115200);
    Serial.println("Test");

    WiFi.mode(WIFI_STA);
    wifiMulti.addAP(WIFI_SSID, WIFI_PASSWORD);

    Serial.print("Connecting to wifi");
    while (wifiMulti.run() != WL_CONNECTED) {
        Serial.print(".");
        delay(100);
    }
    Serial.println();

    timeSync(TZ_INFO, "pool.ntp.org", "time.nis.gov");

    if (client.validateConnection()) {
        Serial.print("Connected to InfluxDB: ");
        Serial.println(client.getServerUrl());
    } else {
        Serial.print("InfluxDB connection failed: ");
        Serial.println(client.getLastErrorMessage());
    }
}

void loop() {
    sensor.clearFields();

    digitalWrite(SENSOR_SWITCH, HIGH);
    delay(100);
    analogValue = analogRead(ANA);
    digitalValue = digitalRead(DIGI);
    digitalWrite(SENSOR_SWITCH, LOW);

    sensor.addField("analog", analogValue);
    sensor.addField("digital", digitalValue);

    Serial.print("Analog raw: ");
    Serial.println(analogValue);
    Serial.print("Digital raw: ");
    Serial.println(digitalValue);
    Serial.println(" ");

    Serial.print("Writing: ");
    Serial.println(sensor.toLineProtocol());

    if (!client.writePoint(sensor)) {
        Serial.print("InfluxDB write failed: ");
        Serial.println(client.getLastErrorMessage());
    }

    Serial.println("Wait....");
    delay(60 * 60 * 1000);
}
