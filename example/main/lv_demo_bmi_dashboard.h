#ifndef LV_DEMO_BMI_DASHBOARD_H
#define LV_DEMO_BMI_DASHBOARD_H

#ifdef __cplusplus
extern "C" {
#endif

#include "lvgl.h"
#include <stdint.h>

typedef struct {
    char name[64];
    char gender[16];
    int age;
    float weight;
    float height;
    float bmi_data[12];
    uint8_t heart_rate;
} profile_data_t;

typedef struct {
    char message[256];
    uint8_t msg_type;
} display_message_t;

void lv_demo_bmi_dashboard(void);
void display_message_handler(const display_message_t * msg);
void profile_screen_init(lv_obj_t * parent);
void profile_update_data(const profile_data_t * data);
void profile_get_data(profile_data_t * data);
void setup_uart_receiver(void);

#ifdef __cplusplus
}
#endif

#endif /* LV_DEMO_BMI_DASHBOARD_H */