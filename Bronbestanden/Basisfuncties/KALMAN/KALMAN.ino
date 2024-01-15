#include <Arduino.h>
#include <Wire.h> // lib voor het lezen van de gyro
float RateRoll, RatePitch, RateYaw; // waardes van de gyro
float RateCalibrationRoll, RateCalibrationPitch, 
        RateCalibrationYaw; // calibratie waardes van de gyro
int RateCalibrationNumber; // number of waardes gebruikt voor calibratie
float AccX, AccY, AccZ;  // waardes van de accelerometer
float AngleRoll, AnglePitch; // waardes van de hoek
float LoopTimer; // geen idee wat dit is

float KalmanAngleRoll = 0, KalmanUncertaintyAngleRoll = 2*2; // waardes van de kalman filter, 0 is de start waarde en 2*2 is de start onzekerheid
float KalmanAnglePitch = 0, KalmanUncertaintyAnglePitch = 2*2;

float Kalman1DOutput[] = {0, 0}; // array voor de output van de kalman filter, state en uncertainty

void kalman_1d(float KalmanState, float KalmanUncertainty, float KalmanInput , float KalmanMeasurement) { // functie van de kalman filter, witchcraft
    KalmanState=KalmanState+0.004*KalmanInput;
    KalmanUncertainty=KalmanUncertainty+0.004*0.004*4*4;
    float KalmanGain=KalmanUncertainty*1/(1*KalmanUncertainty + 3 * 3);
    KalmanState = KalmanState + KalmanGain * (KalmanMeasurement - KalmanState);
    KalmanUncertainty = (1 - KalmanGain) * KalmanUncertainty;

    Kalman1DOutput[0] = KalmanState; // output van de kalman filter
    Kalman1DOutput[1] = KalmanUncertainty;
}
// de functie van het uitlezen van de gyro
 void gyro_signal(void) {
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

    AccX= (float)AccXLSB / 4096 - 0.07; // hier zetten we de 16 bit waardes om naar g's en corrigeren we de waardes voor de offset van de acc.
    AccY = (float)AccYLSB / 4096 - 0.01;
    AccZ = (float)AccZLSB / 4096 + 0.12;

    AngleRoll = atan(AccY /sqrt(AccX * AccX + AccZ * AccZ))*1/(3.142/180); // hier berekenen we de hoek van het board ofwel witchcraft.
    AnglePitch = atan(AccX /sqrt(AccY * AccY + AccZ * AccZ))*1/(3.142/180);
}

void setup() {
    Serial.begin(115200);
    Wire.begin();
    
    Wire.beginTransmission(0x68); 
    Wire.write(0x6B); //start gyro in power mode
    Wire.write(0x00); // alle bits in register moeten naar 0
    Wire.endTransmission(); // terminate connection met gyro

    for (RateCalibrationNumber = 0; RateCalibrationNumber < 2000; RateCalibrationNumber++) {
        gyro_signal(); // neem 2000 waardes van de gyro en sla ze op in de variabelen. Tijdens cal niet bewegen met het board.
        RateCalibrationRoll += RateRoll;
        RateCalibrationPitch += RatePitch;
        RateCalibrationYaw += RateYaw;
        delay(1);
    }
    RateCalibrationRoll /= 2000; // dit zijn de waardes waar de gyro's rotation rate op 0 zijn.
    RateCalibrationPitch /= 2000;
    RateCalibrationYaw /= 2000;
}

void loop() {
    gyro_signal();
    RateRoll -= RateCalibrationRoll; // correcte waardes van de gyro
    RatePitch -= RateCalibrationPitch;
    RateYaw -= RateCalibrationYaw;
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

    Serial.print("Roll Angle [deg] "); // printen van de hoek
    Serial.print(KalmanAngleRoll);
    Serial.print(" Pitch Angle [deg] ");
    Serial.println(KalmanAnglePitch);
    delay(50); // tijd nodig om nieuwe waardes te meten
}

