/*
 * This ESP32 code is created by esp32io.com
 *
 * This ESP32 code is released in the public domain
 *
 * For more detail (instruction and wiring diagram), visit https://esp32io.com/tutorials/esp32-potentiometer
 */

float floatMap(float x, float in_min, float in_max, float out_min, float out_max) {
  return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}

const int buttonPin = 22;
int buttonState = 0;
int lastButtonState = 0;
bool toggleState = false;

// the setup routine runs once when you press reset:
void setup() {
  // initialize serial communication at 115200 bits per second:
  Serial.begin(115200);
  pinMode(buttonPin, INPUT);
}

void checkButton() {
  if (buttonState == HIGH && lastButtonState == LOW) {
    toggleState = !toggleState;
    Serial.print("Button Pressed! Toggle State: ");
    Serial.println(toggleState);
  }
  lastButtonState = buttonState;
}

// the loop routine runs over and over again forever:
void loop() {
  buttonState = digitalRead(buttonPin);
  
  int analogValue = analogRead(32);
  // Rescale to potentiometer's voltage (from 0V to 3.3V):
  int aanvoer = floatMap(analogValue, 0, 4095, 0, 100);

  checkButton();

  // print out the value you read:
  Serial.print("Aanvoer: ");
  Serial.println(aanvoer);
  delay(1000);
}
