#include <Wire.h>
#include <Arduino.h>
#include <PID_v1.h>

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

const double Kp = 2;
const double Ki = 5;
const double Kd = 1;

const int servo_0 = 90;

void readGyro();

PID myPIDx(&x, &correct_x, &Cal_x, Kp, Ki, Kd, DIRECT);
PID myPIDz(&z, &correct_z, &Cal_z, Kp, Ki, Kd, DIRECT);

void readGyro(){
  Wire.beginTransmission(MPU_addr);
  Wire.write(0x3B);
  Wire.endTransmission(false);
  Wire.requestFrom(MPU_addr,14, 1);
  AcX=Wire.read()<<8|Wire.read();
  AcY=Wire.read()<<8|Wire.read();
  AcZ=Wire.read()<<8|Wire.read();
  GyX=Wire.read()<<8|Wire.read();
  GyY=Wire.read()<<8|Wire.read();
  GyZ=Wire.read()<<8|Wire.read();
  int xAng = map(AcX,minVal,maxVal,-90,90);
  int yAng = map(AcY,minVal,maxVal,-90,90);
  int zAng = map(AcZ,minVal,maxVal,-90,90);
  
  x= RAD_TO_DEG * (atan2(-yAng, -zAng)+PI);
  y= RAD_TO_DEG * (atan2(-xAng, -zAng)+PI);
  z= RAD_TO_DEG * (atan2(-yAng, -xAng)+PI);

  delay(400);
}

void setup() {
  // Starts the communication with the gyro
  Wire.begin();
  Wire.beginTransmission(MPU_addr);
  Wire.write(0x6B);
  Wire.write(0);
  Wire.endTransmission(true);
  Serial.begin(115200);

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

  Serial.print("Cal_x: ");
  Serial.println(Cal_x);
  Serial.print("Cal_z: ");
  Serial.println(Cal_z);
  myPIDx.SetMode(AUTOMATIC);
  myPIDz.SetMode(AUTOMATIC);
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
  delay(5);
}



