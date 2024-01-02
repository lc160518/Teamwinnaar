#include <ESP32Servo.h>
#include <Arduino.h>
#include <PID_v1.h>

double x_list[13] = {180.00, 180.03, 180.55, 180.23, 180.12, 179.30, 180.57, 179.23, 179.43, 180.06, 179.70, 180.65, 180.12};
double z_list[13] = {180.00, 180.03, 180.55, 180.70, 182.34, 185.34, 193.23, 192.10, 195.95, 196.42, 194.10, 193.82, 191.07};

// {180.00, 180.03, 180.55, 180.70, 182.34, 185.34, 193.23, 192.10, 195.95, 196.42, 194.10, 193.82, 191.07};

int i = 0;
int n = 0;

double x;
double y;
double z;

double Cal_x = 180.00;
double Cal_z = 180.00;

bool reverseX;
bool reverseZ;

double correct_x;
double correct_z;

const double Kp = 0.02;
const double Ki = 0.01;
const double Kd = 0.01;

double weight = 1;

Servo servo_x1;
Servo servo_x2;
Servo servo_z1;
Servo servo_z2;

const int servo_x1_pin = 26; //x = paars = 26
const int servo_x2_pin = 32; //x = zwart == 33
const int servo_z1_pin = 33; //z = groen = 32
const int servo_z2_pin = 25; //z wit = 25

const int servo_0 = 90;

PID myPIDx(&x, &correct_x, &Cal_x, Kp, Ki, Kd, DIRECT);
PID myPIDz(&z, &correct_z, &Cal_z, Kp, Ki, Kd, DIRECT);

void setup() {
  // Starts the communication with the gyro
  Serial.begin(115200); 
  myPIDx.SetMode(AUTOMATIC);
  myPIDz.SetMode(AUTOMATIC);

  // connect the servo's
  servo_x1.setPeriodHertz(50);    // standard 50 hz servo
	servo_x1.attach(servo_x1_pin, 500, 2400); // attaches the servo to it's pin
  servo_x2.setPeriodHertz(50);    // standard 50 hz servo
	servo_x2.attach(servo_x2_pin, 500, 2400); // attaches the servo to it's pin
  servo_z1.setPeriodHertz(50);    // standard 50 hz servo
	servo_z1.attach(servo_z1_pin, 500, 2400); // attaches the servo to it's pin
  servo_z2.setPeriodHertz(50);    // standard 50 hz servo
	servo_z2.attach(servo_z2_pin, 500, 2400); // attaches the servo to it's pin

  // reset the servo to it's base positions
  servo_x1.write(servo_0);
  servo_x2.write(servo_0);
  servo_z1.write(servo_0);
  servo_z2.write(servo_0);
}

void loop() {
  i += 1;
  if(n > 12){
    n = 0;
  }
  
  x = x_list[n];
  z = z_list[n];

  if(i % 5 == 0){
    n += 1;
  }

  if(x > Cal_x){
    myPIDx.SetControllerDirection(REVERSE);
    reverseX = true;
  }
  else{
    myPIDx.SetControllerDirection(DIRECT);
    reverseX= false;
  }

  if(z > Cal_z){
    myPIDz.SetControllerDirection(REVERSE);
    reverseZ = true;
  }
  else{
    myPIDz.SetControllerDirection(DIRECT);
    reverseZ = false;
  }
  
  // The PID part
  myPIDx.Compute();
  myPIDz.Compute();

  if(reverseX){
    correct_x = -1 * correct_x;
  }
  if(reverseZ){
    correct_z = -1 * correct_z;
  }

  Serial.print("\t");
  Serial.print("x");
  Serial.print("\t");
  Serial.println("z");

  Serial.print("Input:");
  Serial.print("\t");
  Serial.print(x);
  Serial.print("\t");
  Serial.println(z);

  Serial.print("Cal:");
  Serial.print("\t");
  Serial.print(Cal_x);
  Serial.print("\t");
  Serial.println(Cal_z);

  Serial.print("Output:");
  Serial.print("\t");
  Serial.print(correct_x);
  Serial.print("\t");
  Serial.println(correct_z);

  servo_x1.write(servo_0 - (weight * correct_x));
  servo_x2.write(servo_0 + (weight * correct_x));
  servo_z1.write(servo_0 - (weight * correct_z));
  servo_z2.write(servo_0 + (weight * correct_z));

  delay(500);
}
