void setup()
{
  pinMode (36, INPUT);
  Serial.begin(115200);
  pinMode (36, INPUT); // LDR is on Pin 39
  Serial.begin(115200); // init Serial at 115200 baud
}
void loop()
{
  int potReading = analogRead(36);
  Serial.println(potReading);
  delay(500);

} // repeat