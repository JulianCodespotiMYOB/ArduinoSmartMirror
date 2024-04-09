// Arduino script for smart mirror project
// Include the ArduinoJson library
#include <ArduinoJson.h>

// Define pins for sensors and actuators
const int tempSensorPin = A0;
const int motionSensorPin = 2;
const int ledPin = 3;
const int fanPin = 4;

// Variables to store sensor data and actuator states
float temperature = 0.0;
bool motionDetected = false;
bool ledState = false;
bool fanState = false;

// Variables to store conditional rules
int updateInterval = 1000; // Default update interval
float temperatureThreshold = 25.0; // Default temperature threshold
bool motionEnabled = true; // Default motion detection enabled

void setup() {
  // Initialize serial communication
  Serial.begin(9600);

  // Set pin modes for sensors and actuators
  pinMode(motionSensorPin, INPUT);
  pinMode(ledPin, OUTPUT);
  pinMode(fanPin, OUTPUT);
}

void loop() {
  // Read mock data from sensors
  temperature = getTemperatureData();
  motionDetected = getMotionMockData();

  // Create a JSON object to store sensor data
  StaticJsonDocument<200> sensorDoc;
  sensorDoc["temperature"] = temperature;
  sensorDoc["motion"] = motionDetected;

  // Send sensor data as JSON object over serial
  serializeJson(sensorDoc, Serial);
  Serial.println();

  // Check if there are any incoming commands from Raspberry Pi
  if (Serial.available()) {
    String json = Serial.readString();
    processCommand(json);
  }

  // Control actuators based on sensor data and conditions
  controlLed();
  controlFan();

  // Delay for the specified update interval before the next iteration
  delay(updateInterval);
}

// Function to get mock temperature data
float getTemperatureMockData() {
  // Generate random temperature value between 20 and 30 degrees Celsius
  return random(20, 31);
}

float getTemperatureData() {
  return analogRead(tempSensorPin);
}

// Function to get mock motion data
bool getMotionMockData() {
  // Randomly return true or false for motion detection
  return random(0, 2) == 1;
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

  if (doc.containsKey("m")) {
    motionEnabled = doc["m"];
  }
}

// Function to control LED based on conditions
void controlLed() {
  if (ledState || (motionEnabled && motionDetected)) {
    digitalWrite(ledPin, HIGH);
  } else {
    digitalWrite(ledPin, LOW);
  }
}

// Function to control fan based on conditions
void controlFan() {
  if (fanState || temperature > temperatureThreshold) {
    digitalWrite(fanPin, HIGH);
  } else {
    digitalWrite(fanPin, LOW);
  }
}