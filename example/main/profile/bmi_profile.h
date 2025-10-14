/**
 * @file profile_screen.h
 * @brief Profile screen interface
 */

#ifndef BMI_PROFILE_H
#define BMI_PROFILE_H

#include "lvgl.h"
#include "dashboard/dashboard_types.h"

#ifdef __cplusplus
extern "C" {
#endif

// Create profile screen
void bmi_profile_create(lv_obj_t* parent);

// Update profile screen with new data
void bmi_profile_update(const profile_data_t* profile, const sensor_readings_t* readings);

#ifdef __cplusplus
}
#endif

#endif /* BMI_PROFILE_H */