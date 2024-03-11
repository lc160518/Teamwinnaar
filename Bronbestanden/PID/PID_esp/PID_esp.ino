#include <Wire.h>
#include <ESP32Servo.h>
#include <Arduino.h>
#include <PID_v1.h>
#include <SimpleKalmanFilter.h>
#include <WiFi.h>
#include <WebServer.h>
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
const double Kp = 0.2;
const double Ki = 0.1;
const double Kd = 0.0000005;

// Definieer de servo's en de pinnen waar ze op aangesloten zijn en de standaard positie
Servo servo_x1, servo_x2, servo_z1, servo_z2;

// servo1_pin = D19
// servo2_pin = D5
// servo3_pin = D17
// servo4_pin = D23

const int servo_x1_pin = 19;
const int servo_x2_pin = 5;
const int servo_z1_pin = 17;
const int servo_z2_pin = 23;
const int servo_0 = 90;


PID myPIDx(&x, &correct_x, &Cal_x, Kp, Ki, Kd, DIRECT);
PID myPIDz(&z, &correct_z, &Cal_z, Kp, Ki, Kd, DIRECT);

//Kalman filter objects
SimpleKalmanFilter  kf_x(x, 0.001, 0.0001);
SimpleKalmanFilter  kf_y(y, 0.001, 0.0001);
SimpleKalmanFilter  kf_z(z, 0.001, 0.0001);

//Webpagina functie
void handleRoot() {
  String page = "<!DOCTYPE html>\n";
  page += "<html>\n";
  page += "<head><title>ESP32 Slider Control</title></head>\n";
  page += "<body>\n";
  page += "<h1>ESP32 Slider Control</h1>\n";
  page += "<input type=\"range\" min=\"0\" max=\"100\" value=\"" + String(sliderValue) + "\" class=\"slider\" id=\"myRange\" name=\"value\" style=\"width: 80%;\">\n";
  page += "<p>Value: <span id=\"demo\">" + String(sliderValue) + "</span></p>\n";
  page += "<input type=\"number\" id=\"manualInput\" value=\"" + String(sliderValue) + "\" min=\"0\" max=\"100\" style=\"width: 80%;\">\n";
  page += "<button onclick=\"setToZero()\" style=\"font-size: 30px; padding: 20px;\">ABORT</button>\n";
  page += "<button onclick=\"updateSliderFromInput()\" style=\"font-size: 30px; padding: 20px;\">Update Slider</button>\n";
  page += "<script>\n";
  page += "var slider = document.getElementById(\"myRange\");\n";
  page += "var output = document.getElementById(\"demo\");\n";
  page += "var manualInput = document.getElementById(\"manualInput\");\n";
  page += "output.innerHTML = slider.value;\n";
  page += "slider.oninput = function() {\n";
  page += "  output.innerHTML = this.value;\n";
  page += "  updateSlider(this.value);\n"; // Update the slider value and send the update
  page += "}\n";
  page += "function updateSliderFromInput() {\n";
  page += "  var inputValue = parseInt(manualInput.value);\n";
  page += "  if (!isNaN(inputValue) && inputValue >= 0 && inputValue <= 100) {\n";
  page += "    slider.value = inputValue;\n";
  page += "    output.innerHTML = inputValue;\n"; // Update the displayed value next to the slider
  page += "    updateSlider(inputValue);\n";
  page += "  }\n";
  page += "}\n";
  page += "function updateSlider(val) {\n";
  page += "  var xhttp = new XMLHttpRequest();\n";
  page += "  xhttp.onreadystatechange = function() {\n";
  page += "    if (this.readyState == 4 && this.status == 200) {\n";
  page += "      console.log('Value updated');\n";
  page += "    }\n";
  page += "  };\n";
  page += "  xhttp.open(\"GET\", \"/update?value=\" + val, true);\n";
  page += "  xhttp.send();\n";
  page += "}\n";
  page += "function setToZero() {\n";
  page += "  slider.value = 0;\n";
  page += "  output.innerHTML = 0;\n";
  page += "  updateSlider(0);\n";
  page += "}\n";
  page += "</script>\n";
  page += "</body>\n";
  page += "</html>\n";
  server.send(200, "text/html", page);
}

void handleUpdate() {
  if (server.hasArg("value")) {
    String valueStr = server.arg("value");
    sliderValue = valueStr.toInt();
    escValue = map(sliderValue, 0, 100, 1000, 3000);
    escValue = constrain(escValue, 1000, 3000);

  }
  server.send(200, "text/plain", "OK");
}

// De functie voor het aansturen van de EDF  (potmeter versie)
void motorBusiness(){
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
  
  x = kf_x.updateEstimate(RAD_TO_DEG * (atan2(-yAng, -zAng)+PI));
  y = kf_y.updateEstimate(RAD_TO_DEG * (atan2(-xAng, -zAng)+PI));
  z = kf_z.updateEstimate(RAD_TO_DEG * (atan2(-xAng, -zAng)+PI));
  x = y;
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
// Functie voor het koppelen van servo's aan assen (Yaw en Pitch)
void controlYawPitch(){
  reverse_directions(); 
  
  // PID berekening
  myPIDx.Compute();
  myPIDz.Compute();
 
 int limit = 45;
   
  // The PID value will always be positive so this is to make it go two ways instead of ons
  if(reverseX){
    correct_x = -1 * correct_x; 
  }
  if(reverseZ){
    correct_z = -1 * correct_z;
  }
  // Begrenzing van PID output (optioneel)
  correct_x = constrain(correct_x, -limit, limit);  // limit is a defined value
  correct_z = constrain(correct_z, -limit, limit);

  // Berekening van gewogen servo posities
  int servo_x_pos = servo_0 + (weight * correct_x);
  int servo_z_pos = servo_0 + (weight * correct_z);

  Serial.print("servo_x_pos = "); Serial.println(servo_x_pos);
  Serial.print("servo_z_pos = "); Serial.println(servo_z_pos); Serial.println("");

  // Besturing van tegenoverliggende servo's (Yaw)
  servo_x1.write(servo_x_pos);
  servo_x2.write(2 * servo_0 - servo_x_pos);  // 2*servo_0 is the maximum angle

  // Besturing van tegenoverliggende servo's (Pitch)
  servo_z1.write(servo_z_pos);
  servo_z2.write(2 * servo_0 - servo_z_pos);
}

// het connected van de servo's en naar 90 graden zetten
void attachServos(){
  // connect the servo's
  servo_x1.setPeriodHertz(50);    // standard 50 hz servo
	servo_x1.attach(servo_x1_pin, 500, 2400); // attaches the servo to its pin
  servo_x2.setPeriodHertz(50);    // standard 50 hz servo
	servo_x2.attach(servo_x2_pin, 500, 2400); // attaches the servo to its pin
  servo_z1.setPeriodHertz(50);    // standard 50 hz servo
	servo_z1.attach(servo_z1_pin, 500, 2400); // attaches the servo to its pin
  servo_z2.setPeriodHertz(50);    // standard 50 hz servo
	servo_z2.attach(servo_z2_pin, 500, 2400); // attaches the servo to its pin

  // reset the servo to it's base positions
  servo_x1.write(servo_0);
  servo_x2.write(servo_0);
  servo_z1.write(servo_0);
  servo_z2.write(servo_0);
}

// het calibreren van de PID
void calPID(){
  Serial.println("Calibrating");
  int num_samples = 100;
  // calibrate the x and z values
  for(byte i = 0; i < num_samples; i = i + 1){
    readGyro();
    x_tot = x_tot + x;
    z_tot = z_tot + z;
  }

  Cal_x = x_tot / num_samples;
  Cal_z = z_tot / num_samples;

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
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");


  server.on("/", HTTP_GET, handleRoot);
  server.on("/update", HTTP_GET, handleUpdate);

  server.begin();
  Serial.println("HTTP server started");

  delay(2000);
  // Zet de pinnen als output of input
  pinMode(potPin, INPUT);
  pinMode(escPin, OUTPUT);
  attachServos();

  calPID();

}

void loop() {
  server.handleClient();
  readGyro();
  controlYawPitch();

//  motorBusiness();

  
  delay(400);
}