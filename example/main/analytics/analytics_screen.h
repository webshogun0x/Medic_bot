/**
 * @file analytics_screen.h
 * @brief Analytics screen interface
 */

#ifndef ANALYTICS_SCREEN_H
#define ANALYTICS_SCREEN_H

#include "lvgl.h"
#include "dashboard/dashboard_types.h"

#ifdef __cplusplus
extern "C" {
#endif

// Create analytics screen
void analytics_screen_create(lv_obj_t* parent);

// Update analytics screen with new readings
void analytics_screen_update(const sensor_readings_t* readings);

// Set button event callbacks
void analytics_screen_set_read_callback(lv_event_cb_t callback);
void analytics_screen_set_save_callback(lv_event_cb_t callback);

#ifdef __cplusplus
}
#endif

#endif /* ANALYTICS_SCREEN_H */