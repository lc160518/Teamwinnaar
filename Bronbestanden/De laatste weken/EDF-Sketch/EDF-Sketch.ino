#include <WiFi.h>
#include <WebServer.h>

const char *ssid = "ESP32-L";       // Enter your WiFi SSID
const char *password = "12345678"; // Enter your WiFi Password

WebServer server(80);

int sliderValue = 0; // Initial value
int escValue= 1000;

int escPin = 19; // PWM pin for the ESC

void handleRoot()
{
  String page = "<!DOCTYPE html>\n";
  page += "<html>\n";
  page += "<head><title>ESP32 Slider Control</title></head>\n";
  page += "<body>\n";
  page += "<h1>ESP32 Slider Control</h1>\n";
  page += "<input type=\"range\" min=\"0\" max=\"100\" value=\"" + String(sliderValue) + "\" class=\"slider\" id=\"myRange\" name=\"value\" style=\"width: 80%;\">\n"; // Adjusted size here
  page += "<p>Value: <span id=\"demo\">" + String(sliderValue) + "</span></p>\n";
  page += "<button onclick=\"setToZero()\" style=\"font-size: 30px; padding: 20px;\">ABORT</button>\n"; // Larger button with increased font size and padding
  page += "<script>\n";
  page += "var slider = document.getElementById(\"myRange\");\n";
  page += "var output = document.getElementById(\"demo\");\n";
  page += "var requestInProgress = false;\n"; // Flag to track if a request is in progress
  page += "output.innerHTML = slider.value;\n";
  page += "slider.oninput = function() {\n";
  page += "  output.innerHTML = this.value;\n";
  page += "  if (!requestInProgress) {\n"; // Only send request if no request is in progress
  page += "    updateSlider(this.value);\n";
  page += "  }\n";
  page += "}\n";
  page += "function updateSlider(val) {\n";
  page += "  requestInProgress = true;\n"; // Set flag to indicate a request is in progress
  page += "  var xhttp = new XMLHttpRequest();\n";
  page += "  xhttp.onreadystatechange = function() {\n";
  page += "    if (this.readyState == 4 && this.status == 200) {\n";
  page += "      console.log('Value updated');\n";
  page += "      requestInProgress = false;\n"; // Reset flag when request is complete
  page += "    }\n";
  page += "  };\n";
  page += "  xhttp.open(\"GET\", \"/update?value=\" + val, true);\n";
  page += "  xhttp.send();\n";
  page += "}\n";
  page += "function setToZero() {\n"; // Function to set value to 0
  page += "  slider.value = 0;\n";
  page += "  output.innerHTML = 0;\n";
  page += "  updateSlider(0);\n";
  page += "}\n";
  page += "</script>\n";
  page += "</body>\n";
  page += "</html>\n";
  server.send(200, "text/html", page);
}

void handleUpdate()
{
  if (server.hasArg("value"))
  {
    String valueStr = server.arg("value");
    sliderValue = valueStr.toInt();
    Serial.print("Slider Value: ");
    Serial.println(sliderValue); // Print slider value to Serial Monitor
    escValue = map(sliderValue, 0, 100, 1000, 2000);
    constrain(escValue, 1000, 2000);
    Serial.print("ESC Value: ");
    Serial.println(escValue); // Print ESC value to Serial Monitor
  }
  server.send(200, "text/plain", "OK");
}

void setup()
{
 // pinMode(escPin, OUTPUT);
  
  Serial.begin(115200);
  delay(10);

  Serial.println();
  Serial.println();
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
  Serial.println(WiFi.localIP());

  server.on("/", HTTP_GET, handleRoot);
  server.on("/update", HTTP_GET, handleUpdate);

  server.begin();
  Serial.println("HTTP server started");

  delay(2000);
}

void loop()
{
  server.handleClient();

// Stuur de esc waarde naar de esc pin met een pulsduur van escValue microseconden
  server.handleClient();

// Stuur de esc waarde naar de esc pin met een pulsduur van escValue microseconden
  digitalWrite(escPin, HIGH);
  delayMicroseconds(escValue);
  digitalWrite(escPin, LOW);
  delayMicroseconds(20000 - escValue);

  // Toon de throttle waarde op de console
  Serial.print("Throttle: ");
  Serial.print(throttle);
  Serial.println("%");
  Serial.println(escValue);

  delayMicroseconds(10000);
}