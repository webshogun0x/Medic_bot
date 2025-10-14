/**
 * @file dashboard_types.h
 * @brief Common data types for BMI dashboard
 */

#ifndef DASHBOARD_TYPES_H
#define DASHBOARD_TYPES_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

// Current sensor readings structure
typedef struct {
    float bmi;
    float temperature;
    uint8_t heart_rate;
    uint8_t spo2;
} sensor_readings_t;

// Profile data structure
typedef struct {
    char name[32];
    char gender[8];
    uint8_t age;
    float weight;
    float height;
    float bmi_data[12];
    uint8_t heart_rate;
} profile_data_t;

#ifdef __cplusplus
}
#endif

#endif /* DASHBOARD_TYPES_H */