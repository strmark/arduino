#include <ESP8266WiFiMulti.h>
#include <InfluxDbClient.h>
#include <InfluxDbCloud.h>
#include <arduino_wifi_secrets.h>
#include <arduino_influxdb_secrets.h>

#define DEVICE "ESP8266"
#define TZ_INFO "CET-1CEST,M3.5.0,M10.5.0/3"
InfluxDBClient client(INFLUXDB_URL, INFLUXDB_ORG, INFLUXDB_BUCKET, INFLUXDB_TOKEN, InfluxDbCloud2CACert);

ESP8266WiFiMulti wifiMulti;
Point sensor("wifi_status");

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
    sensor.addTag("device", DEVICE);
    sensor.addTag("SSID", WiFi.SSID());

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
    sensor.addField("rssi", WiFi.RSSI());

    Serial.print("Writing: ");
    Serial.println(sensor.toLineProtocol());

    if ((WiFi.RSSI() == 0) && (wifiMulti.run() != WL_CONNECTED)) {
        Serial.println("Wifi connection lost");
    }

    if (!client.writePoint(sensor)) {
        Serial.print("InfluxDB write failed: ");
        Serial.println(client.getLastErrorMessage());
    }

    Serial.println("Wait ....");
    delay(10000);
}
