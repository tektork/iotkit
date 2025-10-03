#define DO_PIN 16 // Define GPIO16 on ESP32 as input pin connected to MQ-2 "DO"(digital output)

void setup() {
  Serial.begin(115200); // Start Serial Monitor communication at 115200 baud
  pinMode(DO_PIN, INPUT); // Configure DO pin as input(reads HIGH or LOW from the MQ-2)
  
  // MQ-2 requires heater warm-up for stable readings
  Serial.println("Warming up the MQ2 sensor");
  delay(20000);  // Wait 20 seconds before starting detection
}

void loop() {
  // Read digital output from MQ-2 module
  // DO pin gives HIGH when gas concentration is below threshold
  // DO pin gives LOW when gas concentration exceeds threshold
  int gasState = digitalRead(DO_PIN);
  
  // Interpret and print the sensor state
  if (gasState == HIGH) {
    Serial.println("The gas is NOT present");    // Safe condition
  } else {
    Serial.println("The gas is present");       // Gas detected above threshold
  }
  
  delay(1000); // Add delay to prevent serial monitor flooding
}