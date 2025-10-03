const int moisturePin = 34;     // GPIO34 (ADC1_CH0) is used for analog input
const int dryValue = 4095;      // ADC value when sensor is completely dry (in air)
const int wetValue = 1500;      // ADC value when sensor is fully wet (dipped in water)

void setup() {
  Serial.begin(115200);                 // Start Serial communication at 115200 baud
  analogSetAttenuation(ADC_11db);       // Configure ADC range for 0–3.3V input
  delay(1000);                          // Small delay for stabilization
  Serial.println("ESP32 Soil Moisture Sensor"); // Print header text to Serial Monitor
  
}

void loop() {
  // Read raw analog value from the soil moisture sensor (range: 0–4095 for 12-bit ADC)
  int rawValue = analogRead(moisturePin);
  // Map the raw sensor value into percentage (0% dry → 100% wet)
  // Since higher ADC value means drier soil, we invert the range using dryValue→0 and wetValue→100
  int moisturePercent = map(rawValue, dryValue, wetValue, 0, 100);
  
  // Keep percentage within valid range (0–100%)
  moisturePercent = constrain(moisturePercent, 0, 100);
  
  // Decide soil status based on percentage
  String status;
  if (moisturePercent > 60) {
    status = "WET";       // Above 60% is considered wet
  } else if (moisturePercent > 30) {
    status = "MOIST";     // Between 30–60% is moderately moist
  } else {
    status = "DRY";       // Below 30% is considered dry
  }
  
  // Print sensor readings to Serial Monitor
  Serial.print("Raw: ");
  Serial.print(rawValue);
  Serial.print(" | Percentage: ");
  Serial.print(moisturePercent);
  Serial.print("% | Status: ");
  Serial.println(status);
  
  delay(1000); // Wait for 1 seconds before the next reading
}
