#ifndef OXIMETER_MODULE_H
#define OXIMETER_MODULE_H

#include <Wire.h>
#include "MAX30105.h"
#include "spo2_algorithm.h"
#include "config.h"

extern MAX30105 particleSensor;
extern uint32_t irBuffer[100];
extern uint32_t redBuffer[100];
extern int32_t bufferLength, spo2, heartRate;
extern int8_t validSPO2, validHeartRate;
extern double user_hr, user_sp02;
extern String hrStatus, sp02Status;

void initOximeter();
void readOximeter();

#endif