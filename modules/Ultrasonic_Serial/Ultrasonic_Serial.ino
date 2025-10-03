// Define ultrasonic sensor pins
#define TRIG_PIN 5    // GPIO pin used for Trigger
#define ECHO_PIN 18   // GPIO pin used for Echo

long duration;        // Variable to store echo pulse duration (µs)
float distanceCm;     // Variable to store calculated distance

void setup() {
  Serial.begin(115200);      // Start serial communication at 115200 baud
  pinMode(TRIG_PIN, OUTPUT); // Set trigger pin as output
  pinMode(ECHO_PIN, INPUT);  // Set echo pin as input
}

void loop() {
  // Step 1: Clear the trigger pin
  digitalWrite(TRIG_PIN, LOW);
  delayMicroseconds(2);

  // Step 2: Send a 10 µs HIGH pulse on trigger pin
  digitalWrite(TRIG_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG_PIN, LOW);

  // Step 3: Measure time taken for echo to return
  // pulseIn() waits for the echo pin to go HIGH and then LOW
  duration = pulseIn(ECHO_PIN, HIGH);

  // Step 4: Convert duration to distance (cm)
  // Formula: distance = (time * speed of sound) / 2
  // Speed of sound = 343 m/s = 0.0343 cm/µs
  distanceCm = (duration * 0.0343) / 2;

  // Step 5: Print distance to Serial Monitor
  Serial.print("Distance: ");
  Serial.print(distanceCm);
  Serial.println(" cm");

  // Small delay before next measurement
  delay(500);
}
