#include <ESP8266WiFi.h>
#include <WiFiClientSecure.h>
#include <UniversalTelegramBot.h>
#include <ArduinoJson.h>
#include <arduino_wifi_secrets.h>
#include <arduino_telegram_secrets.h>

#define WIFI_SSID SECRET_SSID
#define WIFI_PASSWORD SECRET_PASS

#define BOTtoken SECRET_BOT_TOKEN
#define CHAT_ID SECRET_CHAT_ID

#define ANA A0
#define DIGI D5
#define SENSOR D7

X509List cert(TELEGRAM_CERTIFICATE_ROOT);
WiFiClientSecure client;
UniversalTelegramBot bot(BOTtoken, client);

int botRequestDelay = 1000;
unsigned long lastTimeBotRan;

double analogValue = 0.0;
int digitalValue = 0;

String getReadings() {
    digitalWrite(SENSOR, HIGH);
    delay(100);
    analogValue = analogRead(ANA);
    digitalValue = digitalRead(DIGI);
    digitalWrite(SENSOR, LOW);

    Serial.print("Analog raw: ");
    Serial.println(analogValue);
    Serial.print("Digital raw: ");
    Serial.println(digitalValue);
    Serial.println(" ");

    String message = "Analog: " + String(analogValue) + " \n";
    message += "Digital: " + String(digitalValue) + " \n";
    return message;
}

void handleNewMessages(int numNewMessages) {
    Serial.println("handleNewMessages");
    Serial.println(String(numNewMessages));

    for (int i = 0; i < numNewMessages; i++) {
        // Chat id of the requester
        String chat_id = String(bot.messages[i].chat_id);
        if (chat_id != CHAT_ID) {
            bot.sendMessage(chat_id, "Unauthorized", "");
            continue;
        }

        // Print the received message
        String text = bot.messages[i].text;
        Serial.println(text);

        String from_name = bot.messages[i].from_name;

        if (text == "/start") {
            String welcome = "Welcome, " + from_name + ".\n";
            welcome += "Use the following command to get current readings.\n\n";
            welcome += "/readings \n";
            bot.sendMessage(chat_id, welcome, "");
        }

        if (text == "/readings") {
            String readings = getReadings();
            bot.sendMessage(chat_id, readings, "");
        }
    }
}

void setup() {
    pinMode(ANA, INPUT);
    pinMode(DIGI, INPUT);
    pinMode(SENSOR, OUTPUT);

    Serial.begin(115200);

    configTime(0, 0, "pool.ntp.org");
    client.setTrustAnchors(&cert);

    WiFi.mode(WIFI_STA);
    WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
    while (WiFi.status() != WL_CONNECTED) {
        delay(1000);
        Serial.println("Connecting to WiFi..");
    }
    Serial.println(WiFi.localIP());
}

void loop() {
    if (millis() > lastTimeBotRan + botRequestDelay) {
        int numNewMessages = bot.getUpdates(bot.last_message_received + 1);

        while (numNewMessages) {
            Serial.println("got response");
            handleNewMessages(numNewMessages);
            numNewMessages = bot.getUpdates(bot.last_message_received + 1);
        }
        lastTimeBotRan = millis();
    }
}
