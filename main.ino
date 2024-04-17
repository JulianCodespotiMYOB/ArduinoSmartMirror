// Arduino script for smart mirror project
// Include the ArduinoJson library
#include <ArduinoJson.h>

// Define pins for sensors and actuators
const int tempSensorPin = A0;
const int lightSensorPin = A1;
const int piezoPin = 3;
const int motorPin = 4;

// Variables to store sensor data and actuator states
float temperature = 0.0;
int lightLevel = 0;
bool motorState = false;

// Variables to store conditional rules
int updateInterval = 1000;          // Default update interval
float temperatureThreshold = 25.0;  // Default temperature threshold

void setup() {
  // Initialize serial communication
  Serial.begin(9600);

  // Set pin modes for sensors and actuators
  pinMode(piezoPin, OUTPUT);
  pinMode(motorPin, OUTPUT);
}

void loop() {
  // Read data from sensors
  temperature = getTemperatureData();
  lightLevel = getLightData();

  // Create a JSON object to store sensor data
  StaticJsonDocument<200> sensorDoc;
  sensorDoc["temperature"] = temperature;
  sensorDoc["lightLevel"] = lightLevel;

  // Send sensor data as JSON object over serial
  serializeJson(sensorDoc, Serial);
  Serial.println();

  // Check if there are any incoming commands from Raspberry Pi
  if (Serial.available()) {
    String json = Serial.readString();
    processCommand(json);
  }

  // Control actuators based on temperature condition
  controlMotor();
  controlPiezo();

  // Delay for the specified update interval before the next iteration
  delay(updateInterval);
}

// Function to get temperature data
float getTemperatureData() {
  float temperatureF = analogRead(tempSensorPin);
  float temperatureC = (temperatureF - 32.0) * 5.0 / 9.0;
  return temperatureC;
}

// Function to get light data
int getLightData() {
  return analogRead(lightSensorPin);
}

// Function to process incoming commands from Raspberry Pi
void processCommand(String json) {
  // Deserialize the JSON payload
  StaticJsonDocument<200> doc;
  DeserializationError error = deserializeJson(doc, json);

  if (error) {
    // If deserialization fails, print an error message
    Serial.print("Deserialization failed: ");
    Serial.println(error.c_str());
    return;
  }

  // Update conditional rules based on the received JSON payload
  if (doc.containsKey("u")) {
    updateInterval = doc["u"];
  }
  if (doc.containsKey("t")) {
    temperatureThreshold = doc["t"];
  }
}

// Function to control motor based on temperature condition
void controlMotor() {
  if (temperature > temperatureThreshold) {
    digitalWrite(motorPin, HIGH);
    motorState = true;
  } else {
    digitalWrite(motorPin, LOW);
    motorState = false;
  }
}

// Function to control piezo based on temperature condition
void controlPiezo() {
  if (temperature > temperatureThreshold + 10) {
    tone(piezoPin, 1000, 500);  // Play a tone of 1000Hz for 500ms
  } else {
    noTone(piezoPin);  // Stop playing the tone
  }
}