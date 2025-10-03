#define IR_SENSOR_PIN 13  // Define ESP32 GPIO13 as the digital input pin connected to the IR sensor output

void setup() {
  Serial.begin(115200);           // Initialize serial communication at 115200 baud
  pinMode(IR_SENSOR_PIN, INPUT);  // Configure GPIO13 as input because sensor sends digital HIGH/LOW signals
  Serial.println("IR Obstacle Sensor Test");  // Print header message for reference on serial monitor
  Serial.println("------------------------");
}

void loop() {
  int sensorValue = digitalRead(IR_SENSOR_PIN);  // Read digital state from IR sensor pin (HIGH or LOW)

  // Typical IR obstacle sensor logic:
  // - LOW signal means obstacle detected (sensor output active low)
  // - HIGH signal means no obstacle detected
  if (sensorValue == LOW) {
    Serial.println("Obstacle detected!");  // Print detection message when an obstacle is present
  } else {
    Serial.println("No obstacle");          // Print when no obstacle is detected
  }

  delay(1000);  // Delay 1 second before next reading
}
