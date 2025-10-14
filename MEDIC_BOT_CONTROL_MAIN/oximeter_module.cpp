#include "oximeter_module.h"

MAX30105 particleSensor;
uint32_t irBuffer[100];
uint32_t redBuffer[100];
int32_t bufferLength, spo2, heartRate;
int8_t validSPO2, validHeartRate;
double user_hr = 80, user_sp02 = 98;
String hrStatus, sp02Status;

void initOximeter() {
  if (!particleSensor.begin(Wire, I2C_SPEED_FAST)) {
    Serial.println(F("MAX30105 was not found. Please check wiring/power."));
    while (1);
  }
  
  byte ledBrightness = 60;
  byte sampleAverage = 4;
  byte ledMode = 2;
  byte sampleRate = 100;
  int pulseWidth = 411;
  int adcRange = 4096;
  
  particleSensor.enableDIETEMPRDY();
  particleSensor.setup(ledBrightness, sampleAverage, ledMode, sampleRate, pulseWidth, adcRange);
}

void readOximeter() {
  float temperature = 0;
  float temperatureF = 0;

  long irThreshold = 15000;
  long irValue = particleSensor.getIR();

  if (irValue > irThreshold) {
    temperature = particleSensor.readTemperature();
    temperatureF = particleSensor.readTemperatureF();

    bufferLength = 100;
    for (byte i = 0; i < bufferLength; i++) {
      while (particleSensor.available() == false)
        particleSensor.check();

      redBuffer[i] = particleSensor.getRed();
      irBuffer[i] = particleSensor.getIR();
      particleSensor.nextSample();
      Serial.print(F("red=")); Serial.print(redBuffer[i], DEC);
      Serial.print(F(", ir=")); Serial.println(irBuffer[i], DEC);
    }

    maxim_heart_rate_and_oxygen_saturation(irBuffer, bufferLength, redBuffer, &spo2, &validSPO2, &heartRate, &validHeartRate);
    for (byte i = 25; i < 100; i++) {
      redBuffer[i - 25] = redBuffer[i];
      irBuffer[i - 25] = irBuffer[i];
    }

    for (byte i = 75; i < 100; i++) {
      while (particleSensor.available() == false)
        particleSensor.check();

      digitalWrite(READ_LED, !digitalRead(READ_LED));
      redBuffer[i] = particleSensor.getRed();
      irBuffer[i] = particleSensor.getIR();
      particleSensor.nextSample();

      Serial.print(F("red=")); Serial.print(redBuffer[i], DEC);
      Serial.print(F(", ir=")); Serial.print(irBuffer[i], DEC);
      Serial.print(F(", HR=")); Serial.print(heartRate, DEC);
      Serial.print(F(", HRvalid=")); Serial.print(validHeartRate, DEC);
      Serial.print(F(", SPO2=")); Serial.print(spo2, DEC);
      Serial.print(F(", SPO2Valid=")); Serial.print(validSPO2, DEC);
      Serial.print(", temperatureC="); Serial.print(temperature, 4);
      Serial.print(", temperatureF="); Serial.print(temperatureF, 4);
      Serial.println();
    }

    maxim_heart_rate_and_oxygen_saturation(irBuffer, bufferLength, redBuffer, &spo2, &validSPO2, &heartRate, &validHeartRate);
    
    user_hr = heartRate;
    user_sp02 = spo2;

    // HR Status mapping
    if (!isnan(user_hr)) {
      if (user_hr < 60) hrStatus = "SLOW";
      else if (user_hr < 100) hrStatus = "NORM";
      else if (user_hr < 160) hrStatus = "FAST";
      else hrStatus = "EXTR";
    } else hrStatus = "NIL";

    // SPO2 Status mapping
    if (!isnan(user_sp02)) {
      if (user_sp02 > 95) sp02Status = "NORM";
      else if (user_sp02 > 90) sp02Status = "MILD";
      else if (user_sp02 > 85) sp02Status = "MHYP";
      else sp02Status = "SHYP";
    } else sp02Status = "NIL";
  }
}