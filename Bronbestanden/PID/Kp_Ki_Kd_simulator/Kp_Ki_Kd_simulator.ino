#include <PID_v1.h>

double x_list[13] = {180.00, 180.03, 180.55, 180.23, 180.12, 179.30, 180.57, 179.23, 179.43, 180.06, 179.70, 180.65, 180.12};
double z_list[13] = {180.00, 180.03, 180.55, 180.23, 180.12, 179.30, 180.57, 179.23, 179.43, 180.06, 179.70, 180.65, 180.12};

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

const double Kp = 1;
const double Ki = 0;
const double Kd = 0;

PID myPIDx(&x, &correct_x, &Cal_x, Kp, Ki, Kd, DIRECT);
PID myPIDz(&z, &correct_z, &Cal_z, Kp, Ki, Kd, DIRECT);

void setup() {
  // Starts the communication with the gyro
  Serial.begin(9600);
  myPIDx.SetMode(AUTOMATIC);
  myPIDz.SetMode(AUTOMATIC);
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

  delay(500);
}
