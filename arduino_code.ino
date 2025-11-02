#include <LiquidCrystal_I2C.h>  // Library for I2C-based LCD display
#include <ArduinoJson.h>        // Library for handling JSON data
#include <SoftwareSerial.h>     // Library for creating a software serial connection

// Initialize LCD display with I2C address 0x27 and 16x2 display size
LiquidCrystal_I2C lcd(0x27, 16, 2);

// Initialize software serial for communication with ESP module (Tx=2, Rx=3)
SoftwareSerial espSerial(2, 3);

// Pin Definitions for various sensors and output components
#define FIRE_SENSOR_PIN 8
#define LDR_SENSOR_PIN 9
#define LED_PIN 6
#define FAN_RELAY_PIN 5
#define BUZZER_PIN 4
#define TEMP_SENSOR_PIN A1
#define GAS_SENSOR_PIN A0

// Variables to store the current status of the fan and LED
bool fanStatus = false;
bool ledStatus = false;

void setup() {
  // Set sensor pins as input and output pins for actuators
  pinMode(FIRE_SENSOR_PIN, INPUT);
  pinMode(LDR_SENSOR_PIN, INPUT);
  pinMode(LED_PIN, OUTPUT);
  pinMode(FAN_RELAY_PIN, OUTPUT);
  pinMode(BUZZER_PIN, OUTPUT);

  // Initialize serial communication for debugging and ESP module
  Serial.begin(9600);
  espSerial.begin(9600);

  // Initialize the LCD display
  lcd.begin(16, 2);
  lcd.backlight();  // Turn on LCD backlight

  // Display startup message on LCD
  lcd.setCursor(0, 0);
  lcd.print("System Starting...");
  delay(1000);
  lcd.clear();

  // set fan is initially turned off
  digitalWrite(FAN_RELAY_PIN, HIGH);
}

void loop() {
  // Read sensor values
  int tempValue = analogRead(TEMP_SENSOR_PIN);
  int gasValue = analogRead(GAS_SENSOR_PIN);
  int fireStatus = digitalRead(FIRE_SENSOR_PIN);
  int ldrStatus = digitalRead(LDR_SENSOR_PIN);

  // Convert sensor values into meaningful units
  bool fire_detected = fireStatus == LOW ? true : false;
  float temperature = (tempValue * 5.0 / 1024.0) * 100;  // Convert temperature value
  float gas_level = (gasValue / 1024.0) * 100;           // Convert gas sensor value to percentage

  // Control fan, buzzer, and LED based on sensor values
  controlActuators(fireStatus, ldrStatus);

  // Display the sensor readings and actuator status on the LCD screen
  displayData(temperature, gas_level, ldrStatus, fireStatus, fanStatus, ledStatus);

  // Send sensor data to ESP8266 module for further processing
  sendToESP(temperature, gas_level, ldrStatus, fire_detected, fanStatus, ledStatus);

  delay(5000);
}

// Function to control fan, buzzer, and LED based on sensor inputs
void controlActuators(bool fireStatus, bool ldrStatus) {
  if (fireStatus == LOW) { // If fire is detected
    digitalWrite(FAN_RELAY_PIN, LOW);  // Turn fan ON
    fanStatus = true;
    digitalWrite(BUZZER_PIN, HIGH);    // Turn buzzer ON
  } else {
    digitalWrite(FAN_RELAY_PIN, HIGH); // Turn fan OFF
    fanStatus = false;
    digitalWrite(BUZZER_PIN, LOW);     // Turn buzzer OFF
  }

  if (ldrStatus == HIGH) {  // If light it's dark
    digitalWrite(LED_PIN, HIGH); // Turn LED ON
    ledStatus = true;
  } else { // If light it's bright
    digitalWrite(LED_PIN, LOW);  // Turn LED OFF
    ledStatus = false;
  }
}

// Function to display sensor readings and actuator status on LCD screen
void displayData(float temperature, float gas_level, bool ldrStatus, bool fireStatus, bool fanStatus, bool ledStatus) {
  lcd.clear(); // Clear LCD screen

  // Display temperature and gas level on first row
  lcd.setCursor(0, 0);
  lcd.print("T: ");
  lcd.print(temperature, 1);
  lcd.print("C");

  lcd.setCursor(9, 0);
  lcd.print("G: ");
  lcd.print(gas_level, 1);
  lcd.print("%");

  // Display fire and light status on the second row
  lcd.setCursor(0, 1);
  lcd.print("F: ");
  lcd.print(fireStatus == LOW ? "Yes" : "No"); // Fire detected if LOW

  lcd.setCursor(9, 1);
  lcd.print("L: ");
  lcd.print(ldrStatus == HIGH ? "Yes" : "No"); // Light detected if HIGH

  delay(2000);

  // Display fan and LED status on the LCD screen
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Fan: ");
  lcd.print(fanStatus ? "On " : "Off");

  lcd.setCursor(9, 0);
  lcd.print("LED: ");
  lcd.print(ledStatus ? "On" : "Off");
}

// Function to send sensor readings and actuator status as JSON data to ESP8266 module
void sendToESP(float temperature, float gas_level, bool ldrStatus, bool fireStatus, bool fanStatus, bool ledStatus) {
  StaticJsonDocument<200> doc; // Create a JSON object

  // Store sensor readings and actuator status in JSON format
  doc["temperature"] = temperature;
  doc["gas_level"] = gas_level;
  doc["light_intensity"] = ldrStatus;
  doc["fire_detected"] = fireStatus;
  doc["fan_status"] = fanStatus;
  doc["led_status"] = ledStatus;

  // Convert JSON object to string
  String jsonString;
  serializeJson(doc, jsonString);

  // Send the JSON string to ESP8266
  espSerial.println(jsonString);
}
