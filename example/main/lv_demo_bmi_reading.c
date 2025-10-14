#include "lv_demo_bmi_reading.h"
#include <stdio.h>
#include <stdlib.h>

static lv_obj_t *main_screen;
static lv_obj_t *profile_screen;

// Profile data
static char profile_name[32] = "John Doe";
static int profile_age = 25;
static int profile_height = 175; // cm

// Sensor readings
static float bmi_value = 0.0;
static int spo2_value = 0;
static int hr_value = 0;
static float temp_value = 0.0;

// Widgets
static lv_obj_t *bmi_label, *spo2_label, *hr_label, *temp_label;
static lv_obj_t *profile_name_label, *profile_age_label, *profile_height_label;
static lv_obj_t *name_ta, *age_ta, *height_ta;

// Generate random readings
static void generate_random_readings(void)
{
    bmi_value = 18.5 + (float)(rand() % 150) / 10.0; // 18.5 - 33.5
    spo2_value = 95 + (rand() % 6); // 95-100%
    hr_value = 60 + (rand() % 40); // 60-100 bpm
    temp_value = 36.0 + (float)(rand() % 40) / 10.0; // 36.0-40.0°C
}

// Update display with current readings
static void update_readings_display(void)
{
    char buf[64];
    
    snprintf(buf, sizeof(buf), "BMI: %.1f", bmi_value);
    lv_label_set_text(bmi_label, buf);
    
    snprintf(buf, sizeof(buf), "SpO2: %d%%", spo2_value);
    lv_label_set_text(spo2_label, buf);
    
    snprintf(buf, sizeof(buf), "HR: %d bpm", hr_value);
    lv_label_set_text(hr_label, buf);
    
    snprintf(buf, sizeof(buf), "Temp: %.1f°C", temp_value);
    lv_label_set_text(temp_label, buf);
}

// Update profile display
static void update_profile_display(void)
{
    char buf[64];
    
    lv_label_set_text(profile_name_label, profile_name);
    
    snprintf(buf, sizeof(buf), "Age: %d", profile_age);
    lv_label_set_text(profile_age_label, buf);
    
    snprintf(buf, sizeof(buf), "Height: %d cm", profile_height);
    lv_label_set_text(profile_height_label, buf);
}

// Button event handlers
static void read_btn_event_cb(lv_event_t *e)
{
    generate_random_readings();
    update_readings_display();
}

static void save_readings_btn_event_cb(lv_event_t *e)
{
    // Save current readings (placeholder)
    lv_obj_t *mbox = lv_msgbox_create(NULL);
    lv_msgbox_add_title(mbox, "Saved");
    lv_msgbox_add_text(mbox, "Readings saved to profile!");
    lv_msgbox_add_close_button(mbox);
}

static void profile_btn_event_cb(lv_event_t *e)
{
    lv_screen_load(profile_screen);
}

static void save_profile_btn_event_cb(lv_event_t *e)
{
    // Get values from text areas
    const char *name = lv_textarea_get_text(name_ta);
    const char *age_str = lv_textarea_get_text(age_ta);
    const char *height_str = lv_textarea_get_text(height_ta);
    
    // Update profile data
    snprintf(profile_name, sizeof(profile_name), "%s", name);
    profile_age = atoi(age_str);
    profile_height = atoi(height_str);
    
    update_profile_display();
    
    lv_obj_t *mbox = lv_msgbox_create(NULL);
    lv_msgbox_add_title(mbox, "Profile Updated");
    lv_msgbox_add_text(mbox, "Profile saved successfully!");
    lv_msgbox_add_close_button(mbox);
}

static void back_btn_event_cb(lv_event_t *e)
{
    lv_screen_load(main_screen);
}

// Create main screen with readings
static void create_main_screen(void)
{
    main_screen = lv_obj_create(NULL);
    
    // Title
    lv_obj_t *title = lv_label_create(main_screen);
    lv_label_set_text(title, "Health Monitor");
    lv_obj_set_style_text_font(title, &lv_font_montserrat_16, 0);
    lv_obj_align(title, LV_ALIGN_TOP_MID, 0, 10);
    
    // Profile info panel
    lv_obj_t *profile_panel = lv_obj_create(main_screen);
    lv_obj_set_size(profile_panel, 350, 80);
    lv_obj_align(profile_panel, LV_ALIGN_TOP_LEFT, 10, 50);
    
    profile_name_label = lv_label_create(profile_panel);
    lv_obj_align(profile_name_label, LV_ALIGN_TOP_LEFT, 10, 5);
    
    profile_age_label = lv_label_create(profile_panel);
    lv_obj_align(profile_age_label, LV_ALIGN_TOP_LEFT, 10, 25);
    
    profile_height_label = lv_label_create(profile_panel);
    lv_obj_align(profile_height_label, LV_ALIGN_TOP_LEFT, 10, 45);
    
    // BMI Widget
    lv_obj_t *bmi_panel = lv_obj_create(main_screen);
    lv_obj_set_size(bmi_panel, 180, 100);
    lv_obj_align(bmi_panel, LV_ALIGN_TOP_LEFT, 10, 150);
    lv_obj_set_style_bg_color(bmi_panel, lv_color_hex(0x4CAF50), 0);
    
    lv_obj_t *bmi_title = lv_label_create(bmi_panel);
    lv_label_set_text(bmi_title, "BMI");
    lv_obj_set_style_text_color(bmi_title, lv_color_white(), 0);
    lv_obj_align(bmi_title, LV_ALIGN_TOP_MID, 0, 10);
    
    bmi_label = lv_label_create(bmi_panel);
    lv_label_set_text(bmi_label, "BMI: --");
    lv_obj_set_style_text_color(bmi_label, lv_color_white(), 0);
    lv_obj_set_style_text_font(bmi_label, &lv_font_montserrat_16, 0);
    lv_obj_align(bmi_label, LV_ALIGN_CENTER, 0, 10);
    
    // SpO2 Widget
    lv_obj_t *spo2_panel = lv_obj_create(main_screen);
    lv_obj_set_size(spo2_panel, 180, 100);
    lv_obj_align(spo2_panel, LV_ALIGN_TOP_RIGHT, -10, 150);
    lv_obj_set_style_bg_color(spo2_panel, lv_color_hex(0x2196F3), 0);
    
    lv_obj_t *spo2_title = lv_label_create(spo2_panel);
    lv_label_set_text(spo2_title, "SpO2");
    lv_obj_set_style_text_color(spo2_title, lv_color_white(), 0);
    lv_obj_align(spo2_title, LV_ALIGN_TOP_MID, 0, 10);
    
    spo2_label = lv_label_create(spo2_panel);
    lv_label_set_text(spo2_label, "SpO2: --%");
    lv_obj_set_style_text_color(spo2_label, lv_color_white(), 0);
    lv_obj_set_style_text_font(spo2_label, &lv_font_montserrat_16, 0);
    lv_obj_align(spo2_label, LV_ALIGN_CENTER, 0, 10);
    
    // Heart Rate Widget
    lv_obj_t *hr_panel = lv_obj_create(main_screen);
    lv_obj_set_size(hr_panel, 180, 100);
    lv_obj_align(hr_panel, LV_ALIGN_TOP_LEFT, 10, 270);
    lv_obj_set_style_bg_color(hr_panel, lv_color_hex(0xFF5722), 0);
    
    lv_obj_t *hr_title = lv_label_create(hr_panel);
    lv_label_set_text(hr_title, "Heart Rate");
    lv_obj_set_style_text_color(hr_title, lv_color_white(), 0);
    lv_obj_align(hr_title, LV_ALIGN_TOP_MID, 0, 10);
    
    hr_label = lv_label_create(hr_panel);
    lv_label_set_text(hr_label, "HR: -- bpm");
    lv_obj_set_style_text_color(hr_label, lv_color_white(), 0);
    lv_obj_set_style_text_font(hr_label, &lv_font_montserrat_16, 0);
    lv_obj_align(hr_label, LV_ALIGN_CENTER, 0, 10);
    
    // Temperature Widget
    lv_obj_t *temp_panel = lv_obj_create(main_screen);
    lv_obj_set_size(temp_panel, 180, 100);
    lv_obj_align(temp_panel, LV_ALIGN_TOP_RIGHT, -10, 270);
    lv_obj_set_style_bg_color(temp_panel, lv_color_hex(0xFF9800), 0);
    
    lv_obj_t *temp_title = lv_label_create(temp_panel);
    lv_label_set_text(temp_title, "Temperature");
    lv_obj_set_style_text_color(temp_title, lv_color_white(), 0);
    lv_obj_align(temp_title, LV_ALIGN_TOP_MID, 0, 10);
    
    temp_label = lv_label_create(temp_panel);
    lv_label_set_text(temp_label, "Temp: --°C");
    lv_obj_set_style_text_color(temp_label, lv_color_white(), 0);
    lv_obj_set_style_text_font(temp_label, &lv_font_montserrat_16, 0);
    lv_obj_align(temp_label, LV_ALIGN_CENTER, 0, 10);
    
    // Buttons
    lv_obj_t *read_btn = lv_btn_create(main_screen);
    lv_obj_set_size(read_btn, 120, 50);
    lv_obj_align(read_btn, LV_ALIGN_BOTTOM_LEFT, 20, -20);
    lv_obj_add_event_cb(read_btn, read_btn_event_cb, LV_EVENT_CLICKED, NULL);
    lv_obj_t *read_btn_label = lv_label_create(read_btn);
    lv_label_set_text(read_btn_label, "READ");
    lv_obj_center(read_btn_label);
    
    lv_obj_t *save_btn = lv_btn_create(main_screen);
    lv_obj_set_size(save_btn, 120, 50);
    lv_obj_align(save_btn, LV_ALIGN_BOTTOM_MID, 0, -20);
    lv_obj_add_event_cb(save_btn, save_readings_btn_event_cb, LV_EVENT_CLICKED, NULL);
    lv_obj_t *save_btn_label = lv_label_create(save_btn);
    lv_label_set_text(save_btn_label, "SAVE");
    lv_obj_center(save_btn_label);
    
    lv_obj_t *profile_btn = lv_btn_create(main_screen);
    lv_obj_set_size(profile_btn, 120, 50);
    lv_obj_align(profile_btn, LV_ALIGN_BOTTOM_RIGHT, -20, -20);
    lv_obj_add_event_cb(profile_btn, profile_btn_event_cb, LV_EVENT_CLICKED, NULL);
    lv_obj_t *profile_btn_label = lv_label_create(profile_btn);
    lv_label_set_text(profile_btn_label, "PROFILE");
    lv_obj_center(profile_btn_label);
}

// Create profile edit screen
static void create_profile_screen(void)
{
    profile_screen = lv_obj_create(NULL);
    
    // Title
    lv_obj_t *title = lv_label_create(profile_screen);
    lv_label_set_text(title, "Edit Profile");
    lv_obj_set_style_text_font(title, &lv_font_montserrat_16, 0);
    lv_obj_align(title, LV_ALIGN_TOP_MID, 0, 20);
    
    // Name input
    lv_obj_t *name_label = lv_label_create(profile_screen);
    lv_label_set_text(name_label, "Name:");
    lv_obj_align(name_label, LV_ALIGN_TOP_LEFT, 50, 80);
    
    name_ta = lv_textarea_create(profile_screen);
    lv_obj_set_size(name_ta, 300, 40);
    lv_obj_align(name_ta, LV_ALIGN_TOP_LEFT, 150, 70);
    lv_textarea_set_text(name_ta, profile_name);
    lv_textarea_set_one_line(name_ta, true);
    
    // Age input
    lv_obj_t *age_label = lv_label_create(profile_screen);
    lv_label_set_text(age_label, "Age:");
    lv_obj_align(age_label, LV_ALIGN_TOP_LEFT, 50, 140);
    
    age_ta = lv_textarea_create(profile_screen);
    lv_obj_set_size(age_ta, 100, 40);
    lv_obj_align(age_ta, LV_ALIGN_TOP_LEFT, 150, 130);
    char age_buf[8];
    snprintf(age_buf, sizeof(age_buf), "%d", profile_age);
    lv_textarea_set_text(age_ta, age_buf);
    lv_textarea_set_one_line(age_ta, true);
    
    // Height input
    lv_obj_t *height_label = lv_label_create(profile_screen);
    lv_label_set_text(height_label, "Height (cm):");
    lv_obj_align(height_label, LV_ALIGN_TOP_LEFT, 50, 200);
    
    height_ta = lv_textarea_create(profile_screen);
    lv_obj_set_size(height_ta, 100, 40);
    lv_obj_align(height_ta, LV_ALIGN_TOP_LEFT, 150, 190);
    char height_buf[8];
    snprintf(height_buf, sizeof(height_buf), "%d", profile_height);
    lv_textarea_set_text(height_ta, height_buf);
    lv_textarea_set_one_line(height_ta, true);
    
    // Buttons
    lv_obj_t *save_profile_btn = lv_btn_create(profile_screen);
    lv_obj_set_size(save_profile_btn, 120, 50);
    lv_obj_align(save_profile_btn, LV_ALIGN_BOTTOM_LEFT, 50, -50);
    lv_obj_add_event_cb(save_profile_btn, save_profile_btn_event_cb, LV_EVENT_CLICKED, NULL);
    lv_obj_t *save_profile_label = lv_label_create(save_profile_btn);
    lv_label_set_text(save_profile_label, "SAVE");
    lv_obj_center(save_profile_label);
    
    lv_obj_t *back_btn = lv_btn_create(profile_screen);
    lv_obj_set_size(back_btn, 120, 50);
    lv_obj_align(back_btn, LV_ALIGN_BOTTOM_RIGHT, -50, -50);
    lv_obj_add_event_cb(back_btn, back_btn_event_cb, LV_EVENT_CLICKED, NULL);
    lv_obj_t *back_label = lv_label_create(back_btn);
    lv_label_set_text(back_label, "BACK");
    lv_obj_center(back_label);
}

void lv_demo_bmi_reading(void)
{
    create_main_screen();
    create_profile_screen();
    
    // Initialize display
    update_profile_display();
    update_readings_display();
    
    // Load main screen
    lv_screen_load(main_screen);
}