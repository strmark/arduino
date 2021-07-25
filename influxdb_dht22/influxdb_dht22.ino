#include <ESP8266WiFiMulti.h>
#include <InfluxDbClient.h>
#include <InfluxDbCloud.h>
#include "DHTesp.h"
#include <arduino_wifi_secrets.h>
#include <arduino_influxdb_secrets.h>

#define WIFI_SSID SECRET_SSID
#define WIFI_PASSWORD SECRET_PASS
#define INFLUXDB_URL SECRET_INFLUXDB_URL
#define INFLUXDB_TOKEN SECRET_INFLUXDB_TOKEN
#define INFLUXDB_ORG SECRET_INFLUXDB_ORG
#define INFLUXDB_BUCKET SECRET_INFLUXDB_BUCKET

#define TZ_INFO "CET-1CEST,M3.5.0,M10.5.0/3"

#define SENSOR_DIGI D5
#define SENSOR_SWITCH D7

ESP8266WiFiMulti wifiMulti;

InfluxDBClient client(INFLUXDB_URL, INFLUXDB_ORG, INFLUXDB_BUCKET, INFLUXDB_TOKEN, InfluxDbCloud2CACert);

Point sensor("DHT22");
DHTesp dht;

void setup() {
    Serial.begin(115200);

    WiFi.mode(WIFI_STA);
    wifiMulti.addAP(WIFI_SSID, WIFI_PASSWORD);

    Serial.print("Connecting to wifi");
    while (wifiMulti.run() != WL_CONNECTED) {
        Serial.print(".");
        delay(100);
    }
    Serial.println();

    pinMode(SENSOR_SWITCH, OUTPUT);
    dht.setup(SENSOR_DIGI, DHTesp::DHT22);

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
    // Clear fields for reusing the point
    sensor.clearFields();

    digitalWrite(SENSOR_SWITCH, HIGH);
    delay(dht.getMinimumSamplingPeriod());

    float humidity = dht.getHumidity();
    float temperature = dht.getTemperature();
    String status = dht.getStatusString();

    digitalWrite(SENSOR_SWITCH, LOW);

    sensor.addField("status", status);
    sensor.addField("humidity", humidity);
    sensor.addField("temperature", temperature);

    Serial.print("Writing: ");
    Serial.println(sensor.toLineProtocol());

    if (!client.writePoint(sensor)) {
        Serial.print("InfluxDB write failed: ");
        Serial.println(client.getLastErrorMessage());
    }

    Serial.println("Wait....");
    delay(10000);
}
