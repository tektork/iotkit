// Import required libraries
#include <WiFi.h>                  // WiFi support for ESP32
#include <AsyncTCP.h>              // Async TCP library required for AsyncWebServer
#include <ESPAsyncWebServer.h>     // Asynchronous Web server library

// Replace with your network credentials
const char* ssid = "Your Password";      // Wi-Fi SSID
const char* password = "Password"; // Wi-Fi Password

bool ledState = 0;                 // Variable to store the LED state (0 = OFF, 1 = ON)
const int ledPin = 4;              // Define GPIO pin 4 for LED

// Create AsyncWebServer object on port 80
AsyncWebServer server(80);

// Create WebSocket object, accessible at endpoint "/ws"
AsyncWebSocket ws("/ws");

// HTML/JS code stored in program memory for serving webpage
const char index_html[] PROGMEM = R"rawliteral(
<!DOCTYPE HTML><html>
<head>
  <title>ESP Web Server</title>
  <meta name="viewport" content="width=device-width, initial-scale=1">
  <link rel="icon" href="data:,">
  <style>
    /* Page styling */
    html {
      font-family: Arial, Helvetica, sans-serif;
      text-align: center;
      background-color: #e6f2ff; /* Light blue background */
    }
    h1 {
      font-size: 1.8rem;
      color: white;
    }
    h2{
      font-size: 1.5rem;
      font-weight: bold;
      color: #143642;
    }
    .topnav {
      overflow: hidden;
      background-color: #143642;
    }
    body {
      margin: 0;
      background-color: #e6f2ff;
    }
    .content {
      padding: 30px;
      max-width: 600px;
      margin: 0 auto;
    }
    .card {
      background-color: #F8F7F9;
      box-shadow: 2px 2px 12px 1px rgba(140,140,140,.5);
      padding-top:10px;
      padding-bottom:20px;
    }
    .button {
      padding: 15px 50px;
      font-size: 24px;
      text-align: center;
      outline: none;
      color: #fff;
      background-color: #0f8b8d;
      border: none;
      border-radius: 5px;
      user-select: none;
    }
    .button:active {
      background-color: #0f8b8d;
      transform: translateY(2px);
    }
    .state {
      font-size: 1.5rem;
      color:#8c8c8c;
      font-weight: bold;
    }
  </style>
</head>
<body>
  <!-- Web page header -->
  <div class="topnav">
    <h1>ESP WebSocket Server</h1>
  </div>
  <div class="content">
    <div class="card">
      <h2>Output - GPIO 4</h2>
      <!-- State of LED displayed here -->
      <p class="state">state: <span id="state">%STATE%</span></p>
      <!-- Toggle button -->
      <p><button id="button" class="button">Toggle</button></p>
    </div>
  </div>

  <script>
    // WebSocket communication endpoint
    var gateway = `ws://${window.location.hostname}/ws`;
    var websocket;

    // Run when page is loaded
    window.addEventListener('load', onLoad);

    // Initialize WebSocket connection
    function initWebSocket() {
      console.log('Trying to open a WebSocket connection...');
      websocket = new WebSocket(gateway);
      websocket.onopen    = onOpen;
      websocket.onclose   = onClose;
      websocket.onmessage = onMessage; // Receive messages
    }

    // When connection opens
    function onOpen(event) {
      console.log('Connection opened');
    }

    // When connection closes, retry in 2 sec
    function onClose(event) {
      console.log('Connection closed');
      setTimeout(initWebSocket, 2000);
    }

    // Handle incoming WebSocket messages from ESP32
    function onMessage(event) {
      var state;
      if (event.data == "1"){
        state = "ON";
      }
      else{
        state = "OFF";
      }
      document.getElementById('state').innerHTML = state;
    }

    // On load, initialize websocket + button
    function onLoad(event) {
      initWebSocket();
      initButton();
    }

    // Button click triggers toggle message
    function initButton() {
      document.getElementById('button').addEventListener('click', toggle);
    }

    // Send "toggle" command over WebSocket to ESP32
    function toggle(){
      websocket.send('toggle');
    }
  </script>
</body>
</html>
)rawliteral";

// Send LED state to all connected WebSocket clients
void notifyClients() {
  ws.textAll(String(ledState));
}

// Handle received WebSocket messages
void handleWebSocketMessage(void *arg, uint8_t *data, size_t len) {
  AwsFrameInfo *info = (AwsFrameInfo*)arg;
  // Verify message integrity
  if (info->final && info->index == 0 && info->len == len && info->opcode == WS_TEXT) {
    data[len] = 0;   // Null terminate the string
    // If toggle command received, flip LED state
    if (strcmp((char*)data, "toggle") == 0) {
      ledState = !ledState;
      notifyClients();  // Notify all clients of the new state
    }
  }
}

// Event handler for WebSocket events
void onEvent(AsyncWebSocket *server, AsyncWebSocketClient *client,
             AwsEventType type, void *arg, uint8_t *data, size_t len) {
  switch (type) {
    case WS_EVT_CONNECT:
      Serial.printf("WebSocket client #%u connected from %s\n",
                    client->id(), client->remoteIP().toString().c_str());
      break;
    case WS_EVT_DISCONNECT:
      Serial.printf("WebSocket client #%u disconnected\n", client->id());
      break;
    case WS_EVT_DATA:
      handleWebSocketMessage(arg, data, len);
      break;
    case WS_EVT_PONG:
    case WS_EVT_ERROR:
      break;
  }
}

// Initialize WebSocket server
void initWebSocket() {
  ws.onEvent(onEvent);
  server.addHandler(&ws);
}

// Replace HTML placeholder with actual state value
String processor(const String& var){
  Serial.println(var);
  if(var == "STATE"){
    if (ledState){
      return "ON";
    }
    else{
      return "OFF";
    }
  }
  return String();
}

void setup(){
  // Serial port for debugging
  Serial.begin(115200);
  pinMode(ledPin, OUTPUT);
  digitalWrite(ledPin, LOW); // Turn off LED initially
  
  // Connect to Wi-Fi network
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi..");
  }

  // Print ESP Local IP Address
  Serial.println(WiFi.localIP());

  // Initialize WebSocket
  initWebSocket();

  // Define root route "/"
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
    // Send HTML page with processor function replacing placeholders
    request->send(200, "text/html", index_html, processor);
  });

  // Start HTTP server
  server.begin();
}

void loop() {
  // Periodically clean disconnected WebSocket clients
  ws.cleanupClients();

  // Set LED pin based on ledState value
  digitalWrite(ledPin, ledState);
}
