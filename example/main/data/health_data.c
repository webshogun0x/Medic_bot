/**
 * @file health_data.c
 * @brief Health data management implementation
 */

#include "health_data.h"
#include <stdlib.h>
#include <string.h>

static sensor_readings_t current_readings = {
    .bmi = 24.8f,
    .temperature = 98.6f,
    .heart_rate = 72,
    .spo2 = 98
};

static profile_data_t current_profile = {
    .name = "Oluwatimileyin",
    .gender = "Male",
    .age = 35,
    .weight = 180.0f,
    .height = 5.9f,
    .bmi_data = {18.5f, 24.2f, 26.8f, 23.1f, 19.7f, 27.3f, 21.4f, 25.6f, 22.9f, 28.1f, 20.3f, 24.8f},
    .heart_rate = 72
};

void health_data_init(void)
{
    // Initialize data if needed
}

sensor_readings_t* health_data_get_readings(void)
{
    return &current_readings;
}

profile_data_t* health_data_get_profile(void)
{
    return &current_profile;
}

void health_data_generate_random_readings(void)
{
    // Generate random BMI (18.5 - 35.0)
    current_readings.bmi = 18.5f + ((float)rand() / RAND_MAX) * (35.0f - 18.5f);
    
    // Generate random temperature (96.0 - 102.0°F)
    current_readings.temperature = 96.0f + ((float)rand() / RAND_MAX) * (102.0f - 96.0f);
    
    // Generate random heart rate (60 - 120 BPM)
    current_readings.heart_rate = 60 + (rand() % 61);
    
    // Generate random SpO2 (90 - 100%)
    current_readings.spo2 = 90 + (rand() % 11);
}

void health_data_save_readings(void)
{
    // Save current readings to profile
    current_profile.heart_rate = current_readings.heart_rate;
    
    // Add current BMI to history (shift array left and add new value)
    for (int i = 0; i < 11; i++) {
        current_profile.bmi_data[i] = current_profile.bmi_data[i + 1];
    }
    current_profile.bmi_data[11] = current_readings.bmi;
}

void health_data_update_profile(const profile_data_t* data)
{
    if (data) {
        memcpy(&current_profile, data, sizeof(profile_data_t));
    }
}