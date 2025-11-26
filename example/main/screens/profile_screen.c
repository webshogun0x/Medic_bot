#include "profile_screen.h"
#include "../display_manager.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

static lv_obj_t * name_label;
static lv_obj_t * gender_label;
static lv_obj_t * age_label;
static lv_obj_t * weight_label;
static lv_obj_t * height_label;
static lv_obj_t * bmi_chart;
static lv_chart_series_t * bmi_series;
static lv_obj_t * keyboard;
static lv_obj_t * calendar;
static lv_obj_t * profile_name_display;
static lv_obj_t * avatar_initials;

static profile_data_t current_profile = {
    .name = "Oluwatimileyin",
    .gender = "Male",
    .age = 18,
    .weight = 180.0f,
    .height = 5.9f,
    .bmi_data = {18.5f, 24.2f, 6.8f, 23.1f, 19.7f, 27.3f, 2.4f, 5.6f, 22.9f, 28.1f, 2.3f, 24.8f},
    .heart_rate = 72
};

static void keyboard_event_cb(lv_event_t * e);
static void calendar_event_cb(lv_event_t * e);
static void dropdown_event_cb(lv_event_t * e);
static void birthday_event_cb(lv_event_t * e);
static void update_button_event_cb(lv_event_t * e);
static void logout_button_event_cb(lv_event_t * e);
static void create_profile_fields(lv_obj_t * parent);
static void create_profile_overview(lv_obj_t * parent);
static void create_bmi_records_chart(lv_obj_t * parent);
static void update_display(void);

void profile_screen_init(lv_obj_t * parent)
{
    lv_obj_set_style_bg_color(parent, lv_color_hex(0xf0f0f0), 0);
    lv_obj_set_flex_flow(parent, LV_FLEX_FLOW_ROW_WRAP);
    
    create_profile_fields(parent);
    create_profile_overview(parent);
    create_bmi_records_chart(parent);
    update_display();
}

void profile_update_data(const profile_data_t * data)
{
    if (data) {
        memcpy(&current_profile, data, sizeof(profile_data_t));
        update_display();
    }
}

void profile_update_user_data(const char* name, const char* age, const char* gender)
{
    if (name && age && gender) {
        strncpy(current_profile.name, name, sizeof(current_profile.name) - 1);
        current_profile.name[sizeof(current_profile.name) - 1] = '\0';
        
        strncpy(current_profile.gender, gender, sizeof(current_profile.gender) - 1);
        current_profile.gender[sizeof(current_profile.gender) - 1] = '\0';
        
        // Parse age string to integer
        char* age_num = strstr(age, " ");
        if (age_num) {
            *age_num = '\0'; // Terminate at space
        }
        current_profile.age = atoi(age);
        
        update_display();
    }
}

void profile_get_data(profile_data_t * data)
{
    if (data) {
        memcpy(data, &current_profile, sizeof(profile_data_t));
    }
}

static void create_profile_fields(lv_obj_t * parent)
{
    keyboard = lv_keyboard_create(lv_screen_active());
    lv_obj_add_flag(keyboard, LV_OBJ_FLAG_HIDDEN);

    lv_obj_t * profile_panel = lv_obj_create(parent);
    lv_obj_set_size(profile_panel, LV_PCT(45), LV_SIZE_CONTENT);
    lv_obj_set_style_pad_all(profile_panel, 20, 0);

    static int32_t grid_col_dsc[] = {LV_GRID_FR(1), LV_GRID_FR(1), LV_GRID_TEMPLATE_LAST};
    static int32_t grid_row_dsc[] = {
        LV_GRID_CONTENT, 5, LV_GRID_CONTENT, 30, 5, LV_GRID_CONTENT, 30, 5,
        LV_GRID_CONTENT, 30, 5, LV_GRID_CONTENT, 30, 5, LV_GRID_CONTENT, 30,
        20, LV_GRID_CONTENT, LV_GRID_TEMPLATE_LAST
    };
    lv_obj_set_grid_dsc_array(profile_panel, grid_col_dsc, grid_row_dsc);

    lv_obj_t * profile_title = lv_label_create(profile_panel);
    lv_label_set_text(profile_title, "Profile");
    lv_obj_set_style_text_font(profile_title, &lv_font_montserrat_16, 0);
    lv_obj_set_grid_cell(profile_title, LV_GRID_ALIGN_START, 0, 2, LV_GRID_ALIGN_CENTER, 0, 1);

    lv_obj_t * name_label_title = lv_label_create(profile_panel);
    lv_label_set_text(name_label_title, "Name:");
    lv_obj_set_style_text_color(name_label_title, lv_color_hex(0x666666), 0);
    lv_obj_set_grid_cell(name_label_title, LV_GRID_ALIGN_START, 0, 2, LV_GRID_ALIGN_START, 2, 1);

    name_label = lv_textarea_create(profile_panel);
    lv_textarea_set_one_line(name_label, true);
    lv_textarea_set_placeholder_text(name_label, "Enter your name");
    lv_obj_set_grid_cell(name_label, LV_GRID_ALIGN_STRETCH, 0, 2, LV_GRID_ALIGN_START, 3, 1);
    lv_obj_add_event_cb(name_label, keyboard_event_cb, LV_EVENT_ALL, keyboard);

    lv_obj_t * gender_label_title = lv_label_create(profile_panel);
    lv_label_set_text(gender_label_title, "Gender:");
    lv_obj_set_style_text_color(gender_label_title, lv_color_hex(0x666666), 0);
    lv_obj_set_grid_cell(gender_label_title, LV_GRID_ALIGN_START, 0, 1, LV_GRID_ALIGN_START, 5, 1);

    gender_label = lv_dropdown_create(profile_panel);
    lv_dropdown_set_options(gender_label, "Male\nFemale");
    lv_obj_set_grid_cell(gender_label, LV_GRID_ALIGN_STRETCH, 0, 1, LV_GRID_ALIGN_CENTER, 6, 1);
    lv_obj_add_event_cb(gender_label, dropdown_event_cb, LV_EVENT_VALUE_CHANGED, NULL);

    lv_obj_t * age_label_title = lv_label_create(profile_panel);
    lv_label_set_text(age_label_title, "Birthday:");
    lv_obj_set_style_text_color(age_label_title, lv_color_hex(0x666666), 0);
    lv_obj_set_grid_cell(age_label_title, LV_GRID_ALIGN_START, 1, 1, LV_GRID_ALIGN_START, 5, 1);

    age_label = lv_textarea_create(profile_panel);
    lv_textarea_set_one_line(age_label, true);
    lv_textarea_set_placeholder_text(age_label, "Select birthday");
    lv_obj_set_grid_cell(age_label, LV_GRID_ALIGN_STRETCH, 1, 1, LV_GRID_ALIGN_CENTER, 6, 1);
    lv_obj_add_event_cb(age_label, birthday_event_cb, LV_EVENT_ALL, NULL);

    lv_obj_t * weight_label_title = lv_label_create(profile_panel);
    lv_label_set_text(weight_label_title, "Weight:");
    lv_obj_set_style_text_color(weight_label_title, lv_color_hex(0x666666), 0);
    lv_obj_set_grid_cell(weight_label_title, LV_GRID_ALIGN_START, 0, 1, LV_GRID_ALIGN_START, 8, 1);

    weight_label = lv_textarea_create(profile_panel);
    lv_textarea_set_one_line(weight_label, true);
    lv_textarea_set_placeholder_text(weight_label, "kg");
    lv_obj_set_grid_cell(weight_label, LV_GRID_ALIGN_STRETCH, 0, 1, LV_GRID_ALIGN_CENTER, 9, 1);
    lv_obj_add_event_cb(weight_label, keyboard_event_cb, LV_EVENT_ALL, keyboard);

    lv_obj_t * height_label_title = lv_label_create(profile_panel);
    lv_label_set_text(height_label_title, "Height:");
    lv_obj_set_style_text_color(height_label_title, lv_color_hex(0x666666), 0);
    lv_obj_set_grid_cell(height_label_title, LV_GRID_ALIGN_START, 1, 1, LV_GRID_ALIGN_START, 8, 1);

    height_label = lv_textarea_create(profile_panel);
    lv_textarea_set_one_line(height_label, true);
    lv_textarea_set_placeholder_text(height_label, "cm");
    lv_obj_set_grid_cell(height_label, LV_GRID_ALIGN_STRETCH, 1, 1, LV_GRID_ALIGN_CENTER, 9, 1);
    lv_obj_add_event_cb(height_label, keyboard_event_cb, LV_EVENT_ALL, keyboard);

    lv_obj_t * update_btn = lv_btn_create(profile_panel);
    lv_obj_set_size(update_btn, 120, 40);
    lv_obj_set_style_bg_color(update_btn, lv_color_hex(0x2196F3), 0);
    lv_obj_set_style_radius(update_btn, 20, 0);
    lv_obj_set_grid_cell(update_btn, LV_GRID_ALIGN_END, 1, 1, LV_GRID_ALIGN_CENTER, 11, 1);
    lv_obj_add_event_cb(update_btn, update_button_event_cb, LV_EVENT_CLICKED, NULL);
    
    lv_obj_t * update_label = lv_label_create(update_btn);
    lv_label_set_text(update_label, "Update");
    lv_obj_set_style_text_color(update_label, lv_color_white(), 0);
    lv_obj_center(update_label);
    
    // Logout button
    lv_obj_t * logout_btn = lv_btn_create(profile_panel);
    lv_obj_set_size(logout_btn, 120, 40);
    lv_obj_set_style_bg_color(logout_btn, lv_color_hex(0xF44336), 0);
    lv_obj_set_style_radius(logout_btn, 20, 0);
    lv_obj_set_grid_cell(logout_btn, LV_GRID_ALIGN_START, 0, 1, LV_GRID_ALIGN_CENTER, 17, 1);
    lv_obj_add_event_cb(logout_btn, logout_button_event_cb, LV_EVENT_CLICKED, NULL);
    
    lv_obj_t * logout_label = lv_label_create(logout_btn);
    lv_label_set_text(logout_label, "Logout");
    lv_obj_set_style_text_color(logout_label, lv_color_white(), 0);
    lv_obj_center(logout_label);
}

static void create_profile_overview(lv_obj_t * parent)
{
    lv_obj_t * overview_panel = lv_obj_create(parent);
    lv_obj_set_size(overview_panel, LV_PCT(45), LV_SIZE_CONTENT);
    lv_obj_set_style_pad_all(overview_panel, 20, 0);
    lv_obj_set_style_bg_color(overview_panel, lv_color_white(), 0);
    lv_obj_set_style_radius(overview_panel, 10, 0);

    lv_obj_t * avatar = lv_obj_create(overview_panel);
    lv_obj_set_size(avatar, 80, 80);
    lv_obj_align(avatar, LV_ALIGN_TOP_MID, 0, 10);
    lv_obj_set_style_radius(avatar, 40, 0);
    lv_obj_set_style_bg_color(avatar, lv_color_hex(0x2196F3), 0);
    lv_obj_set_style_border_width(avatar, 3, 0);
    lv_obj_set_style_border_color(avatar, lv_color_white(), 0);
    
    avatar_initials = lv_label_create(avatar);
    lv_label_set_text(avatar_initials, "JD");
    lv_obj_set_style_text_color(avatar_initials, lv_color_white(), 0);
    lv_obj_set_style_text_font(avatar_initials, &lv_font_montserrat_16, 0);
    lv_obj_center(avatar_initials);

    profile_name_display = lv_label_create(overview_panel);
    lv_label_set_text(profile_name_display, "User");
    lv_obj_set_style_text_font(profile_name_display, &lv_font_montserrat_16, 0);
    lv_label_set_long_mode(profile_name_display, LV_LABEL_LONG_SCROLL_CIRCULAR);
    lv_obj_set_width(profile_name_display, LV_PCT(90));
    lv_obj_align_to(profile_name_display, avatar, LV_ALIGN_OUT_BOTTOM_MID, 0, 10);

    lv_obj_t * desc = lv_label_create(overview_panel);
    lv_label_set_text(desc, "Health Profile");
    lv_obj_set_style_text_color(desc, lv_color_hex(0x666666), 0);
    lv_label_set_long_mode(desc, LV_LABEL_LONG_WRAP);
    lv_obj_set_width(desc, LV_PCT(90));
    lv_obj_align_to(desc, profile_name_display, LV_ALIGN_OUT_BOTTOM_MID, 0, 5);
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

static void update_display(void)
{
    if (name_label) lv_textarea_set_text(name_label, current_profile.name);
    if (gender_label) {
        uint16_t selected = strcmp(current_profile.gender, "Male") == 0 ? 0 : 1;
        lv_dropdown_set_selected(gender_label, selected);
    }
    if (age_label) {
        char age_buf[32];
        snprintf(age_buf, sizeof(age_buf), "%d years old", current_profile.age);
        lv_textarea_set_text(age_label, age_buf);
    }
    if (weight_label) {
        char weight_buf[16];
        snprintf(weight_buf, sizeof(weight_buf), "%.1f", current_profile.weight);
        lv_textarea_set_text(weight_label, weight_buf);
    }
    if (height_label) {
        char height_buf[16];
        snprintf(height_buf, sizeof(height_buf), "%.1f", current_profile.height);
        lv_textarea_set_text(height_label, height_buf);
    }

    if (profile_name_display) {
        lv_label_set_text(profile_name_display, current_profile.name);
    }
    
    if (avatar_initials && strlen(current_profile.name) > 0) {
        char initials[3] = {0};
        initials[0] = current_profile.name[0];
        
        char *space = strchr(current_profile.name, ' ');
        if (space && *(space + 1)) {
            initials[1] = *(space + 1);
        } else if (strlen(current_profile.name) > 1) {
            initials[1] = current_profile.name[1];
        }
        
        lv_label_set_text(avatar_initials, initials);
    }

    if (bmi_chart && bmi_series) {
        lv_chart_remove_series(bmi_chart, bmi_series);
        bmi_series = lv_chart_add_series(bmi_chart, lv_palette_main(LV_PALETTE_BLUE), LV_CHART_AXIS_PRIMARY_Y);
        for (int i = 0; i < 12; i++) {
            lv_chart_set_next_value(bmi_chart, bmi_series, (int32_t)(current_profile.bmi_data[i] * 10));
        }
        lv_chart_refresh(bmi_chart);
    }
}

static void keyboard_event_cb(lv_event_t * e)
{
    lv_event_code_t code = lv_event_get_code(e);
    lv_obj_t * ta = lv_event_get_target(e);
    lv_obj_t * kb = (lv_obj_t *)lv_event_get_user_data(e);
    
    if(code == LV_EVENT_FOCUSED) {
        if(lv_indev_get_type(lv_indev_active()) != LV_INDEV_TYPE_KEYPAD) {
            lv_keyboard_set_textarea(kb, ta);
            lv_obj_set_style_max_height(kb, LV_VER_RES * 2 / 3, 0);
            lv_obj_clear_flag(kb, LV_OBJ_FLAG_HIDDEN);
            lv_obj_scroll_to_view_recursive(ta, LV_ANIM_OFF);
        }
    }
    else if(code == LV_EVENT_DEFOCUSED) {
        lv_keyboard_set_textarea(kb, NULL);
        lv_obj_add_flag(kb, LV_OBJ_FLAG_HIDDEN);
        
        const char * text = lv_textarea_get_text(ta);
        if (ta == name_label) {
            strncpy(current_profile.name, text, sizeof(current_profile.name) - 1);
        } else if (ta == weight_label) {
            current_profile.weight = atof(text);
        } else if (ta == height_label) {
            current_profile.height = atof(text);
        }
    }
    else if(code == LV_EVENT_READY || code == LV_EVENT_CANCEL) {
        lv_obj_add_flag(kb, LV_OBJ_FLAG_HIDDEN);
    }
}

static void calendar_event_cb(lv_event_t * e)
{
    lv_event_code_t code = lv_event_get_code(e);
    lv_obj_t * ta = lv_event_get_user_data(e);
    lv_obj_t * obj = lv_event_get_current_target(e);
    if(code == LV_EVENT_VALUE_CHANGED) {
        lv_calendar_date_t d;
        lv_calendar_get_pressed_date(obj, &d);
        char buf[32];
        lv_snprintf(buf, sizeof(buf), "%02d.%02d.%d", d.day, d.month, d.year);
        lv_textarea_set_text(ta, buf);
        
        current_profile.age = 2024 - d.year;

        lv_obj_delete(calendar);
        calendar = NULL;
        lv_obj_remove_flag(lv_layer_top(), LV_OBJ_FLAG_CLICKABLE);
        lv_obj_set_style_bg_opa(lv_layer_top(), LV_OPA_TRANSP, 0);
    }
}

static void dropdown_event_cb(lv_event_t * e)
{
    lv_obj_t * dropdown = lv_event_get_target(e);
    uint16_t selected = lv_dropdown_get_selected(dropdown);
    strcpy(current_profile.gender, selected == 0 ? "Male" : "Female");
}

static void birthday_event_cb(lv_event_t * e)
{
    lv_event_code_t code = lv_event_get_code(e);
    lv_obj_t * ta = lv_event_get_target(e);

    if(code == LV_EVENT_FOCUSED) {
        if(lv_indev_get_type(lv_indev_active()) == LV_INDEV_TYPE_POINTER) {
            if(calendar == NULL) {
                lv_obj_add_flag(lv_layer_top(), LV_OBJ_FLAG_CLICKABLE);
                calendar = lv_calendar_create(lv_layer_top());
                lv_obj_set_style_bg_opa(lv_layer_top(), LV_OPA_50, 0);
                lv_obj_set_style_bg_color(lv_layer_top(), lv_palette_main(LV_PALETTE_GREY), 0);
                lv_obj_set_size(calendar, 300, 330);
                lv_calendar_set_showed_date(calendar, 1990, 01);
                lv_obj_align(calendar, LV_ALIGN_CENTER, 0, 30);
                lv_obj_add_event_cb(calendar, calendar_event_cb, LV_EVENT_ALL, ta);
                lv_calendar_header_dropdown_create(calendar);
            }
        }
    }
}

static void update_button_event_cb(lv_event_t * e)
{
    update_display();
}

static void logout_button_event_cb(lv_event_t * e)
{
    // Send logout command to ESP32-S3
    send_uart_command("LOGOUT");
    
    // Return to instruction screen
    display_show_instruction_screen();
}