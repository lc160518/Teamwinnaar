#include <Arduino.h>
#include <Wire.h>
#include <WiFi.h>
#include <AsyncTCP.h> // https://github.com/me-no-dev/AsyncTCP
#include <ESPAsyncWebServer.h>
#include <PID_v1.h>


// The MPU6050 part
const int MPU_addr = 0x68;
int16_t AcX, AcY, AcZ, Tmp, GyX, GyY, GyZ;
int minVal = 265;
int maxVal = 402;
double x;
double y;
double z;
double x_tot;
double z_tot;
double Cal_x;
double Cal_z;
double correct_x;
double correct_z;
const double Kp = 2000;
const double Ki = 5;
const double Kd = 1;
const int servo_0 = 90;

// The PID part
PID myPIDx(&x, &correct_x, &Cal_x, Kp, Ki, Kd, DIRECT);
PID myPIDz(&z, &correct_z, &Cal_z, Kp, Ki, Kd, DIRECT);


const char* ssid = "JULIAN";
const char* password = "12345678";

AsyncWebServer server(80);

void wifiConnect() {
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println(WiFi.localIP());
  server.begin();
}

void readGyro()
{
  Wire.beginTransmission(MPU_addr);
  Wire.write(0x3B);
  Wire.endTransmission(false);
  Wire.requestFrom(MPU_addr, 14, 1);
  AcX = Wire.read() << 8 | Wire.read();
  AcY = Wire.read() << 8 | Wire.read();
  AcZ = Wire.read() << 8 | Wire.read();
  GyX = Wire.read() << 8 | Wire.read();
  GyY = Wire.read() << 8 | Wire.read();
  GyZ = Wire.read() << 8 | Wire.read();
  int xAng = map(AcX, minVal, maxVal, -90, 90);
  int yAng = map(AcY, minVal, maxVal, -90, 90);
  int zAng = map(AcZ, minVal, maxVal, -90, 90);

  x = RAD_TO_DEG * (atan2(-yAng, -zAng) + PI);
  y = RAD_TO_DEG * (atan2(-xAng, -zAng) + PI);
  z = RAD_TO_DEG * (atan2(-yAng, -xAng) + PI);

  delay(400);
}

// calibrating the PID
void calibrate()
{
  Serial.print("Calibrating");
  // calibrate the x and z values
  for (byte i = 0; i < 10; i = i + 1)
  {
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

String PID_functie()
{
  readGyro();

  myPIDx.Compute();
  myPIDz.Compute();

  String x_str = String(x); // Assign value to x_str
  String cal_x_str = String(Cal_x); // Assign value to cal_x_str
  String correct_x_str = String(correct_x); // Assign value to correct_x_str
  String z_str = String(z); // Assign value to z_str
  String cal_z_str = String(Cal_z); // Assign value to cal_z_str
  String correct_z_str = String(correct_z); // Assign value to correct_z_str

  // Combine all strings into 1 print
  String output = "x: " + x_str + "  --- cal_x: " + cal_x_str + "  --- correct_x: " + correct_x_str + "    ------- z: " + z_str + "  --- cal_z: " + cal_z_str + "  --- correct_z: " + correct_z_str;

  String newline_output = output + "\n";

  //Serial.println(output);
  return newline_output;

/*
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
  */

  delay(5);
}

void post_output(){
    server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(200, "text/html", "<html><body><script>setInterval(function(){fetch('/newline_output').then(response => response.text()).then(data => document.getElementById('newline_output').innerText = data);}, 1000);</script><h1 id='newline_output'></h1></body></html>");
  });

  server.on("/newline_output", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(200, "text/plain", PID_functie());
  });

  server.begin();
}

void setup() {
  Wire.begin();
  Wire.beginTransmission(MPU_addr);
  Wire.write(0x6B);
  Wire.write(0);
  Wire.endTransmission(true);
  Serial.begin(115200);

  wifiConnect();
  calibrate();
}



void loop() {
  post_output();
  delay(1000);
}