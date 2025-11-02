#include <ESP8266WiFi.h>       // Library to connect ESP8266 to WiFi
#include <WiFiClientSecure.h>  // Library for secure HTTPS communication
#include <ArduinoJson.h>       // Library for handling JSON data

// WiFi details
const char* ssid = "ADITYA3";  
const char* password = "ADITYA1306";    

const char* server = "institute-protection-system-backend.onrender.com";  // Backend server URL
const int serverPort = 443;  // HTTPS port

// Secure client to handle HTTPS connection
WiFiClientSecure client;

void setup() {
  Serial.begin(9600);   // Start Serial communication for debugging
  Serial.setTimeout(2000);  // Set timeout for Serial communication

  // Connect to WiFi
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");  // Print dots while connecting
  }
  Serial.println("\nConnected to WiFi");  // Print success message

  client.setInsecure();  // Disable SSL certificate verification
}

// Function to check and reconnect WiFi if disconnected
void checkWiFi() {
  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("Reconnecting to WiFi...");
    WiFi.disconnect();  // Disconnect from WiFi
    WiFi.begin(ssid, password);  // Reconnect
    while (WiFi.status() != WL_CONNECTED) {
      delay(500);
      Serial.print(".");  // Print dots while reconnecting
    }
    Serial.println("\nReconnected to WiFi");
  }
}

// Function to send JSON data to the server
void sendDataToServer(const String& jsonString) {
  checkWiFi();  // Ensure WiFi is connected before sending data

  if (client.connect(server, serverPort)) {  // Attempt to connect to the server
    Serial.println("Connected to Server");

    // Send HTTP POST request
    client.print("POST /upload_data HTTP/1.1\r\n");  // Specify the endpoint
    client.print("Host: " + String(server) + "\r\n");  // Send host information
    client.print("Content-Type: application/json\r\n");  // Specify data type
    client.print("Content-Length: " + String(jsonString.length()) + "\r\n");  // Send data length
    client.print("\r\n");  // End of headers
    client.print(jsonString);  // Send JSON data

    delay(1000);  // Wait for server response
    while (client.available()) {
      String response = client.readString();  // Read server response
      Serial.println("Server Response: " + response);
    }

    client.stop();  // Close the connection
  } else {
    Serial.println("Connection to server failed!");  // Print error message if connection fails
  }
}

void loop() {
  if (Serial.available()) {  // Check if data is available from Arduino
    String jsonString = Serial.readStringUntil('\n');  // Read incoming JSON string
    Serial.println("Received from Arduino: " + jsonString);  // Print received data
    sendDataToServer(jsonString);  // Send data to the server
  }
  delay(1000);
}
