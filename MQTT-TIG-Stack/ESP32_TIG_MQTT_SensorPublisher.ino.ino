#include <WiFi.h>
#include <PubSubClient.h>

// WiFi credentials
const char* ssid = "S23FE";
const char* password = "Sanjey123@";

// MQTT broker info (HiveMQ public broker)
const char* mqtt_server = "broker.hivemq.com";
const int mqtt_port = 1883;
const char* mqtt_topic = "iot/sensor";

// Analog pins for sensors
const int potPin = 36;  // Example analog pin for potentiometer
const int ldrPin = 39;  // Example analog pin for LDR

WiFiClient espClient;
PubSubClient client(espClient);

void setup_wifi() {
  delay(10);
  Serial.println();
  Serial.print("Connecting to WiFi: ");
  Serial.println(ssid);

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
}

void reconnect() {
  // Loop until reconnected
  while (!client.connected()) {
    Serial.print("Connecting to MQTT...");
    if (client.connect("ESP32Client")) {
      Serial.println("connected");
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      delay(5000);
    }
  }
}

void setup() {
  Serial.begin(115200);
  setup_wifi();
  client.setServer(mqtt_server, mqtt_port);
}

void loop() {
  if (!client.connected()) {
    reconnect();
  }
  client.loop();

  // Read analog values
  int potValue = analogRead(potPin);
  int ldrValue = analogRead(ldrPin);

  // Construct JSON payload
  String payload = "{";
  payload += "\"pot\":";
  payload += potValue;
  payload += ",";
  payload += "\"ldr\":";
  payload += ldrValue;
  payload += "}";

  Serial.print("Publishing message: ");
  Serial.println(payload);

  client.publish(mqtt_topic, payload.c_str());

  delay(10000);  // Wait 10 seconds (should match Telegraf interval)
}
