// #include<Arduino.h>
#include <WiFi.h>
#include "probability.h"
// #include "esp_wifi.h"

#define CONNECTION_TIMEOUT 20

#define MAX_DISTANCES 8
#define MAX_MEASUREMENTS_PER_DISTANCE 333
#define BUTTON_PIN 0 // GPIO pin for the button

struct DistanceMeasurements {
  float distance;
  int rssi[MAX_MEASUREMENTS_PER_DISTANCE];
  int count;
};


DistanceMeasurements distanceMeasurements[MAX_DISTANCES];
int distanceCount = 0;

float currentDistance = -1; 
bool saveMeasurements = false; 
bool isCalibrating = false;
int calibrationStep = 0;

// Replace with your network credentials (STATION)
const char *ssid = "Galaxy A13";
const char *password = "";

void initWiFi()
{
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  Serial.print("Connecting to WiFi ..");
  Serial.print(ssid);
  
  int timeout_counter = 0;
  while(WiFi.status() != WL_CONNECTED){
        Serial.print(".");
        delay(200);
        timeout_counter++;
        if(timeout_counter >= CONNECTION_TIMEOUT*5){
        ESP.restart();
        }
    }
  Serial.println(WiFi.localIP());
}

void performCalibrationStep() {
  Serial.println("Enter 'd X' to start calibration at distance X feet (replace X with the actual distance).");
}

void handleCalibration(String input) {
  if (input.startsWith("d ")) {
    float calibrationDistance = input.substring(2).toFloat();
    if (calibrationDistance > 0) {
      currentDistance = calibrationDistance;
      saveMeasurements = true;
      Serial.println("Measuring RSSI at distance " + String(calibrationDistance) + " feet.");
      for(int i = 0; i < MAX_MEASUREMENTS_PER_DISTANCE; i++) {
        distanceMeasurements[calibrationStep].rssi[i] = WiFi.RSSI();
        distanceMeasurements[calibrationStep].count++;
        distanceMeasurements[calibrationStep].distance = currentDistance;
        delay(10); 
      }
      calibrationStep++;
      distanceCount++;
      if (calibrationStep >= 4) {
        endCalibration();
      } else {
        performCalibrationStep();
      }
    } else {
      Serial.println("Invalid distance. Please enter a positive number.");
    }
  }
}

void printStatsForDistance(float distance) {
  for (int i = 0; i < distanceCount; i++) {
    if (distanceMeasurements[i].distance == distance) {
      if (distanceMeasurements[i].count > 0) {
        float mean = calculateMean(distanceMeasurements[i].rssi, distanceMeasurements[i].count);
        float variance = calculateVariance(distanceMeasurements[i].rssi, distanceMeasurements[i].count, mean);
        Serial.print("Distance: ");
        Serial.print(distance);
        Serial.print(" - Mean: ");
        Serial.print(mean);
        Serial.print(", Variance: ");
        Serial.println(variance);
      }
      return; // Exit after printing stats for the specified distance
    }
  }
  Serial.println("Distance not found.");
}

void printAllStats() {
  for (int i = 0; i < distanceCount; i++) {
    printStatsForDistance(distanceMeasurements[i].distance);
  }
}

void addMeasurement(int rssi, float distance) {
  for (int i = 0; i < distanceCount; ++i) {
    if (distanceMeasurements[i].distance == distance) {
      if (distanceMeasurements[i].count < MAX_MEASUREMENTS_PER_DISTANCE) {
        distanceMeasurements[i].rssi[distanceMeasurements[i].count] = rssi;
        distanceMeasurements[i].count++;
        Serial.print(rssi);
        Serial.println(distanceMeasurements[i].count);
        return;
      } else {
        Serial.println("Max measurements for this distance reached.");
        return;
      }
    }
  }

  if (distanceCount < MAX_DISTANCES) {
    distanceMeasurements[distanceCount].distance = distance;
    distanceMeasurements[distanceCount].rssi[0] = rssi;
    distanceMeasurements[distanceCount].count = 1;
    distanceCount++;
  } else {
    Serial.println("Max different distances reached.");
  }
}


float calculatePathLossExponent() {
  if (distanceCount < 2) {
    Serial.println("Not enough data to calculate path loss exponent.");
    return 2.0; // Default value
  }

  // Use the first distance measurement as reference
  float refDistance = distanceMeasurements[0].distance;
  float refRSSI = calculateMean(distanceMeasurements[0].rssi, distanceMeasurements[0].count);

  float totalExponent = 0.0;
  int validExponentCount = 0;

  // Loop through each distance measurement starting from the second 
  for (int i = 1; i < distanceCount; i++) {
    float d = distanceMeasurements[i].distance;
    float meanRSSI = calculateMean(distanceMeasurements[i].rssi, distanceMeasurements[i].count);

    // Calculate path loss exponent for this distance based on the second point
    float exponent = (refRSSI - meanRSSI) / (10 * log10(d / refDistance));
    if (!isnan(exponent) && !isinf(exponent)) {
      totalExponent += exponent;
      validExponentCount++;
    }
  }
  return validExponentCount > 0 ? totalExponent / validExponentCount : 2.0; // Default to 2.0 if no valid data
}

void startCalibration() {
  isCalibrating = true;
  calibrationStep = 0;
  Serial.println("Calibration started. Please take 4 measurements at different distances.");
  performCalibrationStep();
}

void endCalibration() {
  isCalibrating = false;
  Serial.println("Calibration ended. Calculating path loss exponent...");
  calculatePathLossExponent();
  Serial.println("Calibration complete. 'Predict Distance [RSSI]' or 'Predict RSSI [Distance]'");
}

float predictRSSI(float distance, float rssiAtD0, float pathLossExponent) {
  return rssiAtD0 - 10 * pathLossExponent * log10(distance);
}

void predictAndPrintRSSI(float distance) {
  float meanRSSIAtRef = calculateMean(distanceMeasurements[0].rssi, distanceMeasurements[0].count); // Assuming the first entry is the reference distance
  float pathLossExponent = calculatePathLossExponent(); 
  float predictedRSSI = predictRSSI(distance, meanRSSIAtRef, pathLossExponent);

  Serial.print("Predicted RSSI at ");
  Serial.print(distance);
  Serial.print("m: ");
  Serial.println(predictedRSSI);
}


void predictAndPrintDistance(int rssi) {
  float meanRSSIAtRef = calculateMean(distanceMeasurements[0].rssi, distanceMeasurements[0].count);
  float pathLossExponent = calculatePathLossExponent();
  float predictedDistance = predictDistance(rssi, meanRSSIAtRef, pathLossExponent, distanceMeasurements[0].distance);
  Serial.print("Mean at Reference ");
  Serial.println(meanRSSIAtRef);
  Serial.print("Predicted distance for RSSI ");
  Serial.print(rssi);
  Serial.print(": ");
  Serial.println(predictedDistance);
}

float predictDistance(int rssi, float rssiAtD0, float pathLossExponent, float refDistance) {
  Serial.print("Path Loss Exponent: ");
  Serial.println(pathLossExponent);
  return refDistance * pow(10, (rssiAtD0 - rssi) / (10 * pathLossExponent));

}


void serialCommand(String input) {
  if (input.equalsIgnoreCase("Wait")) {
    saveMeasurements = false; 
  } else if (input.startsWith("d ")) {
    handleCalibration(input);
  } else if (input.startsWith("Predict RSSI ")) {
    float predictDist = input.substring(13).toFloat();
    predictAndPrintRSSI(predictDist);
  } else if (input.startsWith("Predict Distance ")) {
    int rssi = input.substring(17).toInt();
    predictAndPrintDistance(rssi);
  }
}

void setup()
{
  Serial.begin(115200);
  initWiFi();
  pinMode(BUTTON_PIN, INPUT_PULLUP); 
  Serial.print("RSSI: ");
  Serial.println(WiFi.RSSI());
  Serial.println("Setup Complete");

  // Initiate calibration at startup
  startCalibration();
}

void loop() {
  static unsigned long lastRSSIPrintTime = 0;
  const unsigned long RSSIPrintInterval = 200; // Print RSSI every 1 second

  if (millis() - lastRSSIPrintTime > RSSIPrintInterval) {
    int rssi = WiFi.RSSI();
    Serial.println(rssi);
    lastRSSIPrintTime = millis();
    if (calibrationStep > 3) {predictAndPrintDistance(rssi);}
  }

  if (Serial.available() > 0) {
    String input = Serial.readStringUntil('\n');
    input.trim();
    serialCommand(input);
  }

  if (digitalRead(BUTTON_PIN) == LOW) {
    printAllStats();
  }
}