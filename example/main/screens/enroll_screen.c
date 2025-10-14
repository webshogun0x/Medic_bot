#include "enroll_screen.h"
#include "../display_manager.h"

static lv_obj_t * enroll_screen;
static lv_obj_t * enroll_message_label;

void display_message_handler(const display_message_t * msg)
{
    if (enroll_message_label && msg) {
        lv_label_set_text(enroll_message_label, msg->message);
        
        if (msg->msg_type == 1) {
            lv_obj_set_style_text_color(enroll_message_label, lv_color_hex(0x2196F3), 0);
        } else if (msg->msg_type == 2) {
            lv_obj_set_style_text_color(enroll_message_label, lv_color_hex(0xF44336), 0);
        } else if (msg->msg_type == 3) {
            lv_obj_set_style_text_color(enroll_message_label, lv_color_hex(0x4CAF50), 0);
        }
    }
}

static void back_button_event_cb(lv_event_t * e)
{
    display_show_instruction_screen();
    // Request current MODE_SWITCH state when returning to instruction screen
    send_uart_command("GET_MODE_SWITCH");
}

void enroll_screen_create(void)
{
    enroll_screen = lv_obj_create(lv_screen_active());
    lv_obj_set_size(enroll_screen, LV_PCT(100), LV_PCT(100));
    lv_obj_set_style_bg_color(enroll_screen, lv_color_hex(0xf5f5f5), 0);
    
    // Back button at top left
    lv_obj_t * back_btn = lv_btn_create(enroll_screen);
    lv_obj_set_size(back_btn, 80, 40);
    lv_obj_set_style_bg_color(back_btn, lv_color_hex(0x666666), 0);
    lv_obj_set_style_radius(back_btn, 20, 0);
    lv_obj_align(back_btn, LV_ALIGN_TOP_LEFT, 20, 20);
    lv_obj_add_event_cb(back_btn, back_button_event_cb, LV_EVENT_CLICKED, NULL);
    
    lv_obj_t * back_label = lv_label_create(back_btn);
    lv_label_set_text(back_label, "Back");
    lv_obj_set_style_text_color(back_label, lv_color_white(), 0);
    lv_obj_center(back_label);
    
    lv_obj_t * title = lv_label_create(enroll_screen);
    lv_label_set_text(title, "Fingerprint Enrollment");
    lv_obj_set_style_text_font(title, &lv_font_montserrat_16, 0);
    lv_obj_align(title, LV_ALIGN_TOP_MID, 0, 50);
    
    lv_obj_t * msg_container = lv_obj_create(enroll_screen);
    lv_obj_set_size(msg_container, LV_PCT(80), 200);
    lv_obj_align(msg_container, LV_ALIGN_CENTER, 0, 0);
    lv_obj_set_style_bg_color(msg_container, lv_color_white(), 0);
    lv_obj_set_style_radius(msg_container, 10, 0);
    
    enroll_message_label = lv_label_create(msg_container);
    lv_label_set_text(enroll_message_label, "Waiting for enrollment to start...");
    lv_obj_set_style_text_align(enroll_message_label, LV_TEXT_ALIGN_CENTER, 0);
    lv_obj_set_width(enroll_message_label, LV_PCT(90));
    lv_obj_center(enroll_message_label);
    
    lv_obj_t * status_arc = lv_arc_create(enroll_screen);
    lv_obj_set_size(status_arc, 100, 100);
    lv_obj_align(status_arc, LV_ALIGN_BOTTOM_MID, 0, -50);
    lv_arc_set_range(status_arc, 0, 100);
    lv_arc_set_value(status_arc, 0);
    lv_obj_remove_style(status_arc, NULL, LV_PART_KNOB);
    lv_obj_clear_flag(status_arc, LV_OBJ_FLAG_CLICKABLE);
}