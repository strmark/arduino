#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <ESPAsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include "DHTesp.h"
#include <arduino_wifi_secrets.h>

#define DHTPIN D3
#define DHTSWITCH D2

DHTesp dht;

float temp = 0.0;
float hum = 0.0;

AsyncWebServer server(80);

unsigned long previousMillis = 0;    // will store last time DHT was updated

// Updates readings every 60 seconds
const long interval = 1000*60;  

const char index_html[] PROGMEM = R"rawliteral(
<!DOCTYPE HTML><html>
<head>
  <meta name="viewport" content="width=device-width, initial-scale=1">
  <link rel="stylesheet" href="https://use.fontawesome.com/releases/v5.15.4/css/all.css" integrity="sha384-DyZ88mC6Up2uqS4h/KRgHuoeGwBcD4Ng9SiP4dIRy0EXTlnuz47vAwmeGwVChigm" crossorigin="anonymous">
  <style>
    html {
     font-family: Arial;
     display: inline-block;
     margin: 0px auto;
     text-align: center;
    }
    h2 { font-size: 3.0rem; }
    p { font-size: 3.0rem; }
    .units { font-size: 1.2rem; }
    .dht-labels{
      font-size: 1.5rem;
      vertical-align:middle;
      padding-bottom: 15px;
    }
  </style>
</head>
<body>
  <h2>DHT Server</h2>
  <p>
    <i class="fas fa-temperature-high" style="color:#9e0519;"></i>
    <span class="dht-labels">Temperature</span> 
    <span id="temperature">%TEMPERATURE%</span>
    <sup class="units">&deg;C</sup>
  </p>
  <p>
    <i class="fas fa-tint" style="color:#05199e;"></i> 
    <span class="dht-labels">Humidity</span>
    <span id="humidity">%HUMIDITY%</span>
    <sup class="units">%</sup>
  </p>
</body>
<script>
setInterval(function ( ) {
  var xhttp = new XMLHttpRequest();
  xhttp.onreadystatechange = function() {
    if (this.readyState == 4 && this.status == 200) {
      document.getElementById("temperature").innerHTML = this.responseText;
    }
  };
  xhttp.open("GET", "/temperature", true);
  xhttp.send();
}, 10000 ) ;

setInterval(function ( ) {
  var xhttp = new XMLHttpRequest();
  xhttp.onreadystatechange = function() {
    if (this.readyState == 4 && this.status == 200) {
      document.getElementById("humidity").innerHTML = this.responseText;
    }
  };
  xhttp.open("GET", "/humidity", true);
  xhttp.send();
}, 10000 ) ;
</script>
</html>)rawliteral";

String processor(const String& var){
  if(var == "TEMPERATURE"){
    return String(temp);
  }
  if(var == "HUMIDITY"){
    return String(hum);
  }
  return String();
}

void setup(){
  Serial.begin(115200);
  dht.setup(DHTPIN, DHTesp::DHT22);
  pinMode(DHTSWITCH, OUTPUT);

  Serial.println(WIFI_SSID);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.println("Connecting to WiFi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println(".");
  }
  Serial.println(WiFi.localIP());

  // Route for web pages
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send_P(200, "text/html", index_html, processor);
  });
  server.on("/temperature", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send_P(200, "text/plain", String(temp).c_str());
  });
  server.on("/humidity", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send_P(200, "text/plain", String(hum).c_str());
  });

  server.begin();
}
 
void loop(){  
  unsigned long currentMillis = millis();
  if (currentMillis - previousMillis >= interval) {
    // save the last time you updated the DHT values
    previousMillis = currentMillis;
    digitalWrite(DHTSWITCH, HIGH);
    //bug DHT11 = 1000 else 2000
    delay(dht.getMinimumSamplingPeriod() * 2);

    float newTemp = dht.getTemperature();
    float newHum = dht.getHumidity();

    if (isnan(newTemp) || isnan(newHum)) {
      Serial.println("Failed to read from DHT sensor!");
    } else {
      temp = newTemp;
      Serial.println(temp);
      hum = newHum;
      Serial.println(hum);
    }
    digitalWrite(DHTSWITCH, LOW);
  }
}
