#include <ESP8266WiFi.h>
#include <arduino_wifi_secrets.h>

#define RED_PIN D5
#define GREEN_PIN D6

WiFiServer server(80);

String header;
String redState = "off";
String greenState = "off";

unsigned long currentTime = millis();
unsigned long previousTime = 0;
const long timeoutTime = 2000;

void setup() {
    Serial.begin(115200);

    pinMode(RED_PIN, OUTPUT);
    pinMode(GREEN_PIN, OUTPUT);

    digitalWrite(RED_PIN, LOW);
    digitalWrite(GREEN_PIN, LOW);

    Serial.print("Connecting to ");
    Serial.println(WIFI_SSID);
    WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }

    Serial.println("");
    Serial.println("WiFi connected.");
    Serial.println("IP address: ");
    Serial.println(WiFi.localIP());
    server.begin();
}

void loop() {
    WiFiClient client = server.available();
    if (client) {
        Serial.println("New client...");
        String currentLine = "";
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
                        client.println("Content-type:text/html");
                        client.println("Connection: close");
                        client.println();

                        if (header.indexOf("GET /red/on") >= 0) {
                            Serial.println("Red pin on");
                            redState = "on";
                            digitalWrite(RED_PIN, HIGH);
                        } else if (header.indexOf("GET /red/off") >= 0) {
                            Serial.println("Red pin off");
                            redState = "off";
                            digitalWrite(RED_PIN, LOW);
                        } else if (header.indexOf("GET /green/on") >= 0) {
                            Serial.println("Green pin on");
                            greenState = "on";
                            digitalWrite(GREEN_PIN, HIGH);
                        } else if (header.indexOf("GET /green/off") >= 0) {
                            Serial.println("Green pin off");
                            greenState = "off";
                            digitalWrite(GREEN_PIN, LOW);
                        }

                        client.println("<!DOCTYPE html><html>");
                        client.println("<head><meta name=\"viewport\" content=\"width=device-width, initial-scale=1\">");
                        client.println("<link rel=\"icon\" href=\"data:,\">");
                        client.println("<style>html { font-family: Helvetica; display: inline-block; margin: 0px auto; text-align: center;}");
                        client.println(".button { background-color: #195B6A; border: none; color: white; padding: 16px 40px;");
                        client.println("text-decoration: none; font-size: 30px; margin: 2px; cursor: pointer;}");
                        client.println(".button2 {background-color: #77878A;}</style></head>");
                        client.println("<body><h1>Web Server</h1>");

                        client.println("<p>Red led - State " + redState + "</p>");
                        if (redState == "off") {
                            client.println("<p><a href=\"/red/on\"><button class=\"button\">ON</button></a></p>");
                        } else {
                            client.println("<p><a href=\"/red/off\"><button class=\"button button2\">OFF</button></a></p>");
                        }

                        client.println("<p>Green led - State " + greenState + "</p>");
                        if (greenState == "off") {
                            client.println("<p><a href=\"/green/on\"><button class=\"button\">ON</button></a></p>");
                        } else {
                            client.println("<p><a href=\"/green/off\"><button class=\"button button2\">OFF</button></a></p>");
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
    }
}
