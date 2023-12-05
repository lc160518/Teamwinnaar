#include "SPIFFS.h"

void setup() {
  Serial.begin(115200);
  if (!SPIFFS.begin(true)) {
    Serial.println("An error has occurred while mounting SPIFFS");
    return;
  }

  File root = SPIFFS.open("/");
  File file = root.openNextFile();

  while (file) {
    Serial.print("File: ");
    Serial.println(file.name());
    Serial.println("Content:");
    while (file.available()) {
      Serial.write(file.read());
    }
    file = root.openNextFile();
  }
}

void loop() {
  // put your main code here, to run repeatedly:
}