// ======================================================
// ESP32 Sensor Data Logger (InfluxDB Cloud + MQTT Broker)
// Author: Pradeep Kumar P
// ======================================================

// ----------- HEADER FILES -----------
#include <WiFi.h>
#include <WiFiClientSecure.h>   //  Required for HTTPS (InfluxDB Cloud)
#include <InfluxDbClient.h>
#include <InfluxDbCloud.h>
#include <PubSubClient.h>

// ----------- WIFI -----------
#define WIFI_SSID       "Pradeep"
#define WIFI_PASSWORD   "123456789"

// ----------- INFLUXDB CLOUD SETTINGS -----------
#define INFLUXDB_URL    "https://us-east-1-1.aws.cloud2.influxdata.com"
#define INFLUXDB_TOKEN  "9htuKmvvtH5ynU9mznTsFc_nN5ctz0oJ47VUHNkhEkjvWUVxMjpe686moPWTn7W9GtDWNB6Z1zQF9oQ0aLIwFw=="
#define INFLUXDB_ORG    "tektork"
#define INFLUXDB_BUCKET "SensorData"
#define INFLUXDB_MEASUREMENT "esp32_sensor_data"

// Time Zone (IST = UTC + 5:30)
#define TZ_INFO "IST-5:30"

// ----------- MQTT (Raspberry Pi - Mosquitto) -----------
#define MQTT_BROKER     "192.168.95.75"
#define MQTT_PORT       1883
#define MQTT_CLIENT_ID  "esp32_sensor_client"

// Topics
#define MQTT_TOPIC_LDR  "home/esp32/ldr"
#define MQTT_TOPIC_POT  "home/esp32/pot"
#define MQTT_TOPIC_JSON "home/esp32/sensors"

// ----------- SENSOR PINS -----------
const int LDR_PIN = 39;   // GPIO36
const int POT_PIN = 36;   // GPIO34

// Reading interval (10 seconds)
const int READ_INTERVAL_MS = 10000;

// ----------- OBJECTS -----------

// Secure Client for HTTPS
WiFiClientSecure wifiClient;

//  InfluxDB client without Secure client in constructor
InfluxDBClient client(INFLUXDB_URL, INFLUXDB_ORG, INFLUXDB_BUCKET, INFLUXDB_TOKEN);

// Data structure for point
Point sensorPoint(INFLUXDB_MEASUREMENT);

// MQTT
WiFiClient mqttWiFiClient;
PubSubClient mqttClient(mqttWiFiClient);

// ----------- FUNCTIONS -----------

void connectWiFi() {
  Serial.print("Connecting to WiFi");
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);

  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(400);
  }
  Serial.println("\n WiFi Connected");
  Serial.print("IP Address: ");
  Serial.println(WiFi.localIP());
}

void connectMQTT() {
  Serial.print("Connecting to MQTT...");
  while (!mqttClient.connected()) {
    if (mqttClient.connect(MQTT_CLIENT_ID)) {
      Serial.println("Connected.");
      mqttClient.publish("home/esp32/status", "ESP32 is online");
    } else {
      Serial.print("Failed, state=");
      Serial.println(mqttClient.state());
      delay(2000);
    }
  }
}

// ----------- SETUP -----------

void setup() {
  Serial.begin(115200);

  pinMode(LDR_PIN, INPUT);
  pinMode(POT_PIN, INPUT);

  connectWiFi();

  //  Allow HTTPS communication without certificate
  wifiClient.setInsecure();
  client.setInsecure();

  timeSync(TZ_INFO, "pool.ntp.org", "time.nis.gov");

  sensorPoint.addTag("device", "esp32-01");
  sensorPoint.addTag("location", "home_lab");

  if (client.validateConnection()) {
    Serial.println(" Connected to InfluxDB Cloud");
  } else {
    Serial.print(" InfluxDB connection failed: ");
    Serial.println(client.getLastErrorMessage());
  }

  mqttClient.setServer(MQTT_BROKER, MQTT_PORT);
  connectMQTT();
}

// ----------- LOOP -----------

void loop() {

  if (WiFi.status() != WL_CONNECTED) connectWiFi();
  if (!mqttClient.connected()) connectMQTT();

  sensorPoint.clearFields();

  int ldrRaw = analogRead(LDR_PIN);
  int potRaw = analogRead(POT_PIN);

  float lightPercent = map(ldrRaw, 0, 4095, 100, 0);
  float potPercent   = map(potRaw, 0, 4095, 0, 100);

  sensorPoint.addField("ldr_raw", ldrRaw);
  sensorPoint.addField("pot_raw", potRaw);
  sensorPoint.addField("light_percent", lightPercent);
  sensorPoint.addField("pot_percent", potPercent);

  // ----------- SEND TO INFLUXDB CLO UD -----------
  if (!client.writePoint(sensorPoint)) {
    Serial.print(" InfluxDB write failed: ");
    Serial.println(client.getLastErrorMessage());
  } else {
    Serial.printf(" InfluxDB write OK → LDR=%d  POT=%d\n", ldrRaw, potRaw);
  }

  // ----------- SEND TO MQTT (Raspberry Pi) -----------
  char jsonPayload[128];
  snprintf(jsonPayload, sizeof(jsonPayload),
           "{\"ldr_raw\":%d,\"pot_raw\":%d,\"light_percent\":%.2f,\"pot_percent\":%.2f}",
           ldrRaw, potRaw, lightPercent, potPercent);

  mqttClient.publish(MQTT_TOPIC_JSON, jsonPayload);
  mqttClient.publish(MQTT_TOPIC_LDR, String(ldrRaw).c_str());
  mqttClient.publish(MQTT_TOPIC_POT, String(potRaw).c_str());

  Serial.println(" MQTT Published: ");
  Serial.println(jsonPayload);

  mqttClient.loop();
  delay(READ_INTERVAL_MS);
}
