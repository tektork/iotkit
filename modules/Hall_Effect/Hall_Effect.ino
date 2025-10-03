int val = 0;  // Variable to store sensor reading

void setup() {
  Serial.begin(115200);                // Start serial communication at 115200 baud
  Serial.println("ESP32 Hall Effect Sensor Test");  // Print startup message
}

void loop() {
  val = hallRead();   // Read raw value from the built-in Hall effect sensor
  
  Serial.print("Hall Value: ");
  Serial.print(val);  // Print the sensor value
  
  // Check if the absolute value is beyond threshold (|val| > 50)
  // Larger values indicate stronger magnetic field detection
  if (abs(val) > 50) {  
    Serial.println(" - Magnetic field detected!");
  } else {
    Serial.println(" - No magnetic field");
  }
  
  delay(1000);   // Wait 1 second before next reading
}
