/**
 * @file health_data.h
 * @brief Health data management
 */

#ifndef HEALTH_DATA_H
#define HEALTH_DATA_H

#include "dashboard/dashboard_types.h"

#ifdef __cplusplus
extern "C" {
#endif

// Initialize health data
void health_data_init(void);

// Get current sensor readings
sensor_readings_t* health_data_get_readings(void);

// Get profile data
profile_data_t* health_data_get_profile(void);

// Generate random readings
void health_data_generate_random_readings(void);

// Save current readings to profile
void health_data_save_readings(void);

// Update profile data
void health_data_update_profile(const profile_data_t* data);

#ifdef __cplusplus
}
#endif

#endif /* HEALTH_DATA_H */