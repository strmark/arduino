#include <ESP8266WiFi.h>
#include <WiFiClientSecure.h>
#include <UniversalTelegramBot.h>
#include <ArduinoJson.h>
#include <arduino_wifi_secrets.h>
#include <arduino_telegram_secrets.h>

#define ledPin D4

X509List cert(TELEGRAM_CERTIFICATE_ROOT);

WiFiClientSecure client;
UniversalTelegramBot bot(BOT_TOKEN, client);

int botRequestDelay = 1000;
unsigned long lastTimeBotRan;

void handleNewMessages(int numNewMessages) {
    Serial.println("handleNewMessages");
    Serial.println(String(numNewMessages));

    for (int i = 0; i < numNewMessages; i++) {
        String chat_id = String(bot.messages[i].chat_id);
        if (chat_id != CHAT_ID) {
            bot.sendMessage(chat_id, "Unauthorized user", "");
            continue;
        }

        String text = bot.messages[i].text;
        Serial.println(text);

        String from_name = bot.messages[i].from_name;

        if (text == "/start") {
            String welcome = "Welcome, " + from_name + ".\n";
            welcome += "Use the following commands to control your outputs.\n\n";
            welcome += "/led_on to turn LED ON \n";
            welcome += "/led_off to turn LED OFF \n";
            welcome += "/state to request current LED state \n";
            bot.sendMessage(chat_id, welcome, "");
        }

        // LED active is low
        if (text == "/led_on") {
            bot.sendMessage(chat_id, "LED state set to ON", "");
            digitalWrite(ledPin, LOW);
        }

        if (text == "/led_off") {
            bot.sendMessage(chat_id, "LED state set to OFF", "");
            digitalWrite(ledPin, HIGH);
        }

        if (text == "/state") {
            if (digitalRead(ledPin)) {
                bot.sendMessage(chat_id, "LED is OFF", "");
            } else {
                bot.sendMessage(chat_id, "LED is ON", "");
            }
        }
    }
}

void setup() {
    Serial.begin(115200);

    configTime(0, 0, "pool.ntp.org");
    client.setTrustAnchors(&cert);

    pinMode(ledPin, OUTPUT);
    digitalWrite(ledPin, LOW);

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
