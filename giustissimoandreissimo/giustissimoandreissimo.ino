// PIN sensori
const int pinLM35 = A0;
const int pinPIR  = 2;

// PIN LED
const int pinLED = 6;

// PIN interruttori
const int pinSwitch1 = 3;   // Se su → leggi temperatura
const int pinSwitch2 = 4;   // Se su → leggi PIR

// Variabili temporali
unsigned long lastRead = 0;
const unsigned long interval = 3000; // 3 secondi

void setup() {
  Serial.begin(9600);
  while (!Serial) {}

  pinMode(pinLM35, INPUT);
  pinMode(pinPIR, INPUT);
  pinMode(pinLED, OUTPUT);

  pinMode(pinSwitch1, INPUT_PULLUP);
  pinMode(pinSwitch2, INPUT_PULLUP);

  Serial.println("Sistema avviato.");
}

void loop() {

  bool readTemp = (digitalRead(pinSwitch1) == LOW); // D3 su
  bool readPIR  = (digitalRead(pinSwitch2) == LOW); // D4 su

  // Se nessuno è su → non leggere nulla
  if (!readTemp && !readPIR) {
    digitalWrite(pinLED, LOW);
    return;
  }

  unsigned long now = millis();
  if (now - lastRead >= interval) {
    lastRead = now;

    // --- Lettura temperatura ---
    if (readTemp) {
      int raw = analogRead(pinLM35);
      float voltage = raw * 3.3 / 1023.0;
      float temperature = voltage * 100.0;

      Serial.print("Temperatura: ");
      Serial.print(temperature);
      Serial.println(" °C");
    }

    // --- Lettura PIR ---
    if (readPIR) {
      int motion = digitalRead(pinPIR);
      Serial.print("Movimento: ");

      if (motion == HIGH) {
        Serial.println("RILEVATO");
        digitalWrite(pinLED, HIGH);
      } else {
        Serial.println("Nessun movimento");
        digitalWrite(pinLED, LOW);
      }
    }

    Serial.println("-------------------------");
  }
}