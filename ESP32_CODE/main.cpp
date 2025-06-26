#include <Arduino.h>
#include <WiFi.h>
#include <PsychicMqttClient.h>
#include <Wire.h>
#include <Adafruit_BME280.h>

// ----- WiFi Configuration -----
const char ssid[] = "<YOUR_WIFI_SSID>";
const char pass[] = "<YOUR_WIFI_PASSWORD>";

// ----- Let's Encrypt Certificate (ISRG Root X1) -----
const char *letsencrypt_root_ca = "-----BEGIN CERTIFICATE-----\n"
// [Certificate content truncated for clarity in README]
// Copy full ISRG Root X1 certificate here.
"-----END CERTIFICATE-----\n";

// ----- MQTT Client -----
PsychicMqttClient mqttClient;

// ----- Sensor Pins -----
const int mq2AnalogPin = 34;
const int mq2DigitalPin = 32;
const int fireSensorPin = 35;

// ----- BME280 Sensor -----
Adafruit_BME280 bme;

// ----- MQTT Publish Interval -----
#define PUBLISH_INTERVAL 5000
unsigned long lastPublishTime = 0;

// ----- MQTT Callback -----
void onTopic(const char *topic, const char *payload, int retain, int qos, bool dup) {
  Serial.printf("Received Topic: %s\r\n", topic);
  Serial.printf("Received Payload: %s\r\n", payload);
}

void setup() {
  Serial.begin(115200);
  delay(1000);

  Wire.begin(23, 22);

  if (!bme.begin(0x76)) {
    Serial.println("Could not find a valid BME280 sensor, check wiring!");
    while (1) delay(1000);
  }

  pinMode(mq2DigitalPin, INPUT);
  pinMode(fireSensorPin, INPUT);

  Serial.printf("Connecting to WiFi %s...", ssid);
  WiFi.begin(ssid, pass);
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(500);
  }
  Serial.printf("\nConnected. IP address: %s\n", WiFi.localIP().toString().c_str());

  mqttClient.setServer("mqtts://<YOUR_MQTT_BROKER>:<PORT>");
  mqttClient.setCredentials("<MQTT_USERNAME>", "<MQTT_PASSWORD>");
  mqttClient.setCACert(letsencrypt_root_ca);
  mqttClient.onTopic("<ESP32_ID>/test", 2, onTopic);

  mqttClient.connect();
  while (!mqttClient.connected()) {
    Serial.print(".");
    delay(500);
  }
  Serial.println("\nMQTT connected.");
  mqttClient.publish("<ESP32_ID>/test", 2, false, "ESP32 Sensor Module Online");
}

void loop() {
  unsigned long now = millis();
  if (now - lastPublishTime >= PUBLISH_INTERVAL) {
    lastPublishTime = now;

    float temperature = bme.readTemperature();
    float pressure    = (bme.readPressure() / 100.0F);
    float humidity    = bme.readHumidity();

    int mq2Raw = analogRead(mq2AnalogPin);
    float mq2Voltage = mq2Raw * (3.3 / 4095.0);

    bool gasDetected  = (digitalRead(mq2DigitalPin) == LOW);
    bool fireDetected = (digitalRead(fireSensorPin) == LOW);

    String tempStr  = String(temperature, 2);
    String presStr  = String(pressure, 2);
    String humidStr = String(humidity, 2);
    String gasStr   = String(mq2Voltage, 2);

    mqttClient.publish("<ESP32_ID>/temperature", 0, false, tempStr.c_str());
    mqttClient.publish("<ESP32_ID>/pressure",    0, false, presStr.c_str());
    mqttClient.publish("<ESP32_ID>/humidity",    0, false, humidStr.c_str());
    mqttClient.publish("<ESP32_ID>/gas_voltage", 2, false, gasStr.c_str());

    if (gasDetected) {
      mqttClient.publish("<ESP32_ID>/gas/alarm", 2, false, "1");
      Serial.println("🚨 Gas alarm: gas detected");
    }
    if (fireDetected) {
      mqttClient.publish("<ESP32_ID>/fire/alarm", 2, false, "1");
      Serial.println("🔥 Fire alarm: fire detected");
    }

    Serial.print("Temperature: "); Serial.println(tempStr);
    Serial.print("Pressure:    "); Serial.println(presStr);
    Serial.print("Humidity:    "); Serial.println(humidStr);
    Serial.print("Gas Voltage: "); Serial.println(gasStr);
    Serial.print("Gas Status:  "); Serial.println(gasDetected  ? "🚨 Gas Detected!"  : "✅ No gas");
    Serial.print("Fire Status: "); Serial.println(fireDetected ? "🔥 Fire Detected!" : "✅ No fire");
    Serial.println("-------------------------");
  }
}
