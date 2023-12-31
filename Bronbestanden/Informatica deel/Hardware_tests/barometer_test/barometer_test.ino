#include <SFE_BMP180.h>
#include <Wire.h>
// source: BMP180 library's altitude example

SFE_BMP180 pressure;

double baseline; // baseline pressure

void setup() 
{
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

void loop() {
  double a,P;

  P = getPressure();

  a = pressure.altitude(P, baseline);

  Serial.print("relative altitude: ");
  if (a >= 0.0) Serial.print(" "); // add a space for positive numbers
  Serial.print(a,1);
  Serial.print(" meters, ");

  delay(500);
}

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
