#include "instruction_screen.h"
#include "display_manager.h"

static lv_obj_t * instruction_screen;

static void enroll_btn_cb(lv_event_t * e)
{
    // Send enrollment command to ESP32-S3
    send_uart_command("START_ENROLLMENT");
    lv_obj_delete(instruction_screen);
    display_show_enroll_screen();
}

static void login_btn_cb(lv_event_t * e)
{
    // Send login command to ESP32-S3
    send_uart_command("START_LOGIN");
    lv_obj_delete(instruction_screen);
    display_show_login_screen();
}

void instruction_screen_create(void)
{
    instruction_screen = lv_obj_create(lv_screen_active());
    lv_obj_set_size(instruction_screen, LV_PCT(100), LV_PCT(100));
    lv_obj_set_style_bg_color(instruction_screen, lv_color_hex(0xf5f5f5), 0);
    
    lv_obj_t * welcome = lv_label_create(instruction_screen);
    lv_label_set_text(welcome, "Welcome to MEDIC-BOT");
    lv_obj_set_style_text_font(welcome, &lv_font_montserrat_16, 0);
    lv_obj_align(welcome, LV_ALIGN_TOP_MID, 0, 50);
    
    lv_obj_t * enroll_btn = lv_btn_create(instruction_screen);
    lv_obj_set_size(enroll_btn, 200, 60);
    lv_obj_set_style_bg_color(enroll_btn, lv_color_hex(0x4CAF50), 0);
    lv_obj_align(enroll_btn, LV_ALIGN_CENTER, -120, 0);
    lv_obj_add_event_cb(enroll_btn, enroll_btn_cb, LV_EVENT_CLICKED, NULL);
    
    lv_obj_t * enroll_label = lv_label_create(enroll_btn);
    lv_label_set_text(enroll_label, "Enroll");
    lv_obj_set_style_text_color(enroll_label, lv_color_white(), 0);
    lv_obj_center(enroll_label);
    
    lv_obj_t * login_btn = lv_btn_create(instruction_screen);
    lv_obj_set_size(login_btn, 200, 60);
    lv_obj_set_style_bg_color(login_btn, lv_color_hex(0x2196F3), 0);
    lv_obj_align(login_btn, LV_ALIGN_CENTER, 120, 0);
    lv_obj_add_event_cb(login_btn, login_btn_cb, LV_EVENT_CLICKED, NULL);
    
    lv_obj_t * login_label = lv_label_create(login_btn);
    lv_label_set_text(login_label, "Login");
    lv_obj_set_style_text_color(login_label, lv_color_white(), 0);
    lv_obj_center(login_label);
}

