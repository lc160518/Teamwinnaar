#include <PID_v1.h>

const int MPU_addr=0x69;
int16_t AcX,AcY,AcZ,Tmp,GyX,GyY,GyZ;
 
int minVal=265;
int maxVal=402;

double x_list[13] = {180.00, 180.03, 180.55, 180.70, 181.34, 183.34, 193.23, 192.10, 189.55, 186.42, 184.10, 182.82, 180.00};
double z_list[13] = {180.00, 180.00, 180.00, 180.00, 180.00, 180.00, 180.00, 180.00, 180.00, 180.00, 180.00, 180.00, 180.00};

int i = 0;
int n = 0;

double x;
double y;
double z;

double Cal_x = 180.00;
double Cal_z = 180.00;

double correct_x;
double correct_z;

const double Kp = 0.2;
const double Ki = 0.5;
const double Kd = 0.1;

PID myPIDx(&x, &correct_x, &Cal_x, Kp, Ki, Kd, DIRECT);
PID myPIDz(&z, &correct_z, &Cal_z, Kp, Ki, Kd, DIRECT);

void setup() {
  // Starts the communication with the gyro
  Serial.begin(9600);

}

void loop() {
  x = x_list[n];
  z = z_list[n];
  if(i % 10 = 0){
    n += 1;
  }
  
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

  i += 1;
  delay(5);
}
