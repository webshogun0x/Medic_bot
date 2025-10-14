#ifndef DISPLAY_MANAGER_H
#define DISPLAY_MANAGER_H

#include "lvgl.h"
#include "lv_demo_bmi_dashboard.h"

// Include all screen headers
#include "screens/boot_screen.h"

#include "screens/login_screen.h"
#include "screens/instruction_screen.h"
#include "screens/enroll_screen.h"
#include "screens/analytics_screen.h"
#include "screens/profile_screen.h"

// Main display initialization
void display_manager_init(void);

// Screen navigation functions
void display_show_boot_screen(void);
void display_show_instruction_screen(void);
void display_show_login_screen(void);
void display_show_enroll_screen(void);
void display_show_dashboard(void);

// UART setup
void setup_uart_receiver(void);
void send_uart_command(const char* command);

#endif // DISPLAY_MANAGER_H