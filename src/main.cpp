// Arduino.h
#include <Arduino.h>
// sensors
#include <Seeed_BME280.h>
// json
#include <ArduinoJson.h>
// MQTT
#include <esp32-mqtt.h>

// sensor instance
BME280 bme;

// JSON
DynamicJsonDocument doc(1024);

// prototype
void publish();
float missenard(float temp, float humid);

void setup() {
  // Serial start
  Serial.begin(115200);
  // standby...
  delay(10000);
  Serial.println("***\n***\n***\nMain program activated!");
  // MQTT set
  setupCloudIoT();
  // sensor start
  if (!bme.init()) {
    Serial.println("Device error!");
  }
  // led start
  pinMode(4, OUTPUT);
}

// timer var
unsigned long lastMillis = 0;

void loop() {
  mqttClient->loop();
  delay(10); // <- fixes some issues with WiFi stability
  // connect
  if (!mqttClient->connected()) {
    connect();
  }
  // publish frequency
  if (millis() - lastMillis > 30000) {
    lastMillis = millis();
    publish();
  }
}

void publish() {
  float temp = bme.getTemperature();
  float humid = bme.getHumidity();
  float apparentTemp = missenard(temp, humid);
  // Serial output
  Serial.print("Temperature: ");
  Serial.print(temp);
  Serial.println("C");
  Serial.print("Humidity: ");
  Serial.print(humid);
  Serial.println("%");
  Serial.print("ApparentTemp: ");
  Serial.print(apparentTemp);
  Serial.println("C");
  // JSON
  doc["Temperature"] = temp;
  doc["Humidity"] = humid;
  doc["ApparentTemp"] = apparentTemp;
  String output;
  serializeJson(doc, output);
  Serial.println(output);
  // publish
  publishTelemetry(output);
  // LED signal
  digitalWrite(4, HIGH);  // turn the LED on (HIGH is the voltage level)
  delay(100);             // wait for a second
  digitalWrite(4, LOW);   // turn the LED off by making the voltage LOW
  delay(100);

  Serial.println();
}

float missenard(float temp, float humid) {
  float m;
  m = temp - (1 / 2.3) * (temp - 10) * (0.8 - humid / 100);
  return m;
}