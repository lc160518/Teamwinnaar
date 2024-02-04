#include <Arduino.h>
#include <ESP32Servo.h>

Servo monservo;

int servopin = 17;

int deg = 0;


void setup() {
  Serial.begin(115200);
  ESP32PWM::allocateTimer(0); // allocate a timer first
  monservo.setPeriodHertz(50);
  monservo.attach(servopin, 500, 2400); // (pin, min pulse width, max pulse width in microseconds)
  monservo.write(deg); // set servo to mid-point
}

void loop() {
	
	if (deg != 180) {
		deg = 0;
	}
	else {
		deg = 0;
	}
	monservo.write(deg)
	
}