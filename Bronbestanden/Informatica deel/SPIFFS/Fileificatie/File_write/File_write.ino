#include "SPIFFS.h"
 

void setup() {
 
  Serial.begin(115200);
  delay(5000);


  if (!SPIFFS.begin(true)) {
    Serial.println("An Error has occurred while mounting SPIFFS");
    return;
  }
 
  File file = SPIFFS.open("/test.txt", FILE_WRITE);
 
  if (!file) {
    Serial.println("There was an error opening the file for writing");
    return;
  }
  for (int i = 0; i < 50; i++) {
    file.println("aX = -16384 | aY = 16640 | aZ = 16384 | tmp = 36.53 | gX = -32768 | gY = 32767 | gZ = 32767");
    // Add any additional code you want to execute within the loop here
  }

 
  file.close();

  File file2 = SPIFFS.open("/test.txt");
 
    if(!file2){
        Serial.println("Failed to open file for reading");
        return;
    }
 
    Serial.println("File Content:");
 
    while(file2.available()){
 
        Serial.write(file2.read());
    }
 
    file2.close();
}
 
void loop() {}