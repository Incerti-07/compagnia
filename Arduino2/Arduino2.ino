#include "FastIMU.h"
#include <Wire.h>

#define IMU_ADDRESS 0x68

BMI160 IMU;

calData calib = {0};
AccelData accelData;
GyroData gyroData;

// PIN
const int pinLM35 = A0;
const int pinPIR  = 2;
const int pinLED  = 6;

const int pinSwitchTemp = 3;
const int pinSwitchPIR  = 4;
const int pinSwitchIMU  = 5;

// STATI
bool enableIMU = false;
bool enablePIR = false;

// TIMER
unsigned long lastRead = 0;
const unsigned long interval = 3000; // 3 secondi

void setup() {
  Serial.begin(9600);
  Wire.begin();

  pinMode(pinPIR, INPUT);      // PIR CORRETTO
  pinMode(pinLED, OUTPUT);

  pinMode(pinSwitchTemp, INPUT_PULLUP);
  pinMode(pinSwitchPIR, INPUT_PULLUP);
  pinMode(pinSwitchIMU, INPUT_PULLUP);

  IMU.init(calib, IMU_ADDRESS);

  Serial.println("SISTEMA AVVIATO - ATTENDI WARMUP PIR");
}

void loop() {
  unsigned long now = millis();

  // LETTURA INTERRUTTORI
  bool readTemp = (digitalRead(pinSwitchTemp) == LOW);
  enablePIR     = (digitalRead(pinSwitchPIR) == HIGH);
  enableIMU     = (digitalRead(pinSwitchIMU) == LOW);

  // ----- PIR: LED SEGUE SUBITO IL MOVIMENTO -----
  int motion = digitalRead(pinPIR);
  if (motion == HIGH) {
    digitalWrite(pinLED, HIGH);
  } else {
    digitalWrite(pinLED, LOW);
  }

  // ----- STAMPE OGNI 3 SECONDI -----
  if (now - lastRead >= interval) {
    lastRead = now;

    // LM35
    if (readTemp) {
      int raw = analogRead(pinLM35);
      float voltage = raw * 3.3 / 1023.0;
      float temperature = voltage * 100.0;

      Serial.print("TEMPERATURA: ");
      Serial.print(temperature);
      Serial.println(" C");
    }

    // PIR
    if (enablePIR) {
      if (motion == HIGH) {
        Serial.println("RILEVATO");
      } else {
        Serial.println("NESSUN MOVIMENTO");
      }
    } else {
      Serial.println("PIR DISABILITATO");
    }

    // BMI160
    if (enableIMU) {
      IMU.update();
      IMU.getAccel(&accelData);
      IMU.getGyro(&gyroData);

      Serial.println("BMI160 IMU:");
      Serial.print("ACCEL X: "); Serial.print(accelData.accelX);
      Serial.print(" Y: "); Serial.print(accelData.accelY);
      Serial.print(" Z: "); Serial.println(accelData.accelZ);

      Serial.print("GYRO X: "); Serial.print(gyroData.gyroX);
      Serial.print(" Y: "); Serial.print(gyroData.gyroY);
      Serial.print(" Z: "); Serial.println(gyroData.gyroZ);
    }

    Serial.println("-------------------------");
  }
}
