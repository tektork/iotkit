#include <WiFi.h>
#include <HTTPClient.h>
#include <ESPAsyncWebServer.h>

// Wi-Fi Credentials
const char* ssid = "Your SSID";
const char* password = "Your Password";

// ThingSpeak
const char* thingSpeakApiKey = "Your API Key";
const char* thingSpeakURL = "http://api.thingspeak.com/update";

// Sensor and LED Pins
const int potPin = 36;
const int ldrPin = 39;
const int led1Pin = 33;
const int led2Pin = 4;

// Sensor Values
int potValue = 0;
int ldrValue = 0;
bool led1State = false;
bool led2State = false;

// Async Web Server
AsyncWebServer server(80);

// HTML Web Dashboard
const char index_html[] PROGMEM = R"rawliteral(
<!DOCTYPE html>
<html>
<head>
  <meta name="viewport" content="width=device-width, initial-scale=1.0">
  <title>ESP32 Sensor Dashboard</title>
  <style>
    body { font-family: Arial; text-align: center; padding: 20px; }
    .card { background: #f0f0f0; padding: 20px; margin: 20px auto; border-radius: 10px; width: 300px; box-shadow: 0 4px 6px rgba(0,0,0,0.2); }
    .button { padding: 10px 20px; margin: 5px; border: none; background-color: #007BFF; color: white; border-radius: 5px; cursor: pointer; }
    .button:active { background-color: #0056b3; }
  </style>
</head>
<body>
  <h2>ESP32 Smart Dashboard</h2>
  <div class="card">
    <p><strong>Potentiometer:</strong> <span id="potVal">0</span></p>
    <p><strong>LDR Value:</strong> <span id="ldrVal">0</span></p>
  </div>

  <div class="card">
    <p><strong>LED Controls</strong></p>
    <button class="button" onclick="toggleLED(1)">Toggle LED 1</button>
    <button class="button" onclick="toggleLED(2)">Toggle LED 2</button>
    <p>LED 1: <span id="led1state">OFF</span></p>
    <p>LED 2: <span id="led2state">OFF</span></p>
  </div>

  <script>
    setInterval(() => {
      fetch('/sensor').then(r => r.json()).then(data => {
        document.getElementById('potVal').innerText = data.pot;
        document.getElementById('ldrVal').innerText = data.ldr;
        document.getElementById('led1state').innerText = data.led1 ? 'ON' : 'OFF';
        document.getElementById('led2state').innerText = data.led2 ? 'ON' : 'OFF';
      });
    }, 2000);

    function toggleLED(id) {
      fetch(`/toggle?led=${id}`);
    }
  </script>
</body>
</html>
)rawliteral";

// Send data to ThingSpeak
void sendToThingSpeak(int pot, int ldr) {
  if(WiFi.status() == WL_CONNECTED){
    HTTPClient http;
    String url = String(thingSpeakURL) + "?api_key=" + thingSpeakApiKey +
                 "&field1=" + pot + "&field2=" + ldr;
    http.begin(url);
    int httpCode = http.GET();
    http.end();
  }
}

void setup() {
  Serial.begin(115200);

  pinMode(led1Pin, OUTPUT);
  pinMode(led2Pin, OUTPUT);

  WiFi.begin(ssid, password);
  Serial.print("Connecting to WiFi..");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500); Serial.print(".");
  }
  Serial.println("\nConnected! IP: " + WiFi.localIP().toString());

  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send_P(200, "text/html", index_html);
  });

  server.on("/sensor", HTTP_GET, [](AsyncWebServerRequest *request){
    potValue = analogRead(potPin);
    ldrValue = analogRead(ldrPin);
    String json = "{\"pot\":" + String(potValue) +
                  ",\"ldr\":" + String(ldrValue) +
                  ",\"led1\":" + String(led1State ? "true" : "false") +
                  ",\"led2\":" + String(led2State ? "true" : "false") + "}";
    request->send(200, "application/json", json);
  });

  server.on("/toggle", HTTP_GET, [](AsyncWebServerRequest *request){
    if (request->hasParam("led")) {
      String led = request->getParam("led")->value();
      if (led == "1") {
        led1State = !led1State;
        digitalWrite(led1Pin, led1State);
      } else if (led == "2") {
        led2State = !led2State;
        digitalWrite(led2Pin, led2State);
      }
      request->send(200, "text/plain", "OK");
    }
  });

  server.begin();
}

unsigned long lastUploadTime = 0;

void loop() {
  unsigned long currentTime = millis();
  if (currentTime - lastUploadTime > 15000) { // Upload every 15 sec
    potValue = analogRead(potPin);
    ldrValue = analogRead(ldrPin);
    sendToThingSpeak(potValue, ldrValue);
    lastUploadTime = currentTime;
  }
}
