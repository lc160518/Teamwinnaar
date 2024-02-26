#include <Wire.h>
#include <ESP32Servo.h>
#include <Arduino.h>
#include <PID_v1.h>

// Definieer de variabelen voor de Arduino aansluitingen
const int potPin = 34; // De potmeter is verbonden met analoge pin D4
const int escPin = 18; // De esc is verbonden met digitale pin D5

// Definieer de variabelen voor de potmeter en de esc
int potValue, throttle, escValue;
const int MPU_addr=0x68;
int16_t AcX,AcY,AcZ,Tmp,GyX,GyY,GyZ;

// Definieer de variabelen voor de gyro
int minVal=265;
int maxVal=402;

// Definieer de variabelen voor de gyro
double x, y, z;
bool reverseX, reverseZ;
double x_tot, z_tot;
double Cal_x, Cal_z;
double correct_x, correct_z;

// Determines how much the correction has to go
double weight = 1;
const double Kp = 0.02;
const double Ki = 0.01;
const double Kd = 0.00000005;

// Definieer de servo's en de pinnen waar ze op aangesloten zijn en de standaard positie
Servo servo_x1, servo_x2, servo_z1, servo_z2;
const int servo_x1_pin = 14;
const int servo_x2_pin = 13;
const int servo_z1_pin = 27;
const int servo_z2_pin = 12;
const int servo_0 = 90;


PID myPIDx(&x, &correct_x, &Cal_x, Kp, Ki, Kd, DIRECT);
PID myPIDz(&z, &correct_z, &Cal_z, Kp, Ki, Kd, DIRECT);



// De functie voor het aansturen van de EDF  (potmeter versie)
void motorBusiness(){
  // Als de stroomtoestand true is, lees dan de waarde van de potmeter en zet het om naar een throttle waarde (0-100)
    potValue = analogRead(potPin);
    throttle = map(potValue, 0, 1023, 0, 100);
    throttle = constrain(throttle, 0, 100);

    // Zet de throttle waarde om naar een esc waarde (1000-2000)
    escValue = map(throttle, 0, 100, 1000, 2000);
    escValue = constrain(escValue, 1000, 2000);

    // Stuur de esc waarde naar de esc pin met een pulsduur van escValue microseconden
    digitalWrite(escPin, HIGH);
    delayMicroseconds(escValue);
    digitalWrite(escPin, LOW);
    delayMicroseconds(20000 - escValue);

    // Toon de throttle waarde op de console
    Serial.print("Throttle: ");
    Serial.print(throttle);
    Serial.println("%");
    Serial.print("Speed controller: ");
    Serial.println(escValue);

    // Als de stroomtoestand false is, stuur dan een esc waarde van 1000 naar de esc pin om de EDF uit te zetten
    digitalWrite(escPin, HIGH);
    delayMicroseconds(1000);
    digitalWrite(escPin, LOW);
    delayMicroseconds(19000);
}

// het uitlezen van de gyro en opslaan als graden als x, y en z
void readGyro(){
  Wire.beginTransmission(MPU_addr);
  Wire.write(0x68);
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
}

// het printen van de x, y, z & cal_x, cal_z & correct_x, correct_z
void printValues(){
  // print the values just to be safe
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
}

// het omdraaien van sommige variabelen maar zeker niet alle
void reverse_directions(){
  //PID only works in one way, this makes it work in both ways
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
}

// het connected van de servo's en naar 90 graden zetten
void attachServos(){
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

// het calibreren van de PID
void calPID(){
  Serial.println("Calibrating");
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

void setup() {  
  // Starts the communication with the gyro
  Wire.begin();
  Wire.beginTransmission(MPU_addr);
  Wire.write(0x6B);
  Wire.write(0);
  Wire.endTransmission(true);
  Serial.begin(115200);

  // Zet de pinnen als output of input
  pinMode(potPin, INPUT);
  pinMode(escPin, OUTPUT);
  attachServos();

  calPID();

}

void loop() {
  motorBusiness();
  readGyro();
  reverse_directions(); 
  
  // The PID part
  myPIDx.Compute();
  myPIDz.Compute();
  printValues();

  // The PID value will always be positive so this is to make it go two ways instead of ons
  if(reverseX){
    correct_x = -1 * correct_x;
  }
  if(reverseZ){
    correct_z = -1 * correct_z;
  }

  // Zet de servo's op de goede plek
  servo_x1.write(servo_0 - (weight * correct_x));
  servo_x2.write(servo_0 + (weight * correct_x));
  servo_z1.write(servo_0 - (weight * correct_z));
  servo_z2.write(servo_0 + (weight * correct_z));
  
  delay(10);
}
