#include <WiFi.h>   // ESP32 WiFi library

// WiFi credentials
const char* ssid = "Your SSID";
const char* password = "Your Password";

// Web server object on port 80
WiFiServer server(80);

// Potentiometer analog input pin (ADC1 channel at GPIO36)
const int potPin = 36;

// HTTP request header storage
String header;

// Timing variables for HTTP client timeout
unsigned long currentTime = millis();
unsigned long previousTime = 0;
const long timeoutTime = 2000;  // 2 seconds connection timeout

void setup() {
  Serial.begin(115200);
  pinMode(potPin, INPUT);   // Configure potentiometer pin

  // Connect to WiFi
  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);

  // Wait until connected
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  // Print IP address once connected
  Serial.println("\nWiFi connected.");
  Serial.println("IP Address: ");
  Serial.println(WiFi.localIP());

  // Start server
  server.begin();
}

void loop() {
  // Check for client connections
  WiFiClient client = server.available();  

  if (client) {
    currentTime = millis();
    previousTime = currentTime;
    Serial.println("New Client.");
    String currentLine = "";
    header = "";

    // While the client stays connected & not timed out
    while (client.connected() && currentTime - previousTime <= timeoutTime) {
      currentTime = millis();

      if (client.available()) {
        char c = client.read();   // Read each char
        header += c;              // Append to full header
        Serial.write(c);          // Print to Serial Monitor

        // If end of request line
        if (c == '\n') {
          if (currentLine.length() == 0) {

            // ==== Handle AJAX JSON request ====
            if (header.indexOf("GET /readPot") >= 0) {
              int potValue = analogRead(potPin);             // Raw ADC value (0–4095)
              float voltage = (potValue / 4095.0) * 3.3;     // Convert to voltage (Vref ~ 3.3 V)

              // Send JSON response with both value and voltage
              client.println("HTTP/1.1 200 OK");
              client.println("Content-Type: application/json");
              client.println("Connection: close");
              client.println();
              client.print("{\"value\":");
              client.print(potValue);
              client.print(", \"voltage\":");
              client.print(voltage, 2);   // Rounded to 2 decimals
              client.println("}");
              break;
            }

            // ==== Serve the main web page ====
            client.println("HTTP/1.1 200 OK");
            client.println("Content-type:text/html");
            client.println("Connection: close");
            client.println();

            // HTML Page
            client.println("<!DOCTYPE html><html>");
            client.println("<head><meta name=\"viewport\" content=\"width=device-width, initial-scale=1\">");

            // CSS styles
            client.println("<style>");
            client.println("body { font-family: Arial; text-align: center; padding: 20px; background-color: lightpink; }");
            client.println(".bar-container { width: 80%; height: 30px; background-color: #ddd; margin: 15px auto; border-radius: 10px; overflow: hidden; }");
            client.println(".bar { height: 100%; width: 0%; border-radius: 10px; }");
            client.println("#adcBar { background-color: #4CAF50; }");  // Green - ADC
            client.println("#voltBar { background-color: #2196F3; }"); // Blue - Voltage
            client.println("</style>");

            // JavaScript to update values continuously
            client.println("<script>");
            client.println("function updatePotValue() {");
            client.println("setInterval(function() {");
            client.println("var xhr = new XMLHttpRequest();");
            client.println("xhr.onreadystatechange = function() {");
            client.println("if (xhr.readyState == 4 && xhr.status == 200) {");
            client.println("var data = JSON.parse(xhr.responseText);");  
            client.println("document.getElementById('potValue').innerText = data.value;");  
            client.println("document.getElementById('voltageValue').innerText = data.voltage.toFixed(2) + ' V';");  
            client.println("var adcPercent = Math.round((data.value / 4095) * 100);");  
            client.println("var voltPercent = Math.round((data.voltage / 3.3) * 100);");  
            client.println("document.getElementById('adcBar').style.width = adcPercent + '%';");  
            client.println("document.getElementById('voltBar').style.width = voltPercent + '%';");  
            client.println("}");  
            client.println("};");  
            client.println("xhr.open('GET', '/readPot', true);");  
            client.println("xhr.send();");  
            client.println("}, 1000);");  // Every 1 second
            client.println("}");  
            client.println("</script>");
            client.println("</head>");

            // Body of the page
            client.println("<body onload=\"updatePotValue()\">");
            client.println("<h1>ESP32 Potentiometer Monitor</h1>");
            client.println("<p><strong>Raw ADC Value:</strong> <span id=\"potValue\">0</span></p>");
            client.println("<div class=\"bar-container\"><div class=\"bar\" id=\"adcBar\"></div></div>");
            client.println("<p><strong>Voltage:</strong> <span id=\"voltageValue\">0.00 V</span></p>");
            client.println("<div class=\"bar-container\"><div class=\"bar\" id=\"voltBar\"></div></div>");
            client.println("</body></html>");
            
            client.println(); // End of page
            break;
          } 
          else {
            currentLine = "";  // reset line buffer
          }
        } 
        else if (c != '\r') {
          currentLine += c;   // Build current line
        }
      }
    }

    // Close client connection
    delay(1);
    client.stop();
    Serial.println("Client disconnected.");
  }
}
