idf_component_register(SRCS main.c 
                             lv_demo_bmi_reading.c 
                             lv_demo_bmi_dashboard.c 
                             display_manager.c
                             screens/boot_screen.c
                             screens/login_screen.c
                             screens/instruction_screen.c
                             screens/enroll_screen.c
                             screens/analytics_screen.c
                             screens/profile_screen.c
                             dashboard/dashboard_main.c
                             data/health_data.c
                             analytics/analytics_screen.c
                             profile/bmi_profile.c
                             assets/img_spo2_icons.c 
                             assets/icon_heart_beat.c 
                             assets/icon_temp.c 
                             assets/icon_bpm.c
                    INCLUDE_DIRS . assets dashboard data analytics profile screens
                    REQUIRES esp_lcd driver)
