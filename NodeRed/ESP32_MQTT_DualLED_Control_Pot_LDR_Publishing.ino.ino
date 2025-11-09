#include <WiFi.h>
#include <PubSubClient.h>

// Set these for your WiFi
const char* ssid = "H1";
const char* password = "raspberrypi";

// MQTT broker settings
const char* mqtt_server = "test.mosquitto.org"; // or your own broker
const int mqtt_port = 1883;

// Topics for LED control (subscribing to new topics)
const char* topic_led3 = "esp32van/led3";
const char* topic_led4 = "esp32van/led4";

// Topics for sensor publishing
const char* topic_pot = "esp32van/pot1";
const char* topic_ldr = "esp32van/ldr1";

// GPIO pins for the LEDs
#define LED3_PIN 33  // LED 3
#define LED4_PIN 4   // LED 4

// ADC pins for sensors
#define POT_PIN 36  // ADC pin for potentiometer
#define LDR_PIN 39  // ADC pin for LDR

WiFiClient espClient;
PubSubClient client(espClient);

// Callback function to handle MQTT messages
void callback(char* topic, byte* payload, unsigned int length) {
  String msg;
  for (unsigned int i = 0; i < length; i++) {
    msg += (char)payload[i];
  }
  msg.trim();

  if (strcmp(topic, topic_led3) == 0) {
    if (msg == "on" || msg == "1") {
      digitalWrite(LED3_PIN, HIGH);
    } else if (msg == "off" || msg == "0") {
      digitalWrite(LED3_PIN, LOW);
    }
  } else if (strcmp(topic, topic_led4) == 0) {
    if (msg == "on" || msg == "1") {
      digitalWrite(LED4_PIN, HIGH);
    } else if (msg == "off" || msg == "0") {
      digitalWrite(LED4_PIN, LOW);
    }
  }
}

void setup_wifi() {
  delay(10);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
  }
}

void reconnect() {
  while (!client.connected()) {
    if (client.connect("ESP32_Combined_LED_Sensor_Client")) {
      client.subscribe(topic_led3);
      client.subscribe(topic_led4);
    } else {
      delay(2000);
    }
  }
}

void setup() {
  Serial.begin(115200);
  pinMode(LED3_PIN, OUTPUT);
  pinMode(LED4_PIN, OUTPUT);
  digitalWrite(LED3_PIN, LOW);
  digitalWrite(LED4_PIN, LOW);
  setup_wifi();
  client.setServer(mqtt_server, mqtt_port);
  client.setCallback(callback);
}

void loop() {
  if (!client.connected()) {
    reconnect();
  }
  client.loop();

  int potValue = analogRead(POT_PIN);
  int ldrValue = analogRead(LDR_PIN);

  char buffer[16];
  snprintf(buffer, sizeof(buffer), "%d", potValue);
  client.publish(topic_pot, buffer);

  snprintf(buffer, sizeof(buffer), "%d", ldrValue);
  client.publish(topic_ldr, buffer);

  delay(1000); // 1 Hz publishing
}