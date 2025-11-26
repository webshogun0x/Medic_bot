#include "analytics_screen.h"
#include "../display_manager.h"
#include <stdlib.h>
#include <stdio.h>

static lv_obj_t * analytics_bmi_value;
static lv_obj_t * analytics_spo2_value;
static lv_obj_t * analytics_temp_value;
static lv_obj_t * analytics_hr_value;
static lv_obj_t * analytics_bmi_arc;
static lv_obj_t * analytics_temp_arc;
static lv_obj_t * diastolic_ta;
static lv_obj_t * systolic_ta;
static lv_obj_t * keyboard;

typedef struct {
    float bmi;
    float temperature;
    uint8_t heart_rate;
    uint8_t spo2;
} sensor_readings_t;

static sensor_readings_t current_readings = {
    .bmi = 24.8f,
    .temperature = 98.6f,
    .heart_rate = 72,
    .spo2 = 98
};

static void keyboard_ready_cb(lv_event_t * e);

static void generate_random_readings(void)
{
    current_readings.bmi = 18.5f + ((float)rand() / RAND_MAX) * (35.0f - 18.5f);
    current_readings.temperature = 96.0f + ((float)rand() / RAND_MAX) * (102.0f - 96.0f);
    current_readings.heart_rate = 60 + (rand() % 61);
    current_readings.spo2 = 90 + (rand() % 11);
}

static void read_button_event_cb(lv_event_t * e)
{
    // Send command to ESP32-S3 to start oximeter reading
    send_uart_command("READ_OXIMETER");
    
    // Update UI to show measurement in progress
    if (analytics_spo2_value) lv_label_set_text(analytics_spo2_value, "Reading...");
    if (analytics_hr_value) lv_label_set_text(analytics_hr_value, "Reading...");
}

void send_uart_command(const char* command);

static void save_button_event_cb(lv_event_t * e)
{
    // Send command to ESP32-S3 to save current readings
    send_uart_command("SAVE_READINGS");
}

static void create_bmi_section(lv_obj_t * parent)
{
    lv_obj_t * bmi_cont = lv_obj_create(parent);
    lv_obj_set_grid_cell(bmi_cont, LV_GRID_ALIGN_STRETCH, 0, 1, LV_GRID_ALIGN_STRETCH, 0, 1);
    lv_obj_set_style_bg_color(bmi_cont, lv_color_white(), 0);
    lv_obj_set_style_radius(bmi_cont, 15, 0);
    lv_obj_set_style_pad_all(bmi_cont, 15, 0);
    
    lv_obj_t * bmi_icon = lv_label_create(bmi_cont);
    lv_label_set_text(bmi_icon, LV_SYMBOL_CHARGE);
    lv_obj_set_style_text_color(bmi_icon, lv_color_hex(0x9C27B0), 0);
    lv_obj_set_style_text_font(bmi_icon, &lv_font_montserrat_16, 0);
    lv_obj_align(bmi_icon, LV_ALIGN_TOP_LEFT, 0, 0);
    
    lv_obj_t * bmi_title = lv_label_create(bmi_cont);
    lv_label_set_text(bmi_title, "BMI");
    lv_obj_set_style_text_font(bmi_title, &lv_font_montserrat_16, 0);
    lv_obj_align(bmi_title, LV_ALIGN_TOP_RIGHT, 0, 0);
    
    lv_obj_t * gauge_cont = lv_obj_create(bmi_cont);
    lv_obj_set_size(gauge_cont, 180, 180);
    lv_obj_align(gauge_cont, LV_ALIGN_CENTER, 0, 10);
    lv_obj_set_style_bg_opa(gauge_cont, LV_OPA_TRANSP, 0);
    lv_obj_set_style_border_opa(gauge_cont, LV_OPA_TRANSP, 0);
    
    lv_obj_t * arc = lv_arc_create(gauge_cont);
    lv_obj_set_size(arc, 160, 160);
    lv_obj_center(arc);
    lv_arc_set_range(arc, 0, 100);
    lv_arc_set_value(arc, 25);
    lv_arc_set_bg_angles(arc, 135, 45);
    lv_obj_set_style_arc_color(arc, lv_color_hex(0xe0e0e0), LV_PART_MAIN);
    lv_obj_set_style_arc_color(arc, lv_color_hex(0x4CAF50), LV_PART_INDICATOR);
    lv_obj_set_style_arc_width(arc, 15, LV_PART_MAIN);
    lv_obj_set_style_arc_width(arc, 15, LV_PART_INDICATOR);
    lv_obj_remove_style(arc, NULL, LV_PART_KNOB);
    lv_obj_clear_flag(arc, LV_OBJ_FLAG_CLICKABLE);
    
    analytics_bmi_value = lv_label_create(gauge_cont);
    lv_label_set_text(analytics_bmi_value, "24.8");
    lv_obj_set_style_text_font(analytics_bmi_value, &lv_font_montserrat_16, 0);
    lv_obj_set_style_text_color(analytics_bmi_value, lv_color_hex(0x333333), 0);
    lv_obj_align(analytics_bmi_value, LV_ALIGN_CENTER, 0, 0);
    
    analytics_bmi_arc = arc;
}

static void create_spo2_section(lv_obj_t * parent)
{
    lv_obj_t * spo2_cont = lv_obj_create(parent);
    lv_obj_set_grid_cell(spo2_cont, LV_GRID_ALIGN_STRETCH, 1, 1, LV_GRID_ALIGN_STRETCH, 0, 1);
    lv_obj_set_style_bg_color(spo2_cont, lv_color_white(), 0);
    lv_obj_set_style_radius(spo2_cont, 15, 0);
    lv_obj_set_style_pad_all(spo2_cont, 15, 0);
    
    lv_obj_t * spo2_icon = lv_label_create(spo2_cont);
    lv_label_set_text(spo2_icon, LV_SYMBOL_WIFI);
    lv_obj_set_style_text_color(spo2_icon, lv_color_hex(0x2196F3), 0);
    lv_obj_set_style_text_font(spo2_icon, &lv_font_montserrat_16, 0);
    lv_obj_align(spo2_icon, LV_ALIGN_TOP_LEFT, 0, 0);
    
    lv_obj_t * spo2_title = lv_label_create(spo2_cont);
    lv_label_set_text(spo2_title, "SpO2");
    lv_obj_set_style_text_font(spo2_title, &lv_font_montserrat_16, 0);
    lv_obj_align(spo2_title, LV_ALIGN_TOP_RIGHT, 0, 0);
    
    lv_obj_t * bar_cont = lv_obj_create(spo2_cont);
    lv_obj_set_size(bar_cont, LV_PCT(100), 80);
    lv_obj_align(bar_cont, LV_ALIGN_CENTER, 0, 0);
    lv_obj_set_style_bg_opa(bar_cont, LV_OPA_TRANSP, 0);
    lv_obj_set_style_border_opa(bar_cont, LV_OPA_TRANSP, 0);
    lv_obj_set_flex_flow(bar_cont, LV_FLEX_FLOW_ROW);
    lv_obj_set_flex_align(bar_cont, LV_FLEX_ALIGN_SPACE_AROUND, LV_FLEX_ALIGN_END, LV_FLEX_ALIGN_CENTER);
    
    int bar_heights[] = {20, 35, 45, 30, 50, 25, 40, 35};
    for(int i = 0; i < 8; i++) {
        lv_obj_t * bar = lv_obj_create(bar_cont);
        lv_obj_set_size(bar, 8, bar_heights[i]);
        lv_obj_set_style_bg_color(bar, lv_color_hex(0x2196F3), 0);
        lv_obj_set_style_radius(bar, 4, 0);
        lv_obj_set_style_border_opa(bar, LV_OPA_TRANSP, 0);
    }
    
    analytics_spo2_value = lv_label_create(spo2_cont);
    lv_label_set_text(analytics_spo2_value, "98%");
    lv_obj_set_style_text_font(analytics_spo2_value, &lv_font_montserrat_16, 0);
    lv_obj_align(analytics_spo2_value, LV_ALIGN_BOTTOM_MID, 0, 0);
}

static void create_temperature_section(lv_obj_t * parent)
{
    lv_obj_t * temp_cont = lv_obj_create(parent);
    lv_obj_set_grid_cell(temp_cont, LV_GRID_ALIGN_STRETCH, 0, 1, LV_GRID_ALIGN_STRETCH, 1, 1);
    lv_obj_set_style_bg_color(temp_cont, lv_color_white(), 0);
    lv_obj_set_style_radius(temp_cont, 15, 0);
    lv_obj_set_style_pad_all(temp_cont, 15, 0);
    
    lv_obj_t * temp_icon = lv_label_create(temp_cont);
    lv_label_set_text(temp_icon, LV_SYMBOL_WARNING);
    lv_obj_set_style_text_color(temp_icon, lv_color_hex(0xFF5722), 0);
    lv_obj_set_style_text_font(temp_icon, &lv_font_montserrat_16, 0);
    lv_obj_align(temp_icon, LV_ALIGN_TOP_LEFT, 0, 0);
    
    lv_obj_t * temp_title = lv_label_create(temp_cont);
    lv_label_set_text(temp_title, "Temperature");
    lv_obj_set_style_text_font(temp_title, &lv_font_montserrat_16, 0);
    lv_obj_align(temp_title, LV_ALIGN_TOP_RIGHT, 0, 0);
    
    lv_obj_t * gauge_cont = lv_obj_create(temp_cont);
    lv_obj_set_size(gauge_cont, 120, 120);
    lv_obj_align(gauge_cont, LV_ALIGN_CENTER, 0, 10);
    lv_obj_set_style_bg_opa(gauge_cont, LV_OPA_TRANSP, 0);
    lv_obj_set_style_border_opa(gauge_cont, LV_OPA_TRANSP, 0);
    
    lv_obj_t * temp_arc = lv_arc_create(gauge_cont);
    lv_obj_set_size(temp_arc, 120, 120);
    lv_obj_center(temp_arc);
    lv_arc_set_range(temp_arc, 95, 105);
    lv_arc_set_value(temp_arc, 98);
    lv_arc_set_bg_angles(temp_arc, 135, 45);
    lv_obj_set_style_arc_color(temp_arc, lv_color_hex(0xe0e0e0), LV_PART_MAIN);
    lv_obj_set_style_arc_color(temp_arc, lv_color_hex(0xFF5722), LV_PART_INDICATOR);
    lv_obj_set_style_arc_width(temp_arc, 10, LV_PART_MAIN);
    lv_obj_set_style_arc_width(temp_arc, 10, LV_PART_INDICATOR);
    lv_obj_remove_style(temp_arc, NULL, LV_PART_KNOB);
    lv_obj_clear_flag(temp_arc, LV_OBJ_FLAG_CLICKABLE);
    
    analytics_temp_value = lv_label_create(gauge_cont);
    lv_label_set_text(analytics_temp_value, "98.6°C");
    lv_obj_set_style_text_font(analytics_temp_value, &lv_font_montserrat_16, 0);
    lv_obj_align(analytics_temp_value, LV_ALIGN_CENTER, 0, 0);
    
    analytics_temp_arc = temp_arc;
}

static void create_heart_rate_section(lv_obj_t * parent)
{
    lv_obj_t * hr_cont = lv_obj_create(parent);
    lv_obj_set_grid_cell(hr_cont, LV_GRID_ALIGN_STRETCH, 1, 1, LV_GRID_ALIGN_STRETCH, 1, 1);
    lv_obj_set_style_bg_color(hr_cont, lv_color_white(), 0);
    lv_obj_set_style_radius(hr_cont, 15, 0);
    lv_obj_set_style_pad_all(hr_cont, 15, 0);
    
    lv_obj_t * hr_icon = lv_label_create(hr_cont);
    lv_label_set_text(hr_icon, LV_SYMBOL_PLUS);
    lv_obj_set_style_text_color(hr_icon, lv_color_hex(0xF44336), 0);
    lv_obj_set_style_text_font(hr_icon, &lv_font_montserrat_16, 0);
    lv_obj_align(hr_icon, LV_ALIGN_TOP_LEFT, 0, 0);
    
    lv_obj_t * hr_title = lv_label_create(hr_cont);
    lv_label_set_text(hr_title, "Heart Rate");
    lv_obj_set_style_text_font(hr_title, &lv_font_montserrat_16, 0);
    lv_obj_align(hr_title, LV_ALIGN_TOP_RIGHT, 0, 0);
    
    lv_obj_t * chart = lv_chart_create(hr_cont);
    lv_obj_set_size(chart, LV_PCT(100), 80);
    lv_obj_align(chart, LV_ALIGN_CENTER, 0, 0);
    lv_chart_set_type(chart, LV_CHART_TYPE_LINE);
    lv_chart_set_point_count(chart, 10);
    lv_chart_set_range(chart, LV_CHART_AXIS_PRIMARY_Y, 60, 100);
    lv_obj_set_style_bg_opa(chart, LV_OPA_TRANSP, 0);
    lv_obj_set_style_border_opa(chart, LV_OPA_TRANSP, 0);
    
    lv_chart_series_t * hr_series = lv_chart_add_series(chart, lv_color_hex(0xFF5722), LV_CHART_AXIS_PRIMARY_Y);
    
    lv_coord_t hr_data[] = {72, 75, 73, 76, 74, 77, 75, 78, 76, 74};
    for(int i = 0; i < 10; i++) {
        lv_chart_set_next_value(chart, hr_series, hr_data[i]);
    }
    
    analytics_hr_value = lv_label_create(hr_cont);
    lv_label_set_text(analytics_hr_value, "72 BPM");
    lv_obj_set_style_text_font(analytics_hr_value, &lv_font_montserrat_16, 0);
    lv_obj_align(analytics_hr_value, LV_ALIGN_BOTTOM_MID, 0, 0);
}

static void textarea_focus_cb(lv_event_t * e)
{
    lv_obj_t * ta = lv_event_get_target(e);
    if (keyboard == NULL) {
        keyboard = lv_keyboard_create(lv_layer_top());
        lv_obj_set_size(keyboard, LV_PCT(100), LV_PCT(40));
        lv_obj_align(keyboard, LV_ALIGN_BOTTOM_MID, 0, 0);
        lv_obj_add_event_cb(keyboard, keyboard_ready_cb, LV_EVENT_ALL, NULL);
    }
    lv_keyboard_set_textarea(keyboard, ta);
}

static void keyboard_ready_cb(lv_event_t * e)
{
    lv_keyboard_def_event_cb(e);
    
    if(lv_event_get_code(e) == LV_EVENT_READY || lv_event_get_code(e) == LV_EVENT_CANCEL) {
        if (keyboard) {
            lv_obj_delete(keyboard);
            keyboard = NULL;
        }
    }
}

static void create_blood_pressure_section(lv_obj_t * parent)
{
    lv_obj_t * bp_cont = lv_obj_create(parent);
    lv_obj_set_grid_cell(bp_cont, LV_GRID_ALIGN_STRETCH, 0, 2, LV_GRID_ALIGN_STRETCH, 2, 1);
    lv_obj_set_style_bg_color(bp_cont, lv_color_white(), 0);
    lv_obj_set_style_radius(bp_cont, 15, 0);
    lv_obj_set_style_pad_all(bp_cont, 20, 0);
    
    lv_obj_t * bp_title = lv_label_create(bp_cont);
    lv_label_set_text(bp_title, "Blood Pressure");
    lv_obj_set_style_text_font(bp_title, &lv_font_montserrat_16, 0);
    lv_obj_align(bp_title, LV_ALIGN_TOP_MID, 0, 0);
    
    // Systolic input
    lv_obj_t * systolic_label = lv_label_create(bp_cont);
    lv_label_set_text(systolic_label, "Systolic:");
    lv_obj_set_style_text_font(systolic_label, &lv_font_montserrat_14, 0);
    lv_obj_align(systolic_label, LV_ALIGN_TOP_LEFT, 0, 35);
    
    systolic_ta = lv_textarea_create(bp_cont);
    lv_obj_set_size(systolic_ta, 120, 40);
    lv_textarea_set_placeholder_text(systolic_ta, "120");
    lv_textarea_set_one_line(systolic_ta, true);
    lv_obj_align_to(systolic_ta, systolic_label, LV_ALIGN_OUT_BOTTOM_LEFT, 0, 5);
    lv_obj_add_event_cb(systolic_ta, textarea_focus_cb, LV_EVENT_FOCUSED, NULL);
    
    // Diastolic input
    lv_obj_t * diastolic_label = lv_label_create(bp_cont);
    lv_label_set_text(diastolic_label, "Diastolic:");
    lv_obj_set_style_text_font(diastolic_label, &lv_font_montserrat_14, 0);
    lv_obj_align(diastolic_label, LV_ALIGN_TOP_RIGHT, 0, 35);
    
    diastolic_ta = lv_textarea_create(bp_cont);
    lv_obj_set_size(diastolic_ta, 120, 40);
    lv_textarea_set_placeholder_text(diastolic_ta, "80");
    lv_textarea_set_one_line(diastolic_ta, true);
    lv_obj_align_to(diastolic_ta, diastolic_label, LV_ALIGN_OUT_BOTTOM_RIGHT, 0, 5);
    lv_obj_add_event_cb(diastolic_ta, textarea_focus_cb, LV_EVENT_FOCUSED, NULL);
}

static void create_action_buttons(lv_obj_t * parent)
{
    lv_obj_t * btn_cont = lv_obj_create(parent);
    lv_obj_set_grid_cell(btn_cont, LV_GRID_ALIGN_STRETCH, 0, 2, LV_GRID_ALIGN_CENTER, 3, 1);
    lv_obj_set_style_bg_opa(btn_cont, LV_OPA_TRANSP, 0);
    lv_obj_set_style_border_opa(btn_cont, LV_OPA_TRANSP, 0);
    lv_obj_set_flex_flow(btn_cont, LV_FLEX_FLOW_ROW);
    lv_obj_set_flex_align(btn_cont, LV_FLEX_ALIGN_SPACE_EVENLY, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
    
    lv_obj_t * read_btn = lv_btn_create(btn_cont);
    lv_obj_set_size(read_btn, 150, 50);
    lv_obj_set_style_bg_color(read_btn, lv_color_hex(0x2196F3), 0);
    lv_obj_set_style_radius(read_btn, 25, 0);
    lv_obj_set_style_border_opa(read_btn, LV_OPA_TRANSP, 0);
    
    lv_obj_t * read_label = lv_label_create(read_btn);
    lv_label_set_text(read_label, "Read");
    lv_obj_set_style_text_color(read_label, lv_color_white(), 0);
    lv_obj_set_style_text_font(read_label, &lv_font_montserrat_16, 0);
    lv_obj_center(read_label);
    lv_obj_add_event_cb(read_btn, read_button_event_cb, LV_EVENT_CLICKED, NULL);
    
    lv_obj_t * save_btn = lv_btn_create(btn_cont);
    lv_obj_set_size(save_btn, 150, 50);
    lv_obj_set_style_bg_color(save_btn, lv_color_white(), 0);
    lv_obj_set_style_radius(save_btn, 25, 0);
    lv_obj_set_style_border_color(save_btn, lv_color_hex(0x2196F3), 0);
    lv_obj_set_style_border_width(save_btn, 2, 0);
    
    lv_obj_t * save_label = lv_label_create(save_btn);
    lv_label_set_text(save_label, "Save");
    lv_obj_set_style_text_color(save_label, lv_color_hex(0x2196F3), 0);
    lv_obj_set_style_text_font(save_label, &lv_font_montserrat_16, 0);
    lv_obj_center(save_label);
    lv_obj_add_event_cb(save_btn, save_button_event_cb, LV_EVENT_CLICKED, NULL);
}

void analytics_screen_create(lv_obj_t * parent)
{
    lv_obj_set_style_bg_color(parent, lv_color_hex(0xf0f0f0), 0);
    
    static int32_t analytics_col_dsc[] = {LV_GRID_FR(1), LV_GRID_FR(1), LV_GRID_TEMPLATE_LAST};
    static int32_t analytics_row_dsc[] = {LV_GRID_FR(1), LV_GRID_FR(1), 120, 60, LV_GRID_TEMPLATE_LAST};
    lv_obj_set_grid_dsc_array(parent, analytics_col_dsc, analytics_row_dsc);
    
    create_bmi_section(parent);
    create_spo2_section(parent);
    create_temperature_section(parent);
    create_heart_rate_section(parent);
    create_blood_pressure_section(parent);
    create_action_buttons(parent);
}

void analytics_update_readings(float bmi, float temp, uint8_t hr, uint8_t spo2)
{
    current_readings.bmi = bmi;
    current_readings.temperature = temp;
    current_readings.heart_rate = hr;
    current_readings.spo2 = spo2;
    
    if (analytics_bmi_value) {
        char buf[16];
        snprintf(buf, sizeof(buf), "%.1f", bmi);
        lv_label_set_text(analytics_bmi_value, buf);
        
        int arc_value = (int)((bmi - 18.5f) / (35.0f - 18.5f) * 100);
        if (arc_value < 0) arc_value = 0;
        if (arc_value > 100) arc_value = 100;
        lv_arc_set_value(analytics_bmi_arc, arc_value);
    }
    
    if (analytics_spo2_value) {
        char buf[16];
        snprintf(buf, sizeof(buf), "%d%%", spo2);
        lv_label_set_text(analytics_spo2_value, buf);
    }
    
    if (analytics_temp_value) {
        char buf[16];
        snprintf(buf, sizeof(buf), "%.1f°C", temp);
        lv_label_set_text(analytics_temp_value, buf);
        
        int temp_arc_value = (int)temp;
        if (temp_arc_value < 95) temp_arc_value = 95;
        if (temp_arc_value > 105) temp_arc_value = 105;
        lv_arc_set_value(analytics_temp_arc, temp_arc_value);
    }
    
    if (analytics_hr_value) {
        char buf[16];
        snprintf(buf, sizeof(buf), "%d BPM", hr);
        lv_label_set_text(analytics_hr_value, buf);
    }
}