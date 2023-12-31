#include <Wire.h>
#include <ESP32Servo.h>
#include <Arduino.h>
#include <PID_v1.h>

// Definieer de variabelen voor de Arduino aansluitingen
#define potPin 4 // De potmeter is verbonden met analoge pin D4
#define escPin 5 // De esc is verbonden met digitale pin D5
#define togglePin 18 // De toggle-knop is verbonden met digitale pin D18

// Definieer de variabelen voor de potmeter en de esc
int potValue; // De waarde van de potmeter (0-1023)
int throttle; // De throttle waarde (0-100)
int escValue; // De esc waarde (1000-2000)

// Definieer de variabele voor de stroomtoestand van EDF
bool power = false; // De stroomtoestand van de EDF (true of false)

// Definieer de variabele voor de vorige status van de toggle-knop
int prevToggleState = HIGH;

const int MPU_addr=0x68;
int16_t AcX,AcY,AcZ,Tmp,GyX,GyY,GyZ;
 
int minVal=265;
int maxVal=402;
 
double x;
double y;
double z;

bool reverseX;
bool reverseZ;

double x_tot;
double z_tot;

double Cal_x;
double Cal_z;

double correct_x;
double correct_z;

// Determines how much the correction has to go
double weight = 1;

const double Kp = 0.02;
const double Ki = 0.01;
const double Kd = 0.01;

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


void motorBusiness(){
  // Lees de status van de toggle-knop
  int toggleState = digitalRead(togglePin);

  // Controleer of de toggle-knop is ingedrukt en de vorige status niet hetzelfde is
  if (toggleState == LOW && prevToggleState == HIGH) {
    power = !power; // Toggle de stroomtoestand
  }

  Serial.print("EDF is ");
  Serial.println(power ? "on" : "off"); // Toon de nieuwe toestand in de console
  Serial.print("power: ");
  Serial.println(power);

  // Bewaar de huidige status van de toggle-knop voor de volgende iteratie
  prevToggleState = toggleState;

  // Als de stroomtoestand true is, lees dan de waarde van de potmeter en zet het om naar een throttle waarde (0-100)
  if (power) {
    potValue = analogRead(potPin);
    throttle = map(potValue, 0, 1023, 0, 100);

    // Zet de throttle waarde om naar een esc waarde (1000-2000)
    escValue = map(throttle, 0, 100, 1000, 2000);

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
  } else {
    // Als de stroomtoestand false is, stuur dan een esc waarde van 1000 naar de esc pin om de EDF uit te zetten
    digitalWrite(escPin, HIGH);
    delayMicroseconds(1000);
    digitalWrite(escPin, LOW);
    delayMicroseconds(19000);
  }
  delayMicroseconds(10000);
}

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
}

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

void correctDirection(){
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
  pinMode(togglePin, INPUT_PULLUP); // Gebruik de interne pull-up weerstand voor de toggle-knop

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

void loop() {
  // starts motor and that jazz
  motorBusiness();

  // Measures the location of the gyro
  readGyro();
  // makes PID go both ways
  correctDirection();
  
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
