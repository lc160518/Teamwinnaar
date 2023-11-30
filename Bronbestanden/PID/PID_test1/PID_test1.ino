#include <Servo.h>
#include <Wire.h>

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

double Cal_x;
double Cal_z;

const int servo_0 = 90;

void setup() {
  // attaches a servo to a pin
  servo_x1.attach(5);
  servo_x2.attach(10);
  servo_z1.attach(3);
  servo_z2.attach(9);

  // Starts the communication with the gyro
  Wire.begin();
  Wire.beginTransmission(MPU_addr);
  Wire.write(0x6B);
  Wire.write(0);
  Wire.endTransmission(true);
  Serial.begin(9600);

  // calibrate the x and z values
  readGyro();
  Cal_x = x;
  Cal_z = z;

}

void loop() {
  // Measures the location of the gyro
  readGyro();

  if(x >+ (Cal_x - 20) && x <= (Cal_x + 20)){
    servo_x1.write(servo_0);
    servo_x2.write(servo_0);
    delay(15);
  }
  else if(x < (Cal_x - 20)){
    servo_x1.write(servo_0 - 20);
    servo_x2.write(servo_0 + 20);
    delay(15);
  }
  else if(x > (Cal_x + 20)){
    servo_x1.write(servo_0 + 20);
    servo_x2.write(servo_0 - 20);
    delay(15);
  }

  if(z >= (Cal_z - 20) && z <= (Cal_z + 20)){
    servo_z1.write(servo_0);
    servo_z2.write(servo_0);
    delay(15);
  }
  else if(z < (Cal_z - 20)){
    servo_z1.write(servo_0 - 20);
    servo_z2.write(servo_0 + 20);
    delay(15);
  }
  else if(z > (Cal_z + 20)){
    servo_z1.write(servo_0 + 20);
    servo_z2.write(servo_0 - 20);
    delay(15);
  }
  
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
  
  Serial.print("AngleX= ");
  Serial.println(x);
  
  Serial.print("AngleY= ");
  Serial.println(y);
  
  Serial.print("AngleZ= ");
  Serial.println(z);
  Serial.println("-----------------------------------------");
  delay(400);
}
