#include <ESP32Servo.h> // lib voor het aansturen van de servo's

#define SERVO_PIN1 19 // pin van de servo
#define SERVO_PIN2 5
#define SERVO_PIN3 17
#define SERVO_PIN4 23

Servo servo1;
Servo servo2;
Servo servo3;
Servo servo4;

const int defaultAngle = 90;
const int constrainAngle = 45;

int i = -90;

void setup() {
  // connect the servo's
	servo1.attach(SERVO_PIN1); // attaches the servo to it's pin
	servo2.attach(SERVO_PIN2); // attaches the servo to it's pin
	servo3.attach(SERVO_PIN3); // attaches the servo to it's pin
	servo4.attach(SERVO_PIN4); // attaches the servo to it's pin
}

void loop() {
  while(i < 90){
    servo1.write(constrain(i, (defaultAngle-constrainAngle), (defaultAngle + constrainAngle));
    i = i + 10;
    delay(1000);
  }
  i = -90;
  
  while(i < 90){
    servo1.write(constrain(i, (defaultAngle-constrainAngle), (defaultAngle + constrainAngle));
    i = i + 10;
    delay(1000);
  }
  i = -90;

  while(i < 90){
    servo1.write(constrain(i, (defaultAngle-constrainAngle), (defaultAngle + constrainAngle));
    i = i + 10;
    delay(1000);
  }
  i = -90;

  while(i < 90){
    servo1.write(constrain(i, (defaultAngle-constrainAngle), (defaultAngle + constrainAngle));
    i = i + 10;
    delay(1000);
  }
  i = -90;
}
