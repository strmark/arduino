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
#define MOISTANAPIN A0
#define MOISTDIGIPIN D5
#define MOISTSWITCH D6
#define PUMPPIN D7

DHTesp dht;

static char celsiusTemp[7];
static char humidityTemp[7];
static char analogTemp[7];

String header;
String pumpState = "off";

float temp;
float hum;
double analogValue = 0.0;
int digitalValue = 0;
int percentageHumidity;

unsigned long currentTime = millis();
unsigned long previousTime = 0;

const long timeoutTime = 10000;

void setup() {
    Serial.begin(115200);
    delay(10);

    dht.setup(DHTPIN, DHTesp::DHT11);

    pinMode(DHTSWITCH, OUTPUT);
    pinMode(MOISTANAPIN, INPUT);
    pinMode(MOISTDIGIPIN, INPUT);
    pinMode(MOISTSWITCH, OUTPUT);
    pinMode(PUMPPIN, OUTPUT);

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
    delay(5000);

    Serial.println(WiFi.localIP());
}

void getMoistureValues() {
    digitalWrite(MOISTSWITCH, HIGH);
    delay(1000);
    analogValue = analogRead(MOISTANAPIN);
    digitalValue = digitalRead(MOISTDIGIPIN);
    digitalWrite(MOISTSWITCH, LOW);

    Serial.print("Analog raw: ");
    Serial.println(analogValue);
    Serial.print("Digital raw: ");
    Serial.println(digitalValue);

    percentageHumidity = map(analogValue, wet, dry, 100, 0);

    if (isnan(analogValue)) {
        Serial.println("Failed to read from moisture sensor!");
        strcpy(analogTemp, "Failed");
    } else {
        Serial.println("Analogvalue written!");
        dtostrf(analogValue, 6, 2, analogTemp);
    }
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
        dtostrf(temp, 6, 2, celsiusTemp);
        dtostrf(hum, 6, 2, humidityTemp);
    }
}

void loop(){
    WiFiClient client = server.available();
    if (client) {

        Serial.println("New client...");
        String currentLine = "";
        getMoistureValues();
        getDHTValues();
        currentTime = millis();
        previousTime = currentTime;
        while (client.connected() && currentTime - previousTime <= timeoutTime) {
            currentTime = millis();
            if (client.available()) {
                char c = client.read();
                Serial.write(c);
                header += c;
                if (c == '\n') {
                    if (currentLine.length() == 0) {
                        client.println("HTTP/1.1 200 OK");
                        client.println("Content-Type: text/html");
                        client.println("Connection: close");
                        client.println();
                        if (header.indexOf("GET /pump/on") >= 0) {
                            Serial.println("Pump on");
                            pumpState = "on";
                            digitalWrite(PUMPPIN, HIGH);
                        } else if (header.indexOf("GET /pump/off") >= 0) {
                            Serial.println("Pump off");
                            pumpState = "off";
                            digitalWrite(PUMPPIN, LOW);
                        }
                        // your actual web page that displays temperature and humidity
                        client.println("<!DOCTYPE html><html>");
                        client.println("<head><meta name=\"viewport\" content=\"width=device-width, initial-scale=1\">");
                        client.println("<link rel=\"icon\" href=\"data:,\">");
    
                        client.println("<style>html { font-family: Helvetica; display: inline-block; margin: 0px auto; text-align: center;}");
                        client.println(".button { background-color: #195B6A; border: none; color: white; padding: 16px 40px;");
                        client.println("text-decoration: none; font-size: 30px; margin: 2px; cursor: pointer;}");
                        client.println(".button2 {background-color: #77878A;}</style></head>");
    
                        client.println("<body><h1>Temperature, Humidity and Moisture</h1>");
                        client.println("<h3>Temperature in Celsius: ");
                        client.println(celsiusTemp);
                        client.println("*C</h3><h3>Humidity: ");
                        client.println(humidityTemp);
                        client.println("%</h3><h3>Moisture");
                        client.println(analogTemp);
                        client.println("</h3><h3>Moisture percentage:");
                        client.println(percentageHumidity);
                        client.println("%</h3>");
                        client.println("<h3>Pump - State " + pumpState + "</h3>");
                        if (pumpState == "off") {
                            client.println("<p><a href=\"/pump/on\"><button class=\"button\">ON</button></a></p>");
                        } else {
                            client.println("<p><a href=\"/pump/off\"><button class=\"button button2\">OFF</button></a></p>");
                        }
                        client.println("</body></html>");
                        break;
                    } else {
                        currentLine = "";
                    }
                } else if (c != '\r') {
                    currentLine += c;
                }
            }
        }
        header = "";
        client.stop();
        Serial.println("Client disconnected.");
        Serial.println("");
    }
}
