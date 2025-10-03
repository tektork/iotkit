#include <Adafruit_BMP280.h>   // Include the Adafruit BMP280 sensor library

// Pin definitions if using SPI connection (not used for I2C by default)
#define BMP_SCK  (13)
#define BMP_MISO (12)
#define BMP_MOSI (11)
#define BMP_CS   (10)

// Create BMP280 object using I2C interface
Adafruit_BMP280 bmp; 

// Alternative constructors for SPI connection (commented out)
// Adafruit_BMP280 bmp(BMP_CS); // hardware SPI with defined CS
// Adafruit_BMP280 bmp(BMP_CS, BMP_MOSI, BMP_MISO, BMP_SCK); // software SPI

void setup() {
  Serial.begin(9600);                 // Start Serial Monitor at 9600 baud rate
  Serial.println(F("BMP280 Forced Mode Test."));

  // Initialize BMP280 with I2C address 0x76
  if (!bmp.begin(0x76)) {
    Serial.println(F("Could not find a valid BMP280 sensor, check wiring or "
                     "try a different address!"));
    while (1) delay(10);              // Stay here forever if sensor not found
  }

  /* Configure BMP280 sensor settings */
  bmp.setSampling(
    Adafruit_BMP280::MODE_FORCED,     // Forced mode (sensor measures only when commanded)
    Adafruit_BMP280::SAMPLING_X2,     // Temperature oversampling = x2
    Adafruit_BMP280::SAMPLING_X16,    // Pressure oversampling = x16
    Adafruit_BMP280::FILTER_X16,      // IIR filter strength = 16
    Adafruit_BMP280::STANDBY_MS_500   // Standby time (not much used in forced mode)
  );
}

void loop() {
  // In forced mode, we must trigger a measurement manually
  // takeForcedMeasurement() wakes sensor, blocks until data is ready
  if (bmp.takeForcedMeasurement()) {
    // After measurement, we can read sensor values

    Serial.print(F("Temperature = "));
    Serial.print(bmp.readTemperature());  // Read temperature (in Celsius)
    Serial.println(" *C");

    Serial.print(F("Pressure = "));
    Serial.print(bmp.readPressure());     // Read atmospheric pressure (in Pascals)
    Serial.println(" Pa");

    Serial.print(F("Approx altitude = "));
    Serial.print(
      bmp.readAltitude(1013.25)           // Estimate altitude using sea level pressure (hPa)
    );
    Serial.println(" m");

    Serial.println();
    
    delay(2000); // Wait 2 seconds before taking another forced measurement
  } else {
    Serial.println("Forced measurement failed!");  // Error if measurement did not complete
  }
}
