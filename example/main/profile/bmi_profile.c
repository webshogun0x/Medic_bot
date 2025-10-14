/**
 * @file profile_screen.c
 * @brief Profile screen implementation
 */

#include "bmi_profile.h"
#include <stdio.h>
#include <string.h>

// Profile screen widgets
static lv_obj_t * profile_name_display;
static lv_obj_t * profile_age_display;
static lv_obj_t * profile_weight_display;
static lv_obj_t * profile_height_display;
static lv_obj_t * profile_bmi_display;
static lv_obj_t * profile_heartrate_display;
static lv_obj_t * profile_temp_display;
static lv_obj_t * profile_spo2_display;
static lv_obj_t * avatar_initials;
static lv_obj_t * bmi_chart;
static lv_chart_series_t * bmi_series;

static void create_profile_overview(lv_obj_t * parent);
static void create_bmi_records_chart(lv_obj_t * parent);

void bmi_profile_create(lv_obj_t* parent)
{
    lv_obj_set_style_bg_color(parent, lv_color_hex(0xf0f0f0), 0);
    lv_obj_set_flex_flow(parent, LV_FLEX_FLOW_ROW_WRAP);
    
    create_profile_overview(parent);
    create_bmi_records_chart(parent);
}

void bmi_profile_update(const profile_data_t* profile, const sensor_readings_t* readings)
{
    if (!profile || !readings) return;
    
    // Update profile overview display
    if (profile_name_display) {
        lv_label_set_text(profile_name_display, profile->name);
    }
    
    // Update avatar initials
    if (avatar_initials && strlen(profile->name) > 0) {
        char initials[3] = {0};
        initials[0] = profile->name[0];
        
        char *space = strchr(profile->name, ' ');
        if (space && *(space + 1)) {
            initials[1] = *(space + 1);
        } else if (strlen(profile->name) > 1) {
            initials[1] = profile->name[1];
        }
        
        lv_label_set_text(avatar_initials, initials);
    }
    
    // Update health stats
    if (profile_age_display) {
        char age_buf[16];
        snprintf(age_buf, sizeof(age_buf), "%d years", profile->age);
        lv_label_set_text(profile_age_display, age_buf);
    }
    
    if (profile_weight_display) {
        char weight_buf[16];
        snprintf(weight_buf, sizeof(weight_buf), "%.1f kg", profile->weight);
        lv_label_set_text(profile_weight_display, weight_buf);
    }
    
    if (profile_height_display) {
        char height_buf[16];
        snprintf(height_buf, sizeof(height_buf), "%.1f cm", profile->height);
        lv_label_set_text(profile_height_display, height_buf);
    }
    
    if (profile_bmi_display) {
        char bmi_buf[16];
        snprintf(bmi_buf, sizeof(bmi_buf), "BMI: %.1f", readings->bmi);
        lv_label_set_text(profile_bmi_display, bmi_buf);
    }
    
    if (profile_heartrate_display) {
        char hr_buf[16];
        snprintf(hr_buf, sizeof(hr_buf), "%d BPM", readings->heart_rate);
        lv_label_set_text(profile_heartrate_display, hr_buf);
    }
    
    if (profile_temp_display) {
        char temp_buf[16];
        snprintf(temp_buf, sizeof(temp_buf), "%.1f°F", readings->temperature);
        lv_label_set_text(profile_temp_display, temp_buf);
    }
    
    if (profile_spo2_display) {
        char spo2_buf[16];
        snprintf(spo2_buf, sizeof(spo2_buf), "%d%%", readings->spo2);
        lv_label_set_text(profile_spo2_display, spo2_buf);
    }

    // Update BMI chart
    if (bmi_chart && bmi_series) {
        lv_chart_remove_series(bmi_chart, bmi_series);
        bmi_series = lv_chart_add_series(bmi_chart, lv_palette_main(LV_PALETTE_BLUE), LV_CHART_AXIS_PRIMARY_Y);
        for (int i = 0; i < 12; i++) {
            lv_chart_set_next_value(bmi_chart, bmi_series, (int32_t)(profile->bmi_data[i] * 10));
        }
        lv_chart_refresh(bmi_chart);
    }
}

static void create_profile_overview(lv_obj_t * parent)
{
    lv_obj_t * overview_panel = lv_obj_create(parent);
    lv_obj_set_size(overview_panel, LV_PCT(45), LV_SIZE_CONTENT);
    lv_obj_set_style_pad_all(overview_panel, 20, 0);
    lv_obj_set_style_bg_color(overview_panel, lv_color_white(), 0);
    lv_obj_set_style_radius(overview_panel, 10, 0);

    // Custom Avatar
    lv_obj_t * avatar = lv_obj_create(overview_panel);
    lv_obj_set_size(avatar, 80, 80);
    lv_obj_align(avatar, LV_ALIGN_TOP_MID, 0, 10);
    lv_obj_set_style_radius(avatar, 40, 0);
    lv_obj_set_style_bg_color(avatar, lv_color_hex(0x2196F3), 0);
    lv_obj_set_style_border_width(avatar, 3, 0);
    lv_obj_set_style_border_color(avatar, lv_color_white(), 0);
    
    avatar_initials = lv_label_create(avatar);
    lv_label_set_text(avatar_initials, "O");
    lv_obj_set_style_text_color(avatar_initials, lv_color_white(), 0);
    lv_obj_set_style_text_font(avatar_initials, &lv_font_montserrat_16, 0);
    lv_obj_center(avatar_initials);

    // Profile name
    profile_name_display = lv_label_create(overview_panel);
    lv_label_set_text(profile_name_display, "Oluwatimileyin");
    lv_obj_set_style_text_font(profile_name_display, &lv_font_montserrat_16, 0);
    lv_obj_align_to(profile_name_display, avatar, LV_ALIGN_OUT_BOTTOM_MID, 0, 10);

    // Health stats section
    lv_obj_t * stats_title = lv_label_create(overview_panel);
    lv_label_set_text(stats_title, "Health Stats");
    lv_obj_set_style_text_font(stats_title, &lv_font_montserrat_16, 0);
    lv_obj_set_style_text_color(stats_title, lv_color_hex(0x333333), 0);
    lv_obj_align_to(stats_title, profile_name_display, LV_ALIGN_OUT_BOTTOM_LEFT, 0, 20);

    // Age
    lv_obj_t * age_icon = lv_label_create(overview_panel);
    lv_label_set_text(age_icon, LV_SYMBOL_DUMMY);
    lv_obj_set_style_text_color(age_icon, lv_color_hex(0x2196F3), 0);
    lv_obj_align_to(age_icon, stats_title, LV_ALIGN_OUT_BOTTOM_LEFT, 0, 10);

    profile_age_display = lv_label_create(overview_panel);
    lv_label_set_text(profile_age_display, "35 years");
    lv_obj_align_to(profile_age_display, age_icon, LV_ALIGN_OUT_RIGHT_MID, 10, 0);

    // Weight
    lv_obj_t * weight_icon = lv_label_create(overview_panel);
    lv_label_set_text(weight_icon, LV_SYMBOL_SETTINGS);
    lv_obj_set_style_text_color(weight_icon, lv_color_hex(0x4CAF50), 0);
    lv_obj_align_to(weight_icon, age_icon, LV_ALIGN_OUT_BOTTOM_LEFT, 0, 8);

    profile_weight_display = lv_label_create(overview_panel);
    lv_label_set_text(profile_weight_display, "180.0 kg");
    lv_obj_align_to(profile_weight_display, weight_icon, LV_ALIGN_OUT_RIGHT_MID, 10, 0);

    // Height
    lv_obj_t * height_icon = lv_label_create(overview_panel);
    lv_label_set_text(height_icon, LV_SYMBOL_UP);
    lv_obj_set_style_text_color(height_icon, lv_color_hex(0xFF9800), 0);
    lv_obj_align_to(height_icon, weight_icon, LV_ALIGN_OUT_BOTTOM_LEFT, 0, 8);

    profile_height_display = lv_label_create(overview_panel);
    lv_label_set_text(profile_height_display, "175 cm");
    lv_obj_align_to(profile_height_display, height_icon, LV_ALIGN_OUT_RIGHT_MID, 10, 0);

    // BMI
    lv_obj_t * bmi_icon = lv_label_create(overview_panel);
    lv_label_set_text(bmi_icon, LV_SYMBOL_CHARGE);
    lv_obj_set_style_text_color(bmi_icon, lv_color_hex(0x9C27B0), 0);
    lv_obj_align_to(bmi_icon, height_icon, LV_ALIGN_OUT_BOTTOM_LEFT, 0, 8);

    profile_bmi_display = lv_label_create(overview_panel);
    lv_label_set_text(profile_bmi_display, "BMI: 24.8");
    lv_obj_align_to(profile_bmi_display, bmi_icon, LV_ALIGN_OUT_RIGHT_MID, 10, 0);

    // Heart Rate
    lv_obj_t * hr_icon = lv_label_create(overview_panel);
    lv_label_set_text(hr_icon, LV_SYMBOL_PLUS);
    lv_obj_set_style_text_color(hr_icon, lv_color_hex(0xF44336), 0);
    lv_obj_align_to(hr_icon, bmi_icon, LV_ALIGN_OUT_BOTTOM_LEFT, 0, 8);

    profile_heartrate_display = lv_label_create(overview_panel);
    lv_label_set_text(profile_heartrate_display, "72 BPM");
    lv_obj_align_to(profile_heartrate_display, hr_icon, LV_ALIGN_OUT_RIGHT_MID, 10, 0);

    // Temperature
    lv_obj_t * temp_icon = lv_label_create(overview_panel);
    lv_label_set_text(temp_icon, LV_SYMBOL_WARNING);
    lv_obj_set_style_text_color(temp_icon, lv_color_hex(0xFF5722), 0);
    lv_obj_align_to(temp_icon, hr_icon, LV_ALIGN_OUT_BOTTOM_LEFT, 0, 8);

    profile_temp_display = lv_label_create(overview_panel);
    lv_label_set_text(profile_temp_display, "98.6°F");
    lv_obj_align_to(profile_temp_display, temp_icon, LV_ALIGN_OUT_RIGHT_MID, 10, 0);

    // SpO2
    lv_obj_t * spo2_icon = lv_label_create(overview_panel);
    lv_label_set_text(spo2_icon, LV_SYMBOL_WIFI);
    lv_obj_set_style_text_color(spo2_icon, lv_color_hex(0x2196F3), 0);
    lv_obj_align_to(spo2_icon, temp_icon, LV_ALIGN_OUT_BOTTOM_LEFT, 0, 8);

    profile_spo2_display = lv_label_create(overview_panel);
    lv_label_set_text(profile_spo2_display, "98%");
    lv_obj_align_to(profile_spo2_display, spo2_icon, LV_ALIGN_OUT_RIGHT_MID, 10, 0);
}

static void create_bmi_records_chart(lv_obj_t * parent)
{
    lv_obj_t * chart_panel = lv_obj_create(parent);
    lv_obj_set_size(chart_panel, LV_PCT(90), LV_SIZE_CONTENT);
    lv_obj_set_style_pad_all(chart_panel, 20, 0);
    lv_obj_set_style_bg_color(chart_panel, lv_color_white(), 0);
    lv_obj_set_style_radius(chart_panel, 10, 0);
    lv_obj_add_flag(chart_panel, LV_OBJ_FLAG_FLEX_IN_NEW_TRACK);

    lv_obj_t * chart_title = lv_label_create(chart_panel);
    lv_label_set_text(chart_title, "BMI Records");
    lv_obj_set_style_text_font(chart_title, &lv_font_montserrat_16, 0);
    lv_obj_align(chart_title, LV_ALIGN_TOP_LEFT, 0, 0);

    lv_obj_t * date_label = lv_label_create(chart_panel);
    lv_label_set_text(date_label, "Last 12 months");
    lv_obj_set_style_text_color(date_label, lv_color_hex(0x666666), 0);
    lv_obj_align_to(date_label, chart_title, LV_ALIGN_OUT_BOTTOM_LEFT, 0, 5);

    bmi_chart = lv_chart_create(chart_panel);
    lv_obj_set_size(bmi_chart, LV_PCT(100), 200);
    lv_obj_align_to(bmi_chart, date_label, LV_ALIGN_OUT_BOTTOM_LEFT, 0, 10);
    lv_chart_set_type(bmi_chart, LV_CHART_TYPE_BAR);
    lv_chart_set_div_line_count(bmi_chart, 6, 0);
    lv_chart_set_point_count(bmi_chart, 12);
    lv_chart_set_range(bmi_chart, LV_CHART_AXIS_PRIMARY_Y, 15, 35);

    bmi_series = lv_chart_add_series(bmi_chart, lv_palette_main(LV_PALETTE_BLUE), LV_CHART_AXIS_PRIMARY_Y);

    lv_obj_t * bmi_status = lv_label_create(chart_panel);
    lv_label_set_text(bmi_status, "Varying BMI levels");
    lv_obj_set_style_text_color(bmi_status, lv_color_hex(0xFF9800), 0);
    lv_obj_align_to(bmi_status, bmi_chart, LV_ALIGN_OUT_BOTTOM_LEFT, 0, 10);
}