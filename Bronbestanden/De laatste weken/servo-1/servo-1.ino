// SERVO control code for controlling servos using ESP32 

#include <Arduino.h>
#include <ESP32Servo.h>

int potpin = 27;
int val;


Servo servo1;
Servo servo2;
Servo servo3;
Servo servo4;

void setup(){
  Serial.begin(115200);
  servo1.attach(5); 
  servo2.attach(23);
  servo3.attach(17);
  servo4.attach(19);
  


}

void loop(){
  
  val = analogRead(potpin);

  val = map(val, 0, 1023, -90, 90); 
  val = constrain(val, -90, 90);


  Serial.println(val);
  servo1.write(90-val);
  servo2.write(90+val);
  servo3.write(90-val);
  servo4.write(90+val);
  delay(1);

}
