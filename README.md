# Arduino Sketch Files

### arduino_secrets
Example header files for storing the secrets to be used inside the Sketch Files. So they are not stored inside the Sketch Files.

Copy the folder to the Arduino Library folder and adjust them with the correct settings.

### Projects

##### ESP8266 sketch files

- blink_led test sketch file which blinks the led on the esp8266
- cayenne_mqtt cayenne test example
- dht_moist_pub 
- dht_tester test sketch file for testing the DHT11 sensor
- dhtpub dht and mqtt sketch file sending DHT11 sensor data to a MQTT broker
- influxdb test sketch file which sends wifi data to influxdb
- influxdb_dht_moisture sends dht11 and capacitive soil moisture sensor data to influxdb
- influxdb_dht22 sends dht22 sensor data to influxdb
- moisture_sensor_test test sketch file for the capacitive soil moisture sensor
- MoistureSensor sketch file for sending data to influxdb from a moisture sensor with analog and digital input
- mqtt_dht_moist MQTT sketch file for moisture and dht11 with a node-red dashboard and switching the waterpump from node-red [for more information](./mqtt_dht_moist/Mqtt.md)
- mqtt_esp8266 MQTT test file
- plant_watering_system simple plant watering system which read moisture data and switches the waterpump on an off
- ports translation of the Digital portnumbers to portnumbers [ports](./ports/ports.md)
- slack sketch file for slack integration, sent a message to your slack webhook [info](./slack/slack.md)
- telegram_blink swith the led from esp8266 on and off with a telegram bot
- telegram_sensor read the dht11 sensor data with a telegram bot
