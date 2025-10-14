#ifndef INSTRUCTION_SCREEN_H
#define INSTRUCTION_SCREEN_H

#include "lvgl.h"
#include <stdbool.h>

void instruction_screen_create(void);
void instruction_update_mode_switch(bool switch_high);

#endif // INSTRUCTION_SCREEN_H