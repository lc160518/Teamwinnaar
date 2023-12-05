#include <WiFi.h>
#include <ESPAsyncWebSrv.h>

const char *ssid = "ESP32-Network"; // SSID van het WiFi-netwerk dat de ESP32 uitzendt
const char *password = "1test5"; // Wachtwoord voor het WiFi-netwerk

const int ledPin = 2; // Pin voor de LED

bool ledState = LOW;

AsyncWebServer server(80);

void setup() {
  pinMode(ledPin, OUTPUT);

  // Start de seriële communicatie voor het debuggen
  Serial.begin(115200);

  // Maak verbinding met het WiFi-netwerk
  WiFi.softAP(ssid, password);
  Serial.println("ESP32 WiFi-netwerk is opgezet");

  // Toon het IP-adres van de ESP32 op de seriële monitor
  Serial.print("IP-adres: ");
  Serial.println(WiFi.softAPIP());

  // Definieer de route voor de webpagina en de bijbehorende functie
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
    String html = "<html><body>";
    html += "<h1>LED-bediening</h1>";
    html += "<p><a href='/on'><button>Aan</button></a></p>";
    html += "<p><a href='/off'><button>Uit</button></a></p>";
    html += "</body></html>";
    request->send(200, "text/html", html);
  });

  // Schakel de LED in
  server.on("/on", HTTP_GET, [](AsyncWebServerRequest *request){
    digitalWrite(ledPin, HIGH);
    request->send(200, "text/plain", "LED is ingeschakeld");
  });

  // Schakel de LED uit
  server.on("/off", HTTP_GET, [](AsyncWebServerRequest *request){
    digitalWrite(ledPin, LOW);
    request->send(200, "text/plain", "LED is uitgeschakeld");
  });

  // Start de webserver
  server.begin();
}

void loop() {
  // Doe andere taken hier indien nodig
}