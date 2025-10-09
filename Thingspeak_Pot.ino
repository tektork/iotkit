#include <WiFi.h>            // Include Wi-Fi library for ESP32
#include <ThingSpeak.h>      // Include ThingSpeak library for IoT data communication

// Wi-Fi credentials - replace with your network's SSID and password
const char* ssid = "Your_SSID";
const char* password = "123456789";

WiFiClient client;           // Create a Wi-Fi client object for network communication

// ThingSpeak channel configuration
unsigned long myChannelNumber = 3103881;    // Your ThingSpeak Channel ID (replace this)
const char* myWriteAPIKey = "ZULIBBGEJC7TY4U0";  // Your ThingSpeak Write API Key (replace this)

// Potentiometer connection pin on ESP32
const int potPin = 36;        // GPIO 3 analog input connected to potentiometer output

// Variables for timing data uploads to ThingSpeak
unsigned long lastTime = 0;           // Stores the timestamp of the last upload
unsigned long timerDelay = 20000;     // Delay between uploads (20 seconds, free ThingSpeak limit)

void setup() {
  Serial.begin(115200);               // Start serial communication at 115200 baud for debugging

  // Connect to Wi-Fi as station mode
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  Serial.print("Connecting to WiFi ..");
  
  // Wait in a loop until connection is established
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print('.');                // Print dots as connection attempts
    delay(1000);                     // Wait 1 second before retrying
  }
  
  // Connection successful - print confirmation and network details
  Serial.println("\n Connected successfully!");
  Serial.print(" SSID: ");
  Serial.println(WiFi.SSID());        // Print connected Wi-Fi SSID
  Serial.print(" IP Address: ");
  Serial.println(WiFi.localIP());     // Print assigned ESP32 IP address
  
  ThingSpeak.begin(client);            // Initialize ThingSpeak using the Wi-Fi client
}

void loop() {
  // Execute this block only after timerDelay time has elapsed
  if ((millis() - lastTime) > timerDelay) {
    
    // Check if Wi-Fi is still connected
    if (WiFi.status() != WL_CONNECTED) {
      Serial.print("Attempting to reconnect to WiFi");
      WiFi.begin(ssid, password);
      // Reconnect until successful
      while (WiFi.status() != WL_CONNECTED) {
        Serial.print('.');
        delay(5000);
      }
      Serial.println("\nReconnected.");
      
      // Print Wi-Fi connection details after reconnecting
      Serial.print(" SSID: ");
      Serial.println(WiFi.SSID());
      Serial.print(" IP Address: ");
      Serial.println(WiFi.localIP());
    }
    
    // Read analog potentiometer value (range from 0 to 4095)
    int potValue = analogRead(potPin);
    Serial.println("Potentiometer Value: " + String(potValue));  // Print value to serial monitor
    
    // Write potentiometer value to ThingSpeak channel Field 1
    int x = ThingSpeak.writeField(myChannelNumber, 1, potValue, myWriteAPIKey);
    
    // Check response of ThingSpeak update attempt
    if (x == 200) {
      Serial.println("Channel update successful.");          // Success message
    } else {
      Serial.println("Problem updating channel. HTTP error code " + String(x));  // Error message
    }
    
    lastTime = millis();    // Update last upload time to current time
  }
}
