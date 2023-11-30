// Baro source: BMP180 library's altitude example
// readGyro source: https://how2electronics.com/measure-tilt-angle-mpu6050-arduino/

#include <SFE_BMP180.h>
#include <Servo.h>
#include <Wire.h>

// compiler haalt dit eruit waardoor het leesbaar is maar geen computing power kost
const int LAUNCH = 0;
const int FLIGHT = 1;
const int DROP = 2;
const int LANDING = 3;
const int LANDED = 4;


int status = 0;
int currentSpeed = 0;
int maxSpeed = 100;
int height = 0;
int thrust = 0;
int maxThrust = 100;
bool errorPushed = false;

// de PID variabelen
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

// de Barometer variabelen
SFE_BMP180 pressure;

double baseline;


// PID Setup
void setupPID(){
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

// het bijsturen als waardes niet binnen een bepaalde range zijn
void PID() {
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

// leest de waardes van de Gyro
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

// De barometer setup
void bootBaro(){
  Serial.begin(9600);
  Serial.println("REBOOT");

  // initialize the sensor
  if (pressure.begin())
    Serial.println("BMP180 init success");
  else
  {
    Serial.println("BMP180 init fail\n\n");
    while(1); // pause forever
  }

  baseline = getPressure();
  Serial.print(baseline);
  Serial.print(" mb");
}

int getHeight(){
  double a, P;

  P = getPressure();

  a = pressure.altitude(P, baseline);

  Serial.println("relative altitude: ");
  if (a >= 0.0) Serial.print(" "); // add a space for positive numbers
  Serial.print(a,1);
  Serial.print(" meters");

  delay(500);
  return a;
}

// meet de luchtdruk om daar mee te kunnen rekenen
double getPressure()
{
  char status;
  double T,P,p0,a;

    // You must first get a temperature measurement to perform a pressure reading.
  
  // Start a temperature measurement:
  // If request is successful, the number of ms to wait is returned.
  // If request is unsuccessful, 0 is returned.

  status = pressure.startTemperature();
  if (status != 0)
  {
    // wait for the measurement to complete

    delay(status);
    
    // Retrieve the completed temperature measurement:
    // Note that the measurement is stored in the variable T.
    // Use '&T' to provide the address of T to the function.
    // Function returns 1 if successful, 0 if failure.

    status = pressure.getTemperature(T);
    if (status != 0)
    {
      // Start a pressure measurement:
      // The parameter is the oversampling setting, from 0 to 3 (highest res, longest wait).
      // If request is successful, the number of ms to wait is returned.
      // If request is unsuccessful, 0 is returned.

      status = pressure.startPressure(3);
      if (status != 0)
      {
        // wait for the measurement to complete
        delay(status);

        // Retrieve the completed pressure measurement:
        // Note that the measurement is stored in the variable P.
        // Use '&P' to provide the address of P.
        // Note also that the function requires the previous temperature measurement (T).
        // (If temperature is stable, you can do one temperature measurement for a number of pressure measurements.)
        // Function returns 1 if successful, 0 if failure.

        status = pressure.getPressure(P, T);
        if (status != 0)
        {
          return(P);
        }
        else Serial.println("error retrieving pressure measurement\n");
      }
      else Serial.println("error retrieving pressure measurement\n");
    }
    else Serial.println("error retrieving pressure measurement\n");
  }
  else Serial.println("error retrieving pressure measurement\n");
}

// safety clause
void error() {
  // ejectParachute();
  // stopMotor();
}

void setup() {
  bootBaro();
  setupPID();
}

void loop() {
  // ejects parachute and stops motor when we see something go wrong
  if(errorPushed){
    error();
  }

  while(status == LAUNCH){
    if(errorPushed){
      error();
    }

    height = getHeight();

    thrust += 5;
    PID();
    if(currentSpeed >= maxSpeed){
      status = FLIGHT;
    }
  }

  while(status == FLIGHT){
    if(errorPushed){
      error();
    }
    
    height = getHeight();
    
    PID();
    if(height == 20){
      status = DROP;
    }
  }

  while(status == DROP){
    if(errorPushed){
      error();
    }
    
    height = getHeight();

    thrust = 0;
    PID();

    if(height = 10){
      status = LANDING;
    }
  }

  // het landdeel moet verbeterd worden
  while(status == LANDING){
    if(errorPushed){
      error();
    }
    
    height = getHeight();
    
    thrust == maxThrust - x;
    if(thrust > 0){
      x += 1;
    }

    if(height == 0){
      status = LANDED;
    }
  }
}
