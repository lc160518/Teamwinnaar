// Definieer de variabelen voor de Arduino aansluitingen
int potPin = 12;
int escPin = 19;


// Definieer de variabelen voor de potmeter en de esc
int potValue; // De waarde van de potmeter (0-1023)
int throttle; // De throttle waarde (0-100)
int escValue; // De esc waarde (1000-2000)

void setup() {
  // Zet de pinnen als output of input
  pinMode(potPin, INPUT);
  pinMode(escPin, OUTPUT);

  // Start de seriële communicatie met de console
  Serial.begin(115200);
}

void loop() {


  // Als de stroomtoestand true is, lees dan de waarde van de potmeter en zet het om naar een throttle waarde (0-100)
     potValue = analogRead(potPin);
    throttle = map(potValue, 0, 1023, 0, 100);
    throttle = constrain(throttle, 0, 100);

    // Zet de throttle waarde om naar een esc waarde (1000-2000)
    escValue = map(throttle, 0, 100, 1000, 2000);
    escValue = constrain(escValue, 1000, 2000);

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

    // Als de stroomtoestand false is, stuur dan een esc waarde van 1000 naar de esc pin om de EDF uit te zetten
    
  delayMicroseconds(10000);
}