#include "login_screen.h"
#include "display_manager.h"

static lv_obj_t * login_screen;
static lv_obj_t * keyboard;
static lv_obj_t * active_textarea = NULL;
static lv_obj_t * rfid_display;
static lv_obj_t * fname_ta;
static lv_obj_t * lname_ta;
static lv_obj_t * email_ta;
static lv_obj_t * login_btn;
static lv_obj_t * fingerprint_status;
static lv_obj_t * fingerprint_icon;

static void back_btn_cb(lv_event_t * e)
{
    lv_obj_delete(login_screen);
    display_show_instruction_screen();
}

static void textarea_focus_cb(lv_event_t * e)
{
    lv_obj_t * ta = lv_event_get_target(e);
    if(keyboard != NULL) {
        lv_keyboard_set_textarea(keyboard, ta);
        active_textarea = ta;
    }
}

static void keyboard_ready_cb(lv_event_t * e)
{
    lv_keyboard_def_event_cb(e);
    
    if(lv_event_get_code(e) == LV_EVENT_READY || lv_event_get_code(e) == LV_EVENT_CANCEL) {
        lv_obj_add_flag(keyboard, LV_OBJ_FLAG_HIDDEN);
        active_textarea = NULL;
    }
}

static void login_btn_cb(lv_event_t * e)
{
    lv_obj_delete(login_screen);
    display_show_dashboard();
}

void login_screen_create(void)
{
    login_screen = lv_obj_create(lv_screen_active());
    lv_obj_set_size(login_screen, LV_PCT(100), LV_PCT(100));
    lv_obj_set_style_bg_color(login_screen, lv_color_hex(0xf8f9fa), 0);
    lv_obj_set_style_border_width(login_screen, 0, 0);
    lv_obj_set_style_pad_all(login_screen, 20, 0);
    
    // Back button
    lv_obj_t * back_btn = lv_btn_create(login_screen);
    lv_obj_set_size(back_btn, 80, 40);
    lv_obj_set_style_bg_color(back_btn, lv_color_hex(0x6c757d), 0);
    lv_obj_align(back_btn, LV_ALIGN_TOP_LEFT, 10, 10);
    lv_obj_add_event_cb(back_btn, back_btn_cb, LV_EVENT_CLICKED, NULL);
    
    lv_obj_t * back_label = lv_label_create(back_btn);
    lv_label_set_text(back_label, "Back");
    lv_obj_set_style_text_color(back_label, lv_color_white(), 0);
    lv_obj_center(back_label);
    
    // Title
    lv_obj_t * title = lv_label_create(login_screen);
    lv_label_set_text(title, "Login");
    lv_obj_set_style_text_font(title, &lv_font_montserrat_16, 0);
    lv_obj_set_style_text_color(title, lv_color_hex(0x2c3e50), 0);
    lv_obj_align(title, LV_ALIGN_TOP_MID, 0, 20);
    
    // Instructions
    lv_obj_t * instructions = lv_label_create(login_screen);
    lv_label_set_text(instructions, 
        "Please make sure the switch is ON (position 1), then fill in your details.\\n"
        "After completing the form, place your RFID card on the reader,\\n"
        "wait for the RFID number to display, then place your registered finger\\n"
        "on the fingerprint scanner to login.");
    lv_obj_set_style_text_align(instructions, LV_TEXT_ALIGN_CENTER, 0);
    lv_obj_set_style_text_font(instructions, &lv_font_montserrat_12, 0);
    lv_obj_set_style_text_color(instructions, lv_color_hex(0x495057), 0);
    lv_obj_set_style_bg_color(instructions, lv_color_hex(0xe3f2fd), 0);
    lv_obj_set_style_bg_opa(instructions, LV_OPA_COVER, 0);
    lv_obj_set_style_pad_all(instructions, 10, 0);
    lv_obj_set_style_radius(instructions, 5, 0);
    lv_obj_set_width(instructions, LV_PCT(90));
    lv_obj_align(instructions, LV_ALIGN_TOP_MID, 0, 60);
    
    // RFID Display Section
    lv_obj_t * rfid_container = lv_obj_create(login_screen);
    lv_obj_set_size(rfid_container, LV_PCT(80), 60);
    lv_obj_set_style_bg_color(rfid_container, lv_color_hex(0xfff3cd), 0);
    lv_obj_set_style_border_width(rfid_container, 1, 0);
    lv_obj_set_style_border_color(rfid_container, lv_color_hex(0xffeaa7), 0);
    lv_obj_set_style_radius(rfid_container, 8, 0);
    lv_obj_set_style_pad_all(rfid_container, 15, 0);
    lv_obj_align(rfid_container, LV_ALIGN_TOP_MID, 0, 140);
    
    lv_obj_t * rfid_label = lv_label_create(rfid_container);
    lv_label_set_text(rfid_label, "RFID Card:");
    lv_obj_set_style_text_font(rfid_label, &lv_font_montserrat_14, 0);
    lv_obj_align(rfid_label, LV_ALIGN_TOP_LEFT, 0, 0);
    
    rfid_display = lv_label_create(rfid_container);
    lv_label_set_text(rfid_display, "Waiting for RFID scan...");
    lv_obj_set_style_text_font(rfid_display, &lv_font_montserrat_16, 0);
    lv_obj_set_style_text_color(rfid_display, lv_color_hex(0x856404), 0);
    lv_obj_align_to(rfid_display, rfid_label, LV_ALIGN_OUT_BOTTOM_LEFT, 0, 5);
    
    // Form container
    lv_obj_t * form = lv_obj_create(login_screen);
    lv_obj_set_size(form, LV_PCT(80), 220);
    lv_obj_set_style_bg_color(form, lv_color_white(), 0);
    lv_obj_set_style_border_width(form, 1, 0);
    lv_obj_set_style_border_color(form, lv_color_hex(0xdee2e6), 0);
    lv_obj_set_style_radius(form, 8, 0);
    lv_obj_set_style_pad_all(form, 20, 0);
    lv_obj_align_to(form, rfid_container, LV_ALIGN_OUT_BOTTOM_MID, 0, 10);
    
    // First Name
    lv_obj_t * fname_label = lv_label_create(form);
    lv_label_set_text(fname_label, "First Name:");
    lv_obj_set_style_text_font(fname_label, &lv_font_montserrat_14, 0);
    lv_obj_align(fname_label, LV_ALIGN_TOP_LEFT, 0, 0);
    
    fname_ta = lv_textarea_create(form);
    lv_obj_set_size(fname_ta, LV_PCT(100), 40);
    lv_textarea_set_placeholder_text(fname_ta, "Enter first name");
    lv_textarea_set_one_line(fname_ta, true);
    lv_obj_align_to(fname_ta, fname_label, LV_ALIGN_OUT_BOTTOM_LEFT, 0, 5);
    lv_obj_add_event_cb(fname_ta, textarea_focus_cb, LV_EVENT_FOCUSED, NULL);
    
    // Last Name
    lv_obj_t * lname_label = lv_label_create(form);
    lv_label_set_text(lname_label, "Last Name:");
    lv_obj_set_style_text_font(lname_label, &lv_font_montserrat_14, 0);
    lv_obj_align_to(lname_label, fname_ta, LV_ALIGN_OUT_BOTTOM_LEFT, 0, 15);
    
    lname_ta = lv_textarea_create(form);
    lv_obj_set_size(lname_ta, LV_PCT(100), 40);
    lv_textarea_set_placeholder_text(lname_ta, "Enter last name");
    lv_textarea_set_one_line(lname_ta, true);
    lv_obj_align_to(lname_ta, lname_label, LV_ALIGN_OUT_BOTTOM_LEFT, 0, 5);
    lv_obj_add_event_cb(lname_ta, textarea_focus_cb, LV_EVENT_FOCUSED, NULL);
    
    // Email
    lv_obj_t * email_label = lv_label_create(form);
    lv_label_set_text(email_label, "Email:");
    lv_obj_set_style_text_font(email_label, &lv_font_montserrat_14, 0);
    lv_obj_align_to(email_label, lname_ta, LV_ALIGN_OUT_BOTTOM_LEFT, 0, 15);
    
    email_ta = lv_textarea_create(form);
    lv_obj_set_size(email_ta, LV_PCT(100), 40);
    lv_textarea_set_placeholder_text(email_ta, "Enter email address");
    lv_textarea_set_one_line(email_ta, true);
    lv_obj_align_to(email_ta, email_label, LV_ALIGN_OUT_BOTTOM_LEFT, 0, 5);
    lv_obj_add_event_cb(email_ta, textarea_focus_cb, LV_EVENT_FOCUSED, NULL);
    
    // Fingerprint verification section
    lv_obj_t * fp_container = lv_obj_create(form);
    lv_obj_set_size(fp_container, LV_PCT(100), 50);
    lv_obj_set_style_bg_color(fp_container, lv_color_hex(0xf8f9fa), 0);
    lv_obj_set_style_border_width(fp_container, 1, 0);
    lv_obj_set_style_border_color(fp_container, lv_color_hex(0xdee2e6), 0);
    lv_obj_set_style_radius(fp_container, 5, 0);
    lv_obj_set_style_pad_all(fp_container, 10, 0);
    lv_obj_align_to(fp_container, email_ta, LV_ALIGN_OUT_BOTTOM_LEFT, 0, 15);
    
    fingerprint_icon = lv_label_create(fp_container);
    lv_label_set_text(fingerprint_icon, "⏳");
    lv_obj_set_style_text_font(fingerprint_icon, &lv_font_montserrat_16, 0);
    lv_obj_align(fingerprint_icon, LV_ALIGN_LEFT_MID, 0, 0);
    
    fingerprint_status = lv_label_create(fp_container);
    lv_label_set_text(fingerprint_status, "Waiting for fingerprint scan...");
    lv_obj_set_style_text_font(fingerprint_status, &lv_font_montserrat_12, 0);
    lv_obj_set_style_text_color(fingerprint_status, lv_color_hex(0x6c757d), 0);
    lv_obj_align_to(fingerprint_status, fingerprint_icon, LV_ALIGN_OUT_RIGHT_MID, 10, 0);
    
    // Login Button (initially disabled)
    login_btn = lv_btn_create(form);
    lv_obj_set_size(login_btn, 120, 45);
    lv_obj_set_style_bg_color(login_btn, lv_color_hex(0x28a745), 0);
    lv_obj_set_style_bg_color(login_btn, lv_color_hex(0x6c757d), LV_STATE_DISABLED);
    lv_obj_align_to(login_btn, fp_container, LV_ALIGN_OUT_BOTTOM_RIGHT, 0, 15);
    lv_obj_add_state(login_btn, LV_STATE_DISABLED);
    lv_obj_add_event_cb(login_btn, login_btn_cb, LV_EVENT_CLICKED, NULL);
    
    lv_obj_t * login_label = lv_label_create(login_btn);
    lv_label_set_text(login_label, "Login");
    lv_obj_set_style_text_color(login_label, lv_color_white(), 0);
    lv_obj_center(login_label);
    
    // Create keyboard (initially hidden)
    keyboard = lv_keyboard_create(login_screen);
    lv_obj_set_size(keyboard, LV_PCT(100), LV_PCT(40));
    lv_obj_align(keyboard, LV_ALIGN_BOTTOM_MID, 0, 0);
    lv_obj_add_event_cb(keyboard, keyboard_ready_cb, LV_EVENT_ALL, NULL);
    lv_obj_add_flag(keyboard, LV_OBJ_FLAG_HIDDEN);
}

void login_update_rfid(const char* rfid_number) {
    if (rfid_display && rfid_number) {
        lv_label_set_text(rfid_display, rfid_number);
        
        // Auto-populate name fields
        if (fname_ta) {
            lv_textarea_set_text(fname_ta, "Oluwatimileyin");
        }
        if (lname_ta) {
            lv_textarea_set_text(lname_ta, "Idowu");
        }
        
        // Update fingerprint status
        if (fingerprint_status) {
            lv_label_set_text(fingerprint_status, "Please scan your fingerprint...");
            lv_obj_set_style_text_color(fingerprint_status, lv_color_hex(0x007bff), 0);
        }
    }
}

void login_update_fingerprint_status(bool success, const char* message) {
    if (fingerprint_icon && fingerprint_status) {
        if (success) {
            lv_label_set_text(fingerprint_icon, "✓");
            lv_obj_set_style_text_color(fingerprint_icon, lv_color_hex(0x28a745), 0);
            lv_label_set_text(fingerprint_status, "Fingerprint verified!");
            lv_obj_set_style_text_color(fingerprint_status, lv_color_hex(0x28a745), 0);
            
            // Enable login button
            if (login_btn) {
                lv_obj_clear_state(login_btn, LV_STATE_DISABLED);
                lv_obj_set_style_bg_color(login_btn, lv_color_hex(0x28a745), 0);
            }
        } else {
            lv_label_set_text(fingerprint_icon, "✗");
            lv_obj_set_style_text_color(fingerprint_icon, lv_color_hex(0xdc3545), 0);
            lv_label_set_text(fingerprint_status, message ? message : "Fingerprint verification failed");
            lv_obj_set_style_text_color(fingerprint_status, lv_color_hex(0xdc3545), 0);
        }
    }
}