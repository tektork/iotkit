#include <WiFi.h>
#include <HTTPClient.h>

// Replace with your network credentials
const char* ssid = "S23FE";
const char* password = "Sanjey123@";

// Google Sheets API credentials
const char* google_script_id = "AKfycbw6Oecf6kymGD27SO1jWXdTTAFgPQO5C2Cm0q27-QHEkV8S-rXZCm7GPY_PJOpZGFS1Kg"; // Your Google Apps Script deployment ID

// Analog pins
const int potPin = 36;    // Potentiometer pin
const int ldrPin = 39;    // LDR pin

void setup() {
  Serial.begin(115200);
  WiFi.begin(ssid, password);
  
  // Set analog pins as input
  pinMode(potPin, INPUT);
  pinMode(ldrPin, INPUT);
  
  Serial.print("Connecting to WiFi...");
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.print(".");
  }
  Serial.println("\nConnected to WiFi");
  Serial.println("Starting sensor readings...");
}

void loop() {
  if (WiFi.status() == WL_CONNECTED) {
    HTTPClient http;
    
    // Read analog values
    int potValue = analogRead(potPin);
    int ldrValue = analogRead(ldrPin);
    
    // Print values for debugging
    Serial.print("Pot Value: ");
    Serial.print(potValue);
    Serial.print(", LDR Value: ");
    Serial.println(ldrValue);
    
    // Create URL with both pot and ldr parameters (raw values)
    String url = String("https://script.google.com/macros/s/") + google_script_id + 
                 "/exec?pot=" + String(potValue) + "&ldr=" + String(ldrValue);
    
    Serial.println("Sending request to: " + url);
    
    http.begin(url);
    int httpCode = http.GET();
    
    if (httpCode > 0) {
      String payload = http.getString();
      Serial.println("Response Code: " + String(httpCode));
      Serial.println("Response: " + payload);
    } else {
      Serial.println("Error in sending request. HTTP Code: " + String(httpCode));
    }
    
    http.end();
  } else {
    Serial.println("WiFi Disconnected");
    // Try to reconnect
    WiFi.begin(ssid, password);
  }
  
  delay(10000); // Send data every 10 seconds
}