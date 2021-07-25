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

// adjust value while testing the sensor
static const int dry_sensor = 721;
static const int wet_sensor = 296;

// Digital pin connected to the DHT sensor
#define DHTPIN D3
#define DHTSWITCH D2
#define MOISTPIN A0
#define MOISTSWITCH D5

ESP8266WiFiMulti wifiMulti;

InfluxDBClient client(INFLUXDB_URL, INFLUXDB_ORG, INFLUXDB_BUCKET, INFLUXDB_TOKEN, InfluxDbCloud2CACert);
Point sensor("DHT_MOISTURE");
DHTesp dht;

void setup() {
    Serial.begin(115200);

    // Setup wifi
    WiFi.mode(WIFI_STA);
    wifiMulti.addAP(WIFI_SSID, WIFI_PASSWORD);

    Serial.print("Connecting to wifi");
    while (wifiMulti.run() != WL_CONNECTED) {
        Serial.print(".");
        delay(100);
    }
    Serial.println();

    pinMode(DHTSWITCH, OUTPUT);
    pinMode(MOISTSWITCH, OUTPUT);

    dht.setup(DHTPIN, DHTesp::DHT11);

    timeSync(TZ_INFO, "pool.ntp.org", "time.nis.gov");

    if (client.validateConnection()) {
        Serial.print("Connected to InfluxDB: ");
        Serial.println(client.getServerUrl());
    } else {
        Serial.print("InfluxDB connection failed: ");
        Serial.println(client.getLastErrorMessage());
    }
}

void getTemperatureData() {
    digitalWrite(DHTSWITCH, HIGH);

    // bug in de DHT11 sampling samplingPeriod * 2
    delay(dht.getMinimumSamplingPeriod() * 2);
    float humidity = dht.getHumidity();
    float temperature = dht.getTemperature();
    String statusSensor = dht.getStatusString();
    Serial.printf("Status: %s \n", statusSensor);
    Serial.printf("Humidity: %f \n", humidity);
    Serial.printf("Temperature: %f \n", temperature);

    sensor.addField("statusTemp", statusSensor);
    sensor.addField("humidity", humidity);
    sensor.addField("temperature", temperature);
    digitalWrite(DHTSWITCH, LOW);
}

void getMoistureData() {
    digitalWrite(MOISTSWITCH, HIGH);
    delay(1000);
    int moisture = analogRead(MOISTPIN);
    Serial.printf("Moisture: %i \n", moisture);
    int percentageHumidity = map(moisture, wet_sensor, dry_sensor, 100, 0);

    sensor.addField("moisture", moisture);
    sensor.addField("percentage", percentageHumidity);
    digitalWrite(MOISTSWITCH, LOW);
}

void loop() {
    sensor.clearFields();
    getTemperatureData();
    getMoistureData();

    Serial.print("Writing: ");
    Serial.println(sensor.toLineProtocol());

    if (!client.writePoint(sensor)) {
        Serial.print("InfluxDB write failed: ");
        Serial.println(client.getLastErrorMessage());
    }

    Serial.println("Wait....");
    delay(60 * 60 * 1000);
}
