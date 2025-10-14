#include <WiFi.h>
#include <PubSubClient.h>

// Wi-Fi credentials
const char* ssid = "Your SSID";
const char* password = "Your Password";

// MQTT Broker details
const char* mqtt_broker = "broker.hivemq.com"; // or "any Public Broker"
const int mqtt_port = 1883;

// Topic names
const char* pot_topic = "esp32/potentiometer";
const char* ldr_topic = "esp32/ldr";

// Pin assignments
const int potPin = 36; // Potentiometer analog pin (ADC1_CH6)
const int ldrPin = 39; // LDR analog pin (ADC1_CH0)

WiFiClient espClient;
PubSubClient client(espClient);

void setup() {
  Serial.begin(115200);
  setup_wifi();
  client.setServer(mqtt_broker, mqtt_port);
}

void setup_wifi() {
  delay(10);
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}

void reconnect_mqtt() {
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    String clientId = "ESP32Client-";
    clientId += String(random(0xffff), HEX);
    if (client.connect(clientId.c_str())) {
      Serial.println("connected");
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" trying again in 5 seconds");
      delay(5000);
    }
  }
}

void loop() {
  if (!client.connected()) {
    reconnect_mqtt();
  }
  client.loop();

  // Read sensor values
  int potValue = analogRead(potPin);
  int ldrValue = analogRead(ldrPin);

  // Publish potentiometer value
  client.publish(pot_topic, String(potValue).c_str());
  Serial.print("Published potentiometer value: ");
  Serial.println(potValue);

  // Publish LDR value
  client.publish(ldr_topic, String(ldrValue).c_str());
  Serial.print("Published LDR value: ");
  Serial.println(ldrValue);

  delay(2000); // Publish every 2 seconds
}
