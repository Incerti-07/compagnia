#include "FastIMU.h"
#include <Wire.h>

#define IMU_ADDRESS 0x68   // SDO collegato a GND

MPU6050 IMU;

calData calib = {0};
AccelData accelData;
GyroData gyroData;

// PIN
const int pinLM35 = A0;
const int pinPIR  = 2;
const int pinLED  = 6;

const int pinSwitchTemp = 3;
const int pinSwitchPIR  = 4;

// Timer
unsigned long lastRead = 0;
const unsigned long interval = 3000;

void setup() {
  Serial.begin(9600);
  Wire.begin();
  Wire.setClock(400000);

  pinMode(pinPIR, INPUT);
  pinMode(pinLED, OUTPUT);

  pinMode(pinSwitchTemp, INPUT_PULLUP);
  pinMode(pinSwitchPIR,  INPUT_PULLUP);

  int err = IMU.init(calib, IMU_ADDRESS);
  if (err != 0) {
    Serial.print("Errore IMU: ");
    Serial.println(err);
    while (1);
  }

  Serial.println("Sistema avviato.");
}

void loop() {

  bool readTemp = (digitalRead(pinSwitchTemp) == LOW);
  bool readPIR  = (digitalRead(pinSwitchPIR)  == LOW);

  unsigned long now = millis();
  if (now - lastRead >= interval) {
    lastRead = now;

    // ----- LM35 -----
    if (readTemp) {
      int raw = analogRead(pinLM35);
      float voltage = raw * 3.3 / 1023.0;
      float temperature = voltage * 100.0;

      Serial.print("Temperatura: ");
      Serial.print(temperature);
      Serial.println(" °C");
    }

    // ----- PIR -----
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

    // ----- BMI160 -----
    IMU.update();
    IMU.getAccel(&accelData);
    IMU.getGyro(&gyroData);

    Serial.println("BMI160 IMU:");
    Serial.print("Accel X: "); Serial.print(accelData.accelX);
    Serial.print(" Y: "); Serial.print(accelData.accelY);
    Serial.print(" Z: "); Serial.println(accelData.accelZ);

    Serial.print("Gyro  X: "); Serial.print(gyroData.gyroX);
    Serial.print(" Y: "); Serial.print(gyroData.gyroY);
    Serial.print(" Z: "); Serial.println(gyroData.gyroZ);

    Serial.println("-------------------------");
  }
}
