#include <WiFi.h>               // WiFi library for ESP32
#include <PubSubClient.h>       // MQTT client library

// WiFi credentials (SSID and Password)
const char* ssid = "Your SSID";
const char* password = "Your Password";

// Public MQTT broker info (HiveMQ broker)
const char* mqtt_server = "broker.hivemq.com";  
const int mqtt_port = 1883;                     // MQTT default port
const char* mqtt_topic_led1 = "esp32/led1";     // Topic for LED1 control
const char* mqtt_topic_led2 = "esp32/led2";     // Topic for LED2 control

WiFiClient espClient;        // Create WiFi client
PubSubClient client(espClient); // Create MQTT client using WiFi

// Define LED GPIO pins
#define LED1 33    // LED1 connected to GPIO33
#define LED2 4     // LED2 connected to GPIO4

// Function to connect ESP32 to WiFi
void setup_wifi() {
  delay(10);
  Serial.println();
  Serial.print("Connecting to WiFi: ");
  Serial.println(ssid);

  WiFi.begin(ssid, password); // Start WiFi connection
  while (WiFi.status() != WL_CONNECTED) { // Wait until connected
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nWiFi connected!");      // Success message
  Serial.print("IP address: ");             
  Serial.println(WiFi.localIP());           // Print IP assigned to ESP32
}

// MQTT callback function to handle incoming messages
void callback(char* topic, byte* payload, unsigned int length) {
  payload[length] = '\0';                   // Null-terminate payload for String conversion
  String message = String((char*)payload);  // Convert payload to String
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("]: ");
  Serial.println(message);

  // Control LED1 based on MQTT message
  if (String(topic) == mqtt_topic_led1) {
    if (message == "ON")  digitalWrite(LED1, HIGH); // Turn LED1 ON
    else if (message == "OFF") digitalWrite(LED1, LOW); // Turn LED1 OFF
  }

  // Control LED2 based on MQTT message
  if (String(topic) == mqtt_topic_led2) {
    if (message == "ON")  digitalWrite(LED2, HIGH); // Turn LED2 ON
    else if (message == "OFF") digitalWrite(LED2, LOW); // Turn LED2 OFF
  }
}

// Function to reconnect to MQTT broker if disconnected
void reconnect() {
  while (!client.connected()) { // Loop until connected
    Serial.print("Connecting to MQTT...");
    // Connect to broker with unique client ID
    if (client.connect("ESP32_LED_Controller")) {
      Serial.println("connected");
      // Subscribe to both LED control topics
      client.subscribe(mqtt_topic_led1);
      client.subscribe(mqtt_topic_led2);
      Serial.println("Subscribed to LED topics");
    } else { // If failed, wait and retry
      Serial.print("failed, rc=");
      Serial.print(client.state()); // Print error code
      Serial.println(" retrying in 5s");
      delay(5000);
    }
  }
}

// Initial setup
void setup() {
  Serial.begin(115200);      // Start serial monitor
  pinMode(LED1, OUTPUT);     // Set LED1 pin as output
  pinMode(LED2, OUTPUT);     // Set LED2 pin as output
  digitalWrite(LED1, LOW);   // Initially turn OFF LED1
  digitalWrite(LED2, LOW);   // Initially turn OFF LED2

  setup_wifi();              // Connect to WiFi
  client.setServer(mqtt_server, mqtt_port); // Set MQTT server & port
  client.setCallback(callback); // Set MQTT callback function
}

// Main loop
void loop() {
  if (!client.connected()) reconnect(); // Ensure MQTT connection
  client.loop(); // Process incoming messages and keep connection alive
}
