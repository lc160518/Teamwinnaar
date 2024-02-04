/*
 * This ESP32 code is created by esp32io.com
 *
 * This ESP32 code is released in the public domain
 *
 * For more detail (instruction and wiring diagram), visit https://esp32io.com/tutorials/esp32-servo-motor
 */
#include <Arduino.h>
#include <ESP32Servo.h>


#define SERVO_PIN 16 // ESP32 pin GPIO26 connected to servo motor

Servo myServo;

void setup() {
  myServo.attach(SERVO_PIN);  // attaches the servo on ESP32 pin
}

void loop() 
{
  myServo.write(0);    // Move the servo to 0 degrees
  delay(1000);         // Wait for 1 second
  myServo.write(90);   // Move the servo to 90 degrees
  delay(1000);         // Wait for 1 second
  myServo.write(180);  // Move the servo to 180 degrees
  delay(1000);         // Wait for 1 second
}
