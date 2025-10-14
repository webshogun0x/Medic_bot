#ifndef ANALYTICS_SCREEN_H
#define ANALYTICS_SCREEN_H

#include "lvgl.h"

void analytics_screen_create(lv_obj_t * parent);
void analytics_update_readings(float bmi, float temp, uint8_t hr, uint8_t spo2);

#endif // ANALYTICS_SCREEN_H