#include <ESP8266WiFi.h>
#include <WiFiClientSecure.h>
#include <arduino_wifi_secrets.h>
#include <arduino_slack_secrets.h>

#define WIFI_SSID     SECRET_SSID
#define WIFI_PASSWORD SECRET_PASS

String url = SLACK_URL;
const char *host = SLACK_HOST;
const int httpsPort = SLACK_PORT;
const char fingerprint[] PROGMEM = SLACK_FINGERPRINT;

void setup() {
    Serial.begin(115200);
    Serial.println();
    Serial.print("connecting to ");
    Serial.println(WIFI_SSID);
    WiFi.mode(WIFI_STA);
    WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }
    Serial.println("");
    Serial.println("WiFi connected");
    Serial.println("IP address: ");
    Serial.println(WiFi.localIP());
}

void loop() {
    WiFiClientSecure client;
    client.setFingerprint(fingerprint);
    if (!client.connect(host, httpsPort)) {
        Serial.println("connection failed");
        return;
    }

    if (Serial.available() > 0) {
        String message = Serial.readString();
        int lengte = message.length() + 11;
        Serial.println("requesting");
        client.print(String("POST ") + url + " HTTP/1.1\r\n" +
                     "Host: " + host + "\r\n" +
                     "User-Agent: ArduinoIoT/1.0\r\n" +
                     "Connection: close\r\n" +
                     "Content-Type: application/json\r\n" +
                     "Content-Length: " + lengte + "\r\n\r\n" +
                     "{\"text\":\"" + message + "\"}");
        Serial.println(client.readString());
    }
}
