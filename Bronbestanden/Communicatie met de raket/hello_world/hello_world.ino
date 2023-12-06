#include <WiFi.h>
#include <ESPAsyncWebServer.h>

const char* ssid = "JULIAN";
const char* password = "12345678";

AsyncWebServer server(80);

String message = "Hello World";

String getMessage() {
  message += "\ne";
  return message;
}

void setup() {
  Serial.begin(115200);

  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }
  Serial.println(WiFi.localIP());

  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(200, "text/html", "<html><body><script>setInterval(function(){fetch('/message').then(response => response.text()).then(data => document.getElementById('message').innerText = data);}, 1000);</script><h1 id='message'></h1></body></html>");
  });

  server.on("/message", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(200, "text/plain", getMessage());
  });

  server.begin();
}

void loop() {
  // put your main code here, to run repeatedly:
}