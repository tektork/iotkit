
// MQ-2 Gas Sensor with ESP32 - Analog Mode (Raw Value)
// AO pin -> ESP32 ADC pin (e.g., GPIO34)

#define AO_PIN 34  // Define ESP32 GPIO34 as the pin connected to MQ-2 AO (Analog Output)

void setup() {
  Serial.begin(115200);   // Start Serial Monitor at 9600 baud for debugging/monitoring

  // Configure ESP32 ADC range
  // ADC_11db attenuation allows measurement up to ~3.3V safely (full sensor voltage range)
  analogSetAttenuation(ADC_11db);

  // MQ-2 sensor requires a short preheating time for the internal heater element
  // This ensures more stable readings before measuring gas concentration
  delay(20000);  // Wait 20 seconds for sensor warm-up
}

void loop() {
  // 1) Read the raw analog value from MQ-2 sensor
  // ESP32 ADC gives values from 0 (0V) to 4095 (~3.3V with 12-bit resolution)
  int gasValue = analogRead(AO_PIN);

  // 2) Print raw sensor value to Serial Monitor
  Serial.print("MQ-2 Sensor AO Value: ");
  Serial.println(gasValue);

  // 3) Delay before the next reading to avoid flooding Serial Monitor
  delay(1000);  // Read AO value every 1 second
}


