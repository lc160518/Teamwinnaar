// Definieer de variabelen voor de Arduino aansluitingen
const int potPin = A0; // De potmeter is verbonden met analoge pin A0
const int escPin = 9; // De esc is verbonden met digitale pin 9 (PWM)
const int togglePin = 2; // De toggle-knop is verbonden met digitale pin 2

// Definieer de variabelen voor de potmeter en de esc
int potValue; // De waarde van de potmeter (0-1023)
int throttle; // De throttle waarde (0-100)
int escValue; // De esc waarde (1000-2000)

// Definieer de variabele voor de stroomtoestand van EDF
bool power = false; // De stroomtoestand van de EDF (true of false)

// Definieer de variabele voor de vorige status van de toggle-knop
int prevToggleState = HIGH;

void setup() {
  // Zet de pinnen als output of input
  pinMode(potPin, INPUT);
  pinMode(escPin, OUTPUT);
  pinMode(togglePin, INPUT_PULLUP); // Gebruik de interne pull-up weerstand voor de toggle-knop

  // Start de seriële communicatie met de console
  Serial.begin(9600);
}

void loop() {
  // Lees de status van de toggle-knop
  int toggleState = digitalRead(togglePin);

  // Controleer of de toggle-knop is ingedrukt en de vorige status niet hetzelfde is
  if (toggleState == LOW && prevToggleState == HIGH) {
    power = !power; // Toggle de stroomtoestand
    Serial.print("EDF is ");
    Serial.println(power ? "on" : "off"); // Toon de nieuwe toestand in de console
  }

  // Bewaar de huidige status van de toggle-knop voor de volgende iteratie
  prevToggleState = toggleState;

  // Als de stroomtoestand true is, lees dan de waarde van de potmeter en zet het om naar een throttle waarde (0-100)
  if (power) {
    potValue = analogRead(potPin);
    throttle = map(potValue, 0, 1023, 0, 100);

    // Zet de throttle waarde om naar een esc waarde (1000-2000)
    escValue = map(throttle, 0, 100, 1000, 2000);

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
  } else {
    // Als de stroomtoestand false is, stuur dan een esc waarde van 1000 naar de esc pin om de EDF uit te zetten
    digitalWrite(escPin, HIGH);
    delayMicroseconds(1000);
    digitalWrite(escPin, LOW);
    delayMicroseconds(19000);
  }
}
