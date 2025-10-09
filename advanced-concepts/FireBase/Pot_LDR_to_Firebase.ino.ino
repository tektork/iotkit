#include <WiFi.h>
#include <FirebaseESP32.h> // By Mobizt

#define WIFI_SSID "S23FE"
#define WIFI_PASSWORD "Sanjey123@"

// Firebase project details - note https:// and trailing slash in database_url
#define FIREBASE_HOST "https://esp32-12b7b-default-rtdb.firebaseio.com/"
#define FIREBASE_AUTH "AIzaSyBla5H-b7BtCG4R4bGjv7rwAew4l4A6TVk"

// Use pins from your image:
// POT (RP1) --> GPIO 36
// LDR (R12) --> GPIO 39
#define POT_PIN 36
#define LDR_PIN 39

FirebaseData fbData;
FirebaseConfig config;
FirebaseAuth auth;

void setup() {
  Serial.begin(115200);

  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.print("Connecting to WiFi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nWiFi connected");
  Serial.print("IP: ");
  Serial.println(WiFi.localIP());

  // Configure Firebase
  config.database_url = FIREBASE_HOST;
  auth.api_key = FIREBASE_AUTH;

  Firebase.begin(&config, &auth);
  Firebase.reconnectWiFi(true);
}

void loop() {
  int potValue = analogRead(POT_PIN); // Read potentiometer
  int ldrValue = analogRead(LDR_PIN); // Read LDR

  Serial.printf("POT: %d, LDR: %d\n", potValue, ldrValue);

  // Send POT value
  if (Firebase.setInt(fbData, "/sensors/pot", potValue)) {
    Serial.println("POT value sent to Firebase");
  } else {
    Serial.printf("POT ERROR: %s\n", fbData.errorReason().c_str());
  }

  // Send LDR value
  if (Firebase.setInt(fbData, "/sensors/ldr", ldrValue)) {
    Serial.println("LDR value sent to Firebase");
  } else {
    Serial.printf("LDR ERROR: %s\n", fbData.errorReason().c_str());
  }

  delay(10000); // Wait 10 seconds before sending again
}
