#include "DHT.h"             // Include the DHT sensor library

#define DHTPIN 13         // GPIO pin where DHT11 data pin is connected
#define DHTTYPE DHT11        // Define the sensor type (here: DHT11)

// Create a DHT object, passing the pin number and sensor type
DHT dht(DHTPIN, DHTTYPE);

void setup() {
  Serial.begin(115200);      // Initialize serial communication at 115200 baud
  dht.begin();               // Initialize the DHT11 sensor
}

void loop() {
  // Reading humidity or temperature takes ~250ms (sensor is relatively slow)

  // Read relative humidity (%)
  float humidity = dht.readHumidity();

  // Read temperature as Celsius (isFahrenheit = false)
  float temperatureC = dht.readTemperature();

  // Read temperature as Fahrenheit (isFahrenheit = true)
  float temperatureF = dht.readTemperature(true);

  // Check if sensor reading failed (NaN = Not a Number)
  // Sometimes DHT sensors fail to return data due to timing issues
  if (isnan(humidity) || isnan(temperatureC)) {
    Serial.println("Failed to read from DHT sensor!");
    return;   // Skip the rest of the loop and try again next cycle
  }

  // Print the humidity and temperature values
  Serial.print("Humidity: ");
  Serial.print(humidity);
  Serial.print(" %\t");    // Print % symbol with a tab for spacing

  Serial.print("Temperature: ");
  Serial.print(temperatureC);
  Serial.print(" °C / ");
  Serial.print(temperatureF);
  Serial.println(" °F");

  // The DHT11 sensor requires at least a 1-second delay between readings
  delay(1000);
}
