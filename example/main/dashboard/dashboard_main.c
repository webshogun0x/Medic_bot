/**
 * @file dashboard_main.c
 * @brief Main dashboard implementation
 */

#include "dashboard_main.h"
#include "analytics/analytics_screen.h"
#include "profile/bmi_profile.h"
#include "data/health_data.h"

static void read_button_event_cb(lv_event_t * e);
static void save_button_event_cb(lv_event_t * e);

void dashboard_main_create(void)
{
    // Initialize health data
    health_data_init();
    
    // Create tabview for navigation
    lv_obj_t * tv = lv_tabview_create(lv_screen_active());
    lv_tabview_set_tab_bar_size(tv, 60);
    
    // Create tabs
    lv_obj_t * profile_tab = lv_tabview_add_tab(tv, "Profile");
    lv_obj_t * analytics_tab = lv_tabview_add_tab(tv, "Analytics");
    
    // Create screens
    analytics_screen_create(analytics_tab);
    analytics_screen_set_read_callback(read_button_event_cb);
    analytics_screen_set_save_callback(save_button_event_cb);
    
    bmi_profile_create(profile_tab);
    
    // Initial update
    bmi_profile_update(health_data_get_profile(), health_data_get_readings());
    analytics_screen_update(health_data_get_readings());
}

static void read_button_event_cb(lv_event_t * e)
{
    // Generate random readings
    health_data_generate_random_readings();
    
    // Update displays
    analytics_screen_update(health_data_get_readings());
    bmi_profile_update(health_data_get_profile(), health_data_get_readings());
}

static void save_button_event_cb(lv_event_t * e)
{
    // Save current readings to profile
    health_data_save_readings();
    
    // Update displays
    bmi_profile_update(health_data_get_profile(), health_data_get_readings());
}