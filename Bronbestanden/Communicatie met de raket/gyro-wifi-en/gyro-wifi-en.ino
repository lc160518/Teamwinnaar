#include <Wire.h>
#include <WiFi.h>
#include <ESPAsyncWebServer.h>
#include "MPU6050.h"

// Replace with your network credentials
const char* ssid = "your_SSID";
const char* password = "your_PASSWORD";

// Create an instance of the MPU6050 sensor
MPU6050 mpu;

// Create an instance of the server
AsyncWebServer server(80);

void setup() {
  Serial.begin(115200);

  // Initialize MPU6050
  while(!mpu.begin(MPU6050_SCALE_2000DPS, MPU6050_RANGE_2G))
  {
    Serial.println("Could not find a valid MPU6050 sensor, check wiring!");
    delay(500);
  }

  // Connect to Wi-Fi
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }
  Serial.println(WiFi.localIP());

  // Route for handling the client request
  server.on("/readings", HTTP_GET, [](AsyncWebServerRequest *request){
    String readings;
    Vector rawGyro = mpu.readRawGyro();
    Vector normGyro = mpu.readNormalizeGyro();
    readings = "Raw:" + String(rawGyro.XAxis) + "," + String(rawGyro.YAxis) + "," + String(rawGyro.ZAxis) + "\n";
    readings += "Norm:" + String(normGyro.XAxis) + "," + String(normGyro.YAxis) + "," + String(normGyro.ZAxis);
    request->send(200, "text/plain", readings);
  });

  // Start the server
  server.begin();
}

void loop() {
  // put your main code here, to run repeatedly:
}