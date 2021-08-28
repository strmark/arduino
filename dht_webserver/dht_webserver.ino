#include <ESP8266WiFi.h>
#include "DHTesp.h"
#include <arduino_wifi_secrets.h>

WiFiServer server(80);

// adjust value while testing the sensor
static const int dry = 715; // dry sensor
static const int wet = 288; // wet sensor

// Digital pins
#define DHTPIN D3
#define DHTSWITCH D2
#define MOISTPIN A0
#define MOISTSWITCH D1
#define PUMPPIN D6

DHTesp dht;

static char celsiusTemp[7];
static char humidityTemp[7];
float temp;
float hum;

void setup() {
    Serial.begin(115200);
    delay(10);

    dht.setup(DHTPIN, DHTesp::DHT11);

    pinMode(PUMPPIN, OUTPUT);
    pinMode(DHTSWITCH, OUTPUT);
    pinMode(MOISTSWITCH, OUTPUT);

    // Connecting to WiFi network
    Serial.println();
    Serial.print("Connecting to ");
    Serial.println(WIFI_SSID);

    WiFi.begin(WIFI_SSID, WIFI_PASSWORD);

    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }
    Serial.println("");
    Serial.println("WiFi connected");

    server.begin();
    Serial.println("Web server running. Waiting for IP...");
    delay(10000);

    Serial.println(WiFi.localIP());
}

void getMoistureValues() {
    digitalWrite(MOISTSWITCH, HIGH);
    delay(100);
    sensorVal = analogRead(MOISTPIN);
    Serial.printf("Sensorval: %i \n", sensorVal);
    digitalWrite(MOISTSWITCH, LOW);
    percentageHumidity = map(sensorVal, wet, dry, 100, 0);
}

void getDHTValues() {
    digitalWrite(DHTSWITCH, HIGH);
    //bug DHT11 = 1000 else 2000
    delay(dht.getMinimumSamplingPeriod() * 2);
    // DHT sensor readings
    hum = dht.getHumidity();
    Serial.printf("Hum: %.2f \n", hum);
    // Read temperature as Celsius (the default)
    temp = dht.getTemperature();
    Serial.printf("Temp: %.2f \n", temp);

    digitalWrite(DHTSWITCH, LOW);

    // Check if any reads failed and exit early (to try again).
    if (isnan(hum) || isnan(temp)) {
        Serial.println("Failed to read from DHT sensor!");
        strcpy(celsiusTemp, "Failed");
        strcpy(humidityTemp, "Failed");
    } else {
        Serial.println("Failed to read from DHT sensor!");
        float hic = dht.computeHeatIndex(temp, hum, false);
        dtostrf(hic, 6, 2, celsiusTemp);
        dtostrf(hum, 6, 2, humidityTemp);
    }
}

void refreshWebserver() {
    // Listenning for new clients
    WiFiClient client = server.available();

    if (client) {
        Serial.println("New client");
        // boolean to locate when the http request ends
        boolean blank_line = true;
        while (client.connected()) {
            if (client.available()) {
                char c = client.read();

                if (c == '\n' && blank_line) {
                    client.println("HTTP/1.1 200 OK");
                    client.println("Content-Type: text/html");
                    client.println("Connection: close");
                    client.println();
                    // your actual web page that displays temperature and humidity
                    client.println("<!DOCTYPE HTML>");
                    client.println("<html>");
                    client.println("<head></head><body><h1>Temperature and Humidity</h1><h3>Temperature in Celsius: ");
                    client.println(celsiusTemp);
                    client.println("*C</h3><h3>Humidity: ");
                    client.println(humidityTemp);
                    client.println("%</h3><h3>");
                    client.println("</body></html>");
                    break;
                }
                if (c == '\n') {
                    // when starts reading a new line
                    blank_line = true;
                } else if (c != '\r') {
                    // when finds a character on the current line
                    blank_line = false;
                }
            }
        }
        // closing the client connection
        delay(1);
        client.stop();
        Serial.println("Client disconnected.");
    }
}

// runs over and over again
void loop() {
    unsigned long currentMillis = millis();
    // Every X number of seconds (interval = 10 seconds) it publishes a new MQTT message
    if (currentMillis - previousMillis >= interval) {
        previousMillis = currentMillis;
        getMoistureValues();
        getDHTValues();
        refreshWebserver()
    }
}
