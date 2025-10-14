#ifndef LOGIN_SCREEN_H
#define LOGIN_SCREEN_H

#include "lvgl.h"
#include <stdbool.h>

void login_screen_create(void);
void login_update_rfid(const char* rfid_number);
void login_update_fingerprint_status(bool success, const char* message);

#endif // LOGIN_SCREEN_H