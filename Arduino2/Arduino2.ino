#include <Wire.h>
#include <DFRobot_BMI160.h>

// PIN sensori
const int pinLM35 = A0;
const int pinPIR  = 2;

// PIN LED
const int pinLED = 6;

// PIN interruttori
const int pinSwitchTemp = 3;
const int pinSwitchPIR  = 4;
const int pinSwitchIMU  = 5;

// Oggetto BMI160
DFRobot_BMI160 bmi160;
int16_t accel[3];
int16_t gyro[3];

// Timer
unsigned long lastRead = 0;
const unsigned long interval = 3000;

void setup() {
  Serial.begin(9600);
  while (!Serial) {}

  pinMode(pinPIR, INPUT);
  pinMode(pinLED, OUTPUT);

  pinMode(pinSwitchTemp, INPUT_PULLUP);
  pinMode(pinSwitchPIR,  INPUT_PULLUP);
  pinMode(pinSwitchIMU,  INPUT_PULLUP);

  // I2C
  Wire.begin();

  // Inizializzazione BMI160
  if (bmi160.softReset() != BMI160_OK) {
    Serial.println("Errore reset BMI160");
  }

  if (bmi160.I2cInit(0x68) != BMI160_OK) {
    Serial.println("BMI160 non trovato");
  } else {
    Serial.println("BMI160 pronto");
  }

  Serial.println("Sistema avviato.");
}

void loop() {

  bool readTemp = (digitalRead(pinSwitchTemp) == LOW);
  bool readPIR  = (digitalRead(pinSwitchPIR)  == LOW);
  bool readIMU  = (digitalRead(pinSwitchIMU)  == LOW);

  // Nessun sensore attivo
  if (!readTemp && !readPIR && !readIMU) {
    digitalWrite(pinLED, LOW);
    return;
  }

  unsigned long now = millis();
  if (now - lastRead >= interval) {
    lastRead = now;

    // --- LM35 ---
    if (readTemp) {
      int raw = analogRead(pinLM35);
      float voltage = raw * 3.3 / 1023.0;
      float temperature = voltage * 100.0;

      Serial.print("Temperatura: ");
      Serial.print(temperature);
      Serial.println(" °C");
    }

    // --- PIR ---
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

    // --- BMI160 IMU ---
    if (readIMU) {
      bmi160.getAccelData(accel);
      bmi160.getGyroData(gyro);

      Serial.println("BMI160 IMU:");
      Serial.print("Accel X: "); Serial.print(accel[0]);
      Serial.print(" Y: "); Serial.print(accel[1]);
      Serial.print(" Z: "); Serial.println(accel[2]);

      Serial.print("Gyro  X: "); Serial.print(gyro[0]);
      Serial.print(" Y: "); Serial.print(gyro[1]);
      Serial.print(" Z: "); Serial.println(gyro[2]);
    }

    Serial.println("-------------------------");
  }
}
