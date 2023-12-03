#include <Servo.h>
#include <Wire.h>
#include <PID_v1.h>

Servo servo_x1;
Servo servo_x2;
Servo servo_z1;
Servo servo_z2;

const int MPU_addr=0x68;
int16_t AcX,AcY,AcZ,Tmp,GyX,GyY,GyZ;
 
int minVal=265;
int maxVal=402;
 
double x;
double y;
double z;

double x_tot;
double z_tot;

double Cal_x;
double Cal_z;

double correct_x;
double correct_z;

const double Kp = 0.2;
const double Ki = 0.5;
const double Kd = 0.1;

const int servo_0 = 90;

PID myPIDx(&x, &correct_x, &Cal_x, Kp, Ki, Kd, DIRECT);
PID myPIDz(&z, &correct_z, &Cal_z, Kp, Ki, Kd, DIRECT);

void setup() {
  myPIDx.SetOutputLimits(-45, 45);
  myPIDz.SetOutputLimits(-45, 45);
  myPIDx.SetMode(AUTOMATIC);
  myPIDz.SetMode(AUTOMATIC);

  // attaches a servo to a pin
  servo_x1.attach(3);
  servo_x2.attach(5);
  servo_z1.attach(9);
  servo_z2.attach(10);

  servo_x1.write(servo_0);
  servo_x2.write(servo_0);
  servo_z1.write(servo_0);
  servo_z2.write(servo_0);

  // Starts the communication with the gyro
  Wire.begin();
  Wire.beginTransmission(MPU_addr);
  Wire.write(0x6B);
  Wire.write(0);
  Wire.endTransmission(true);
  Serial.begin(9600);
  
  Serial.print("Calibrating");
  // calibrate the x and z values
  for(byte i = 0; i < 10; i = i + 1){
    readGyro();
    x_tot = x_tot + x;
    z_tot = z_tot + z;
    Serial.print(".");
  }
  Serial.println("");

  Cal_x = x_tot / 10;
  Cal_z = z_tot / 10;

}

void loop() {
  // Measures the location of the gyro
  readGyro();
  
  // The PID part
  myPIDx.Compute();
  myPIDz.Compute();

  Serial.println("x-----------");
  Serial.print("Input: ");
  Serial.println(x);
  Serial.print("Setpoint: ");
  Serial.println(Cal_x);
  Serial.print("Output: ");
  Serial.println(correct_x);

  Serial.println("z-----------");
  Serial.print("Input: ");
  Serial.println(z);
  Serial.print("Setpoint: ");
  Serial.println(Cal_z);
  Serial.print("Output: ");
  Serial.println(correct_z);

  servo_x1.write(servo_0 - correct_x);
  servo_x2.write(servo_0 + correct_x);
  servo_z1.write(servo_0 - correct_z);
  servo_z2.write(servo_0 + correct_z);

  delay(5);
}


void readGyro(){
  Wire.beginTransmission(MPU_addr);
  Wire.write(0x3B);
  Wire.endTransmission(false);
  Wire.requestFrom(MPU_addr,14,true);
  AcX=Wire.read()<<8|Wire.read();
  AcY=Wire.read()<<8|Wire.read();
  AcZ=Wire.read()<<8|Wire.read();
  int xAng = map(AcX,minVal,maxVal,-90,90);
  int yAng = map(AcY,minVal,maxVal,-90,90);
  int zAng = map(AcZ,minVal,maxVal,-90,90);
  
  x= RAD_TO_DEG * (atan2(-yAng, -zAng)+PI);
  y= RAD_TO_DEG * (atan2(-xAng, -zAng)+PI);
  z= RAD_TO_DEG * (atan2(-yAng, -xAng)+PI);
  
  // Serial.print("AngleX= ");
  // Serial.println(x);
  
  // Serial.print("AngleY= ");
  // Serial.println(y);
  
  // Serial.print("AngleZ= ");
  // Serial.println(z);
  // Serial.println("-----------------------------------------");
  delay(400);
}
