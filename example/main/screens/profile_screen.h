#ifndef PROFILE_SCREEN_H
#define PROFILE_SCREEN_H

#include "lvgl.h"
#include "lv_demo_bmi_dashboard.h"

void profile_screen_init(lv_obj_t * parent);
void profile_update_data(const profile_data_t * data);
void profile_get_data(profile_data_t * data);

#endif // PROFILE_SCREEN_H