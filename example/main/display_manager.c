#include "display_manager.h"
#include "screens/login_screen.h"
#include "driver/uart.h"
#include "driver/gpio.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_err.h"
#include <string.h>
#include <stdlib.h>

static bool is_instruction_screen_active = false;

void display_manager_init(void)
{
    display_show_boot_screen();
}

void display_show_boot_screen(void)
{
    boot_screen_create();
}

void display_show_instruction_screen(void)
{
    lv_obj_clean(lv_screen_active());
    is_instruction_screen_active = true;
    instruction_screen_create();
}

void display_show_login_screen(void)
{
    lv_obj_clean(lv_screen_active());
    is_instruction_screen_active = false;
    login_screen_create();
}

void display_show_enroll_screen(void)
{
    lv_obj_clean(lv_screen_active());
    is_instruction_screen_active = false;
    enroll_screen_create();
}

void display_show_dashboard(void)
{
    lv_obj_clean(lv_screen_active());
    is_instruction_screen_active = false;
    lv_obj_t * main_dashboard = lv_tabview_create(lv_screen_active());
    lv_tabview_set_tab_bar_size(main_dashboard, 60);
    
    lv_obj_t * profile_tab = lv_tabview_add_tab(main_dashboard, "Profile");
    lv_obj_t * analytics_tab = lv_tabview_add_tab(main_dashboard, "Analytics");
    
    analytics_screen_create(analytics_tab);
    profile_screen_init(profile_tab);
}

#define UART_PORT_NUM      UART_NUM_1
#define UART_BAUD_RATE     115200
#define UART_TX_PIN        17
#define UART_RX_PIN        18
#define BUF_SIZE           1024

static void uart_message_task(void *pvParameters) {
    uint8_t data[BUF_SIZE];
    display_message_t receivedMsg;
    
    while (1) {
        int len = uart_read_bytes(UART_PORT_NUM, data, BUF_SIZE - 1, pdMS_TO_TICKS(100));
        if (len > 0) {
            data[len] = '\0';
            
            // Parse JSON message from ESP32-S3
            char* type_start = strstr((char*)data, "\"type\":\"");
            char* msg_start = strstr((char*)data, "\"message\":\"");
            char* mode_start = strstr((char*)data, "\"mode_switch\":");
            
            // Parse mode_switch state only if instruction screen is active
            if (mode_start && is_instruction_screen_active) {
                mode_start += 14; // Skip '"mode_switch":'
                bool switch_high = (strncmp(mode_start, "true", 4) == 0);
                instruction_update_mode_switch(switch_high);
            }
            
            if (type_start && msg_start) {
                type_start += 8; // Skip '"type":"'
                msg_start += 11; // Skip '"message":"'
                
                char* type_end = strchr(type_start, '"');
                char* msg_end = strchr(msg_start, '"');
                
                if (type_end && msg_end) {
                    *type_end = '\0';
                    *msg_end = '\0';
                    
                    memset(&receivedMsg, 0, sizeof(receivedMsg));
                    strncpy(receivedMsg.message, msg_start, sizeof(receivedMsg.message) - 1);
                    
                    // Route to appropriate screen based on message type
                    if (strcmp(type_start, "PROMPT") == 0) {
                        receivedMsg.msg_type = 1; // Blue for prompts
                        display_message_handler(&receivedMsg);
                        
                        // Check if message contains RFID detection
                        if (strstr(msg_start, "RFID Detected:")) {
                            char* rfid_start = strstr(msg_start, "RFID Detected: ");
                            if (rfid_start) {
                                rfid_start += 15; // Skip "RFID Detected: "
                                login_update_rfid(rfid_start);
                            }
                        }
                    } else if (strcmp(type_start, "FINGERPRINT_SUCCESS") == 0) {
                        login_update_fingerprint_status(true, msg_start);
                    } else if (strcmp(type_start, "FINGERPRINT_ERROR") == 0) {
                        login_update_fingerprint_status(false, msg_start);
                    } else if (strcmp(type_start, "USER_DATA") == 0) {
                        receivedMsg.msg_type = 3; // Green for success
                        display_show_dashboard();
                        display_message_handler(&receivedMsg);
                    } else if (strcmp(type_start, "SENSOR_DATA") == 0) {
                        receivedMsg.msg_type = 1; // Blue for data
                        display_message_handler(&receivedMsg);
                        
                        // Parse all sensor data from JSON
                        char* hr_start = strstr((char*)data, "\"heart_rate\":");
                        char* spo2_start = strstr((char*)data, "\"spo2\":");
                        char* temp_start = strstr((char*)data, "\"temperature\":");
                        char* bmi_start = strstr((char*)data, "\"bmi\":");
                        
                        float hr = 72.0f, spo2 = 98.0f, temp = 98.6f, bmi = 24.8f;
                        
                        if (hr_start) {
                            hr_start += 13; // Skip '"heart_rate":'
                            hr = atof(hr_start);
                        }
                        if (spo2_start) {
                            spo2_start += 8; // Skip '"spo2":'
                            spo2 = atof(spo2_start);
                        }
                        if (temp_start) {
                            temp_start += 14; // Skip '"temperature":'
                            temp = atof(temp_start);
                        }
                        if (bmi_start) {
                            bmi_start += 7; // Skip '"bmi":'
                            bmi = atof(bmi_start);
                        }
                        
                        analytics_update_readings(bmi, temp, (uint8_t)hr, (uint8_t)spo2);
                        
                    }
                }
            }
        }
    }
}

void setup_uart_receiver(void)
{
    uart_config_t uart_config = {
        .baud_rate = UART_BAUD_RATE,
        .data_bits = UART_DATA_8_BITS,
        .parity = UART_PARITY_DISABLE,
        .stop_bits = UART_STOP_BITS_1,
        .flow_ctrl = UART_HW_FLOWCTRL_DISABLE,
    };
    
    uart_driver_install(UART_PORT_NUM, BUF_SIZE * 2, 0, 0, NULL, 0);
    uart_param_config(UART_PORT_NUM, &uart_config);
    uart_set_pin(UART_PORT_NUM, UART_TX_PIN, UART_RX_PIN, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE);
    
    xTaskCreate(uart_message_task, "uart_msg_task", 4096, NULL, 2, NULL);
}



void send_uart_command(const char* command)
{
    if (command) {
        uart_write_bytes(UART_PORT_NUM, command, strlen(command));
        uart_write_bytes(UART_PORT_NUM, "\n", 1);
    }
}