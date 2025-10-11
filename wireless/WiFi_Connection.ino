#include <WiFi.h>  // Include the WiFi library for ESP32

// Replace with your WiFi credentials
const char* ssid = "Your SSID";         // Your WiFi network name
const char* password = "Your Password"; // Your WiFi password

void setup() {
  Serial.begin(115200);   // Initialize serial communication at 115200 baud
  delay(1000);            // Wait for serial to initialize

  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.begin(ssid, password);  // Start connecting to WiFi

  // Wait until the ESP32 is connected
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");     // Print dots on serial monitor while waiting
  }

  Serial.println();
  Serial.println("WiFi Connected!");       // Connected successfully
  Serial.print("Connected to: ");
  Serial.println(WiFi.SSID());               // Print connected network name
  Serial.print("IP Address: ");
  Serial.println(WiFi.localIP());            // Print ESP32 IP address
}

void loop() {
  // Empty loop - Add repeated code here if needed
}
