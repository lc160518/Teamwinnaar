#include <Arduino.h>
#include <Wire.h>       // lib voor het lezen van de gyro
#include <ESP32Servo.h> // lib voor het aansturen van de motoren

#define SERVO_PIN1 19 // pin van de servo
#define SERVO_PIN2 5
#define SERVO_PIN3 17
#define SERVO_PIN4 23

Servo servo1;
Servo servo2;
Servo servo3;
Servo servo4;

float RateRoll, RatePitch, RateYaw; // waardes van de gyro
float RateCalibrationRoll, RateCalibrationPitch,
    RateCalibrationYaw;      // calibratie waardes van de gyro
int RateCalibrationNumber;   // number of waardes gebruikt voor calibratie
float AccX, AccY, AccZ;      // waardes van de accelerometer
float AngleRoll, AnglePitch; // waardes van de hoek
float LoopTimer;             // geen idee wat dit is

float KalmanAngleRoll = 0, KalmanUncertaintyAngleRoll = 2 * 2; // waardes van de kalman filter, 0 is de start waarde en 2*2 is de start onzekerheid
float KalmanAnglePitch = 0, KalmanUncertaintyAnglePitch = 2 * 2;
float Kalman1DOutput[] = {0, 0}; // array voor de output van de kalman filter, state en uncertainty

float DesiredAngleRoll, DesiredAnglePitch; // waardes van de gewenste hoek
float ErrorAngleRoll, ErrorAnglePitch;     // waardes van de error van de hoek

float PrevErrorAngleRoll, PrevErrorAnglePitch; // waardes van de vorige error van de hoek
float PrevItermAngleRoll, PrevItermAnglePitch; // waardes van de vorige Iterm van de hoek
float PAngleRoll = 2;
float PAnglePitch = PAngleRoll; // waardes van de P van de hoek
float IAngleRoll = 0;
float IAnglePitch = IAngleRoll; // waardes van de I van de hoek
float DAngleRoll = 0;
float DAnglePitch = DAngleRoll; // waardes van de D van de hoek

float DesiredRateRoll, DesiredRatePitch, DesiredRateYaw; // waardes van de gewenste rotatie
float ErrorRateRoll, ErrorRatePitch, ErrorRateYaw;       // waardes van de error van de rotatie

float InputRoll, InputThrottle, InputPitch, InputYaw;          // waardes van de input van de PID
float PrevErrorRateRoll, PrevErrorRatePitch, PrevErrorRateYaw; // waardes van de vorige error van de rotatie
float PrevItermRateRoll, PrevItermRatePitch, PrevItermRateYaw; // waardes van de vorige Iterm van de rotatie
float PIDReturn[] = {0, 0, 0};                                 // array voor de output van de PID, roll, pitch en yaw

float PRateRoll = 0.6;
float PRatePitch = PRateRoll;
float PRateYaw = 2; // waardes van de P van de PID
float IRateRoll = 3.5;
float IRatePitch = IRateRoll;
float IRateYaw = 12; // waardes van de I van de PID
float DRateRoll = 0.03;
float DRatePitch = DRateRoll;
float DRateYaw = 0; // waardes van de D van de PID

float MotorInput1, MotorInput2, MotorInput3, MotorInput4; // waardes van de input van de motoren

void kalman_1d(float KalmanState, float KalmanUncertainty, float KalmanInput, float KalmanMeasurement)
{ // functie van de kalman filter, witchcraft
  KalmanState = KalmanState + 0.004 * KalmanInput;
  KalmanUncertainty = KalmanUncertainty + 0.004 * 0.004 * 4 * 4;
  float KalmanGain = KalmanUncertainty * 1 / (1 * KalmanUncertainty + 3 * 3);
  KalmanState = KalmanState + KalmanGain * (KalmanMeasurement - KalmanState);
  KalmanUncertainty = (1 - KalmanGain) * KalmanUncertainty;

  Kalman1DOutput[0] = KalmanState; // output van de kalman filter
  Kalman1DOutput[1] = KalmanUncertainty;
}
// de functie van het uitlezen van de gyro
void gyro_signal(void)
{
  Wire.beginTransmission(0x68); // uit verlagen van high frequentie ruis van de gyro, ook wel de Digital Low Pass Filter genoemd.
  Wire.write(0x1A);
  Wire.write(0x05);
  Wire.endTransmission();

  Wire.beginTransmission(0x68); // hiermee wordt de sensitivity van de accelerometer ingesteld op 8g.
  Wire.write(0x1C);
  Wire.write(0x10);
  Wire.endTransmission();

  Wire.beginTransmission(0x68); // de memory allocation ofzo
  Wire.write(0x3B);
  Wire.endTransmission();
  Wire.requestFrom(0x68, 6);
  int16_t AccXLSB = Wire.read() << 8 | Wire.read(); // het opslaan van de meest recente waardes van de acc in de variabelen in 16 bit formaat.
  int16_t AccYLSB = Wire.read() << 8 | Wire.read();
  int16_t AccZLSB = Wire.read() << 8 | Wire.read();

  Wire.beginTransmission(0x68); // hiermee wordt de sensitivity van de gyro ingesteld op 500 graden per seconde.
  Wire.write(0x1B);
  Wire.write(0x8);
  Wire.endTransmission();

  Wire.beginTransmission(0x68); // de memory allocation ofzo
  Wire.write(0x43);
  Wire.endTransmission();

  Wire.requestFrom(0x68, 6); // hier zetten we de 16 bit waardes om naar degs / sec
  int16_t GyroX = Wire.read() << 8 | Wire.read();
  int16_t GyroY = Wire.read() << 8 | Wire.read();
  int16_t GyroZ = Wire.read() << 8 | Wire.read();
  RateRoll = (float)GyroX / 65.5;
  RatePitch = (float)GyroY / 65.5;
  RateYaw = (float)GyroZ / 65.5;

  AccX = (float)AccXLSB / 4096 - 0.07; // hier zetten we de 16 bit waardes om naar g's en corrigeren we de waardes voor de offset van de acc.
  AccY = (float)AccYLSB / 4096 - 0.01;
  AccZ = (float)AccZLSB / 4096 + 0.12;

  // voor als we wat extra significante cijfers nodid hebben    AngleRoll = atan(AccY /sqrt(AccX * AccX + AccZ * AccZ))*1/(3.14159265358979323846264338327950288419716939937510582097494459230781640628620899862803482534211706798214808651328230664709384460955058223172535940812848111745028410270193852110555964462294895493038196442881097566593344612847564823378678316527120190914564856692346034861045432664821339360726024914127372458700660631558817488152092096282925409171536436789259036001133053054882046652138414695194151160943305727036575959195309218611738193261179310511854807446237996274956735188575272489122793818301194912983367336244065664308602139494639522473719070217986094370277053921716329317675238467481846766940513200056812714526356082778577134275778960917363717872146844090122495343014654958537105079227968925892354201995611212902196086403441815981362977471309960518707211349999998372978049951059731732816096318595024459455346908302642522308253344685035261931188171010003137838752886587533208381420617177669147303598253490428755678736893715195818557780532171226806613001927876611195909216420198/180); // hier berekenen we de hoek van het board ofwel witchcraft.
  AngleRoll = atan(AccY / sqrt(AccX * AccX + AccZ * AccZ)) * 1 / (3.14159265359 / 180); // hier berekenen we de hoek van het board ofwel witchcraft.
  AnglePitch = atan(AccX / sqrt(AccY * AccY + AccZ * AccZ)) * 1 / (3.14159265359 / 180);
}

void PID_equation(float Error, float P, float I, float D, float PrevError, float PrevIterm)
{
  float Pterm = P * Error;                                       // berekenen van de Pterm
  float Iterm = PrevIterm + I * (Error + PrevError) * 0.004 / 2; // berekenen van de Iterm
  if (Iterm > 400)
    Iterm = 400; // limiteren van de Iterm
  else if (Iterm < -400)
    Iterm = -400;
  float Dterm = D * (Error - PrevError) / 0.004; // berekenen van de Dterm
  float PIDOutput = Pterm + Iterm + Dterm;       // berekenen van de PID output
  if (PIDOutput > 400)
    PIDOutput = 400; // limiteren van de PID output
  else if (PIDOutput < -400)
    PIDOutput = -400;

  PIDReturn[0] = PIDOutput; // output van de PID
  PIDReturn[1] = Error;
  PIDReturn[2] = Iterm;
}

void reset_pid(void)
{
  PrevErrorRateRoll = 0; // resetten van de PID
  PrevErrorRatePitch = 0;
  PrevErrorRateYaw = 0;
  PrevItermRateRoll = 0;
  PrevItermRatePitch = 0;
  PrevItermRateYaw = 0;
  PrevErrorAngleRoll = 0;
  PrevErrorAnglePitch = 0;
  PrevItermAngleRoll = 0;
  PrevItermAnglePitch = 0;
}

void setup()
{
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, HIGH);

  servo1.attach(SERVO_PIN1); // aansturen van de servo's
  servo2.attach(SERVO_PIN2);
  servo3.attach(SERVO_PIN3);
  servo4.attach(SERVO_PIN4);

  Serial.begin(115200);
  Wire.begin();

  Wire.beginTransmission(0x68);
  Wire.write(0x6B);       // start gyro in power mode
  Wire.write(0x00);       // alle bits in register moeten naar 0
  Wire.endTransmission(); // terminate connection met gyro

  for (RateCalibrationNumber = 0; RateCalibrationNumber < 2000; RateCalibrationNumber++)
  {
    gyro_signal(); // neem 2000 waardes van de gyro en sla ze op in de variabelen. Tijdens cal niet bewegen met het board.
    RateCalibrationRoll += RateRoll;
    RateCalibrationPitch += RatePitch;
    RateCalibrationYaw += RateYaw;
    delay(1);
  }
  RateCalibrationRoll /= 2000; // dit zijn de waardes waar de gyro's rotation rate op 0 zijn.
  RateCalibrationPitch /= 2000;
  RateCalibrationYaw /= 2000;
  digitalWrite(LED_BUILTIN, LOW);
}

void loop()
{
  gyro_signal();
  RateRoll -= RateCalibrationRoll; // correcte waardes van de gyro
  RatePitch -= RateCalibrationPitch;
  RateYaw -= RateCalibrationYaw;

  ErrorRateRoll = DesiredRateRoll - RateRoll; // berekenen van de error van de rotatie
  ErrorRatePitch = DesiredRatePitch - RatePitch;
  ErrorRateYaw = DesiredRateYaw - RateYaw;
  /*
      Serial.print("RateRoll: "); ongebruikt code van raw waardes van de gyro
      Serial.print(RateRoll);
      Serial.print(" RatePitch: ");
      Serial.print(RatePitch);
      Serial.print(" RateYaw: ");
      Serial.println(RateYaw);
      delay(50);

      Serial.print(" Acceleration X [g]= "); // ongebruikt code van raw waardes van de acc
      Serial.print(AccX);
      Serial.print(" Acceleration Y [g]= ");
      Serial.print(AccY);
      Serial.print(" Acceleration Z [g]= ");
      Serial.println(AccZ);
      delay(50);

          Serial.print(" Angle Roll [deg]= "); // code van de hoek printen
      Serial.print(AngleRoll);
      Serial.print(" Angle Pitch [deg]= ");
      Serial.println(AnglePitch);
      delay(50);

      */

  kalman_1d(KalmanAngleRoll, KalmanUncertaintyAngleRoll, RateRoll, AngleRoll); // kalman filter toepassen op de hoek
  KalmanAngleRoll = Kalman1DOutput[0];
  KalmanUncertaintyAngleRoll = Kalman1DOutput[1];

  kalman_1d(KalmanAnglePitch, KalmanUncertaintyAnglePitch, RatePitch, AnglePitch); // kalman filter toepassen op de hoek
  KalmanAnglePitch = Kalman1DOutput[0];
  KalmanUncertaintyAnglePitch = Kalman1DOutput[1];

  ErrorAngleRoll = DesiredAngleRoll - KalmanAngleRoll; // berekenen van de error van de hoek
  ErrorAnglePitch = DesiredAnglePitch - KalmanAnglePitch;

  Serial.print("Roll Angle [deg] "); // printen van de hoek
  Serial.print(KalmanAngleRoll);
  Serial.print(" Pitch Angle [deg] ");
  Serial.println(KalmanAnglePitch);
  delay(50); // tijd nodig om nieuwe waardes te meten

  PID_equation(ErrorAngleRoll, PAngleRoll, IAngleRoll, DAngleRoll, PrevErrorAngleRoll, PrevItermAngleRoll); // toepassen van de PID op de hoek
  DesiredRateRoll = PIDReturn[0];                                                                           // output van de PID
  PrevErrorAngleRoll = PIDReturn[1];
  PrevItermAngleRoll = PIDReturn[2];
  PID_equation(ErrorAnglePitch, PAnglePitch, IAnglePitch, DAnglePitch, PrevErrorAnglePitch, PrevItermAnglePitch); // toepassen van de PID op de hoek
  DesiredRatePitch = PIDReturn[0];                                                                                // output van de PID
  PrevErrorAnglePitch = PIDReturn[1];
  PrevItermAnglePitch = PIDReturn[2];

  PID_equation(ErrorRateRoll, PRateRoll, IRateRoll, DRateRoll, PrevErrorRateRoll, PrevItermRateRoll);
  InputRoll = PIDReturn[0]; // output van de PID
  PrevErrorRateRoll = PIDReturn[1];
  PrevItermRateRoll = PIDReturn[2];
  PID_equation(ErrorRatePitch, PRatePitch, IRatePitch, DRatePitch, PrevErrorRatePitch, PrevItermRatePitch);
  InputPitch = PIDReturn[0];
  PrevErrorRatePitch = PIDReturn[1];
  PrevItermRatePitch = PIDReturn[2];
  PID_equation(ErrorRateYaw, PRateYaw,
               IRateYaw, DRateYaw, PrevErrorRateYaw,
               PrevItermRateYaw);
  InputYaw = PIDReturn[0];
  PrevErrorRateYaw = PIDReturn[1];
  PrevItermRateYaw = PIDReturn[2];

  servo1.write(KalmanAngleRoll);
  servo2.write(-KalmanAngleRoll);
  servo3.write(KalmanAnglePitch);
  servo4.write(-KalmanAnglePitch);
}
