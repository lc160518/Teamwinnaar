/*
        Arduino Brushless Motor Control
     by Dejan, https://howtomechatronics.com
*/
#include <Arduino.h>
#include <Servo.h>

Servo ESC;     // create servo object to control the ESC

int potValue;  // value from the analog pin

void setup() {
  Serial.begin(9600);
  // Attach the ESC on pin 9
  ESC.attach(9);
  ESC.writeMicroseconds(1000); // Send the signal to the ESC
}

void loop() {

  
  potValue = analogRead(A0);   // reads the value of the potentiometer (value between 0 and 1023)
  potValue = map(potValue, 0, 1023, 1000, 2000);   // scale it to use it with the servo library (value between 0 and 180)
  
  Serial.println("Pot Value: " + String(potValue));
  ESC.writeMicroseconds(potValue);  // Send the signal to the ESC
}