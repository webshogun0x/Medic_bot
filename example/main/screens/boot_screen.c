#include "boot_screen.h"
#include "display_manager.h"
#include <stdio.h>

static lv_obj_t * boot_screen;
static lv_obj_t * logo;
static lv_obj_t * title;
static lv_obj_t * loading_bar;
static lv_obj_t * loading_label;
static lv_timer_t * loading_timer;
static lv_timer_t * intro_timer;
static int loading_progress = 0;

static void intro_complete_cb(lv_timer_t * timer);

static void loading_timer_cb(lv_timer_t * timer)
{
    loading_progress += 2;
    lv_bar_set_value(loading_bar, loading_progress, LV_ANIM_ON);
    
    char progress_text[16];
    snprintf(progress_text, sizeof(progress_text), "Loading... %d%%", loading_progress);
    lv_label_set_text(loading_label, progress_text);
    
    if(loading_progress >= 100) {
        lv_timer_delete(timer);
        intro_timer = lv_timer_create(intro_complete_cb, 500, NULL);
        lv_timer_set_repeat_count(intro_timer, 1);
    }
}

static void intro_complete_cb(lv_timer_t * timer)
{
    lv_timer_delete(timer);
    setup_uart_receiver();
    display_show_instruction_screen();
}

void boot_screen_create(void)
{
    boot_screen = lv_obj_create(lv_screen_active());
    lv_obj_set_size(boot_screen, LV_PCT(100), LV_PCT(100));
    lv_obj_set_style_bg_color(boot_screen, lv_color_hex(0x1a1a2e), 0);
    lv_obj_set_style_border_width(boot_screen, 0, 0);
    lv_obj_set_style_pad_all(boot_screen, 0, 0);
    
    logo = lv_label_create(boot_screen);
    lv_label_set_text(logo, "RFID-BIOMETRIC");
    lv_obj_set_style_text_font(logo, &lv_font_montserrat_16, 0);
    lv_obj_set_style_text_color(logo, lv_color_hex(0x00d4ff), 0);
    lv_obj_align(logo, LV_ALIGN_CENTER, 0, -60);
    
    title = lv_label_create(boot_screen);
    lv_label_set_text(title, "Access Control System");
    lv_obj_set_style_text_align(title, LV_TEXT_ALIGN_CENTER, 0);
    lv_obj_set_style_text_font(title, &lv_font_montserrat_16, 0);
    lv_obj_set_style_text_color(title, lv_color_hex(0x8892b0), 0);
    lv_obj_align_to(title, logo, LV_ALIGN_OUT_BOTTOM_MID, 0, 10);
    
    loading_bar = lv_bar_create(boot_screen);
    lv_obj_set_size(loading_bar, 300, 8);
    lv_obj_set_style_bg_color(loading_bar, lv_color_hex(0x2d2d44), 0);
    lv_obj_set_style_bg_color(loading_bar, lv_color_hex(0x00d4ff), LV_PART_INDICATOR);
    lv_obj_set_style_radius(loading_bar, 4, 0);
    lv_obj_align(loading_bar, LV_ALIGN_CENTER, 0, 40);
    lv_bar_set_value(loading_bar, 0, LV_ANIM_OFF);
    
    loading_label = lv_label_create(boot_screen);
    lv_label_set_text(loading_label, "Loading... 0%");
    lv_obj_set_style_text_font(loading_label, &lv_font_montserrat_14, 0);
    lv_obj_set_style_text_color(loading_label, lv_color_hex(0x8892b0), 0);
    lv_obj_align_to(loading_label, loading_bar, LV_ALIGN_OUT_BOTTOM_MID, 0, 15);
    
    loading_progress = 0;
    loading_timer = lv_timer_create(loading_timer_cb, 50, NULL);
}