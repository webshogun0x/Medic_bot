#include "display_manager.h"

void app_main(void)
{
    // Initialize LVGL and display hardware (your existing setup code here)
    
    // Initialize the display manager which handles all screens
    display_manager_init();
    
    // Setup ESP-NOW receiver for communication
    setup_espnow_receiver();
}