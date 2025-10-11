#include "WiFi.h"

void setup() {
  Serial.begin(115200);
  
  // Set WiFi to station mode and disconnect from any AP
  WiFi.mode(WIFI_STA);
  WiFi.disconnect();
  delay(100);
  
  Serial.println("Setup completed");
}

void loop() {
  Serial.println("Scanning for networks...");
  
  // Perform WiFi scan
  int networkCount = WiFi.scanNetworks();
  Serial.println("Scan completed");
  
  if (networkCount == 0) {
    Serial.println("No networks found");
  } else {
    Serial.print(networkCount);
    Serial.println(" networks found:");
    
    // Display network details in table format
    Serial.println("Nr | SSID                             | RSSI | CH | Encryption");
    
    for (int i = 0; i < networkCount; i++) {
      Serial.printf("%2d", i + 1);
      Serial.print(" | ");
      Serial.printf("%-32.32s", WiFi.SSID(i).c_str());
      Serial.print(" | ");
      Serial.printf("%4d", WiFi.RSSI(i));
      Serial.print(" | ");
      Serial.printf("%2d", WiFi.channel(i));
      Serial.print(" | ");
      
      // Print encryption type
      switch (WiFi.encryptionType(i)) {
        case WIFI_AUTH_OPEN: Serial.print("Open"); break;
        case WIFI_AUTH_WEP: Serial.print("WEP"); break;
        case WIFI_AUTH_WPA_PSK: Serial.print("WPA"); break;
        case WIFI_AUTH_WPA2_PSK: Serial.print("WPA2"); break;
        case WIFI_AUTH_WPA_WPA2_PSK: Serial.print("WPA+WPA2"); break;
        case WIFI_AUTH_WPA2_ENTERPRISE: Serial.print("WPA2-EAP"); break;
        case WIFI_AUTH_WPA3_PSK: Serial.print("WPA3"); break;
        case WIFI_AUTH_WPA2_WPA3_PSK: Serial.print("WPA2+WPA3"); break;
        case WIFI_AUTH_WAPI_PSK: Serial.print("WAPI"); break;
        default: Serial.print("Unknown");
      }
      Serial.println();
    }
  }
  
  // Clear scan results to free memory
  WiFi.scanDelete();
  
  // Wait before scanning again
  delay(5000);
}