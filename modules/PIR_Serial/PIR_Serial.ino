// Define the digital pin where the PIR sensor output is connected
const int PIR_SENSOR_OUTPUT_PIN = 13;  /* PIR sensor O/P pin */

// Variable to keep track of warm-up status (flag)
int warm_up;

void setup() {
  // Configure the PIR sensor pin as input
  pinMode(PIR_SENSOR_OUTPUT_PIN, INPUT);

  // Start serial communication at 115200 baud rate
  Serial.begin(115200);  

  // Notify that sensor is warming up
  Serial.println("Waiting For Power On Warm Up");

  // Wait 20 seconds to let the PIR sensor stabilize (PIR sensors need a warm-up time for accuracy)
  delay(20000);  

  // Notify that PIR sensor is ready after warm-up
  Serial.println("Ready!");
}

void loop() {
  int sensor_output;  // Variable to store sensor reading

  // Read the current output state from the PIR sensor (HIGH = motion, LOW = no motion)
  sensor_output = digitalRead(PIR_SENSOR_OUTPUT_PIN);

  // Case 1: No motion detected (sensor output is LOW)
  if (sensor_output == LOW)  
  {
    // If warm-up flag is set, print a message once during transition to "no motion"
    if (warm_up == 1)
    {
      Serial.print("Warming Up\n\n");  // Special case message
      warm_up = 0;   // Reset flag
      delay(2000);   // Short delay after transition
    }
    Serial.print("No object in sight\n\n");  // Print when no motion detected
    delay(1000);  // Small delay before next check
  }

  // Case 2: Motion detected (sensor output is HIGH)
  else  
  {
    Serial.print("Object detected\n\n");  // Print motion detection message
    warm_up = 1;  // Set flag for transition handling
    delay(1000);  // Small delay before next check
  } 
}
