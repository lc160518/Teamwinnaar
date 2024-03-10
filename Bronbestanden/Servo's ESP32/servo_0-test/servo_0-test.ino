#include <ESP32Servo.h>

Servo servo_x1, servo_x2, servo_z1, servo_z2;

// put your setup code here, to run once:
const int servo_x1_pin = 19;
const int servo_x2_pin = 5;
const int servo_z1_pin = 17;
const int servo_z2_pin = 23;
const int servo_0 = 90;

void setup() {
  servo_x1.setPeriodHertz(50);    // standard 50 hz servo
	servo_x1.attach(servo_x1_pin, 500, 2400); // attaches the servo to it's pin
  servo_x2.setPeriodHertz(50);    // standard 50 hz servo
	servo_x2.attach(servo_x2_pin, 500, 2400); // attaches the servo to it's pin
  servo_z1.setPeriodHertz(50);    // standard 50 hz servo
	servo_z1.attach(servo_z1_pin, 500, 2400); // attaches the servo to it's pin
  servo_z2.setPeriodHertz(50);    // standard 50 hz servo
	servo_z2.attach(servo_z2_pin, 500, 2400); // attaches the servo to it's pin
}

void loop() {
  // put your main code here, to run repeatedly:
  servo_x1.write(servo_0);
  servo_x2.write(servo_0);
  servo_z1.write(servo_0);
  servo_z2.write(servo_0);
  delay(100);
}
