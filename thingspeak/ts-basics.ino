#include <WiFi.h>
#include <WebServer.h>
#include <HTTPClient.h>

// WiFi Credentials
const char* ssid = "Your SSID";
const char* password = "Your Password";

// ThingSpeak Settings
const char* thingspeakServer = "http://api.thingspeak.com/update";
const String writeAPIKey = "CR9CQ5QZV7Q3N18T";

// Sensor Pin
const int potPin = 36;  // ADC1_CH0

// Web Server
WebServer server(80);

// Global Variables
int potValue = 0;
unsigned long lastUpload = 0;
const unsigned long uploadInterval = 20000;  // 20 seconds (minimum for ThingSpeak)

void setup() {
  Serial.begin(115200);
  WiFi.begin(ssid, password);
  Serial.print("Connecting to WiFi");

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nConnected! IP address: " + WiFi.localIP().toString());

  // Web Routes
  server.on("/", handleRoot);
  server.on("/data", handleData);
  server.begin();
  Serial.println("Web server started");
}

void loop() {
  potValue = analogRead(potPin);

  // Upload to ThingSpeak
  if (millis() - lastUpload >= uploadInterval) {
    sendToThingSpeak(potValue);
    lastUpload = millis();
  }

  server.handleClient();
}

void sendToThingSpeak(int value) {
  if (WiFi.status() == WL_CONNECTED) {
    HTTPClient http;
    String postData = "api_key=" + writeAPIKey + "&field1=" + String(value);
    http.begin(thingspeakServer);
    http.addHeader("Content-Type", "application/x-www-form-urlencoded");

    int httpResponseCode = http.POST(postData);
    if (httpResponseCode == 200) {
      Serial.println("Data sent to ThingSpeak: " + String(value));
    } else {
      Serial.println("Failed to send. HTTP code: " + String(httpResponseCode));
    }
    http.end();
  }
}

// HTML Dashboard
void handleRoot() {
  String html = "<!DOCTYPE html><html><head><meta charset='UTF-8'>";
  html += "<meta name='viewport' content='width=device-width, initial-scale=1'>";
  html += "<meta http-equiv='refresh' content='5'>";
  html += "<title>POT Dashboard</title>";
  html += "<style>body{font-family:sans-serif;padding:20px;}</style></head><body>";
  html += "<h2>ESP32 Potentiometer Monitoring</h2>";
  html += "<p><strong>POT Value:</strong> " + String(potValue) + "</p>";
  html += "<p><a href='/data'>JSON Output</a></p>";
  html += "</body></html>";

  server.send(200, "text/html", html);
}

// JSON API
void handleData() {
  String json = "{\"pot\":" + String(potValue) + "}";
  server.send(200, "application/json", json);
}
