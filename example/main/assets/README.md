# BMI Dashboard Assets

This directory contains icon assets for the BMI Dashboard demo.

## Icon Conversion Process

To convert your own icons to C arrays:

1. **Prepare Icons**: Create 24x24 pixel PNG icons with transparent backgrounds
2. **Use LVGL Image Converter**: 
   - Online tool: https://lvgl.io/tools/imageconverter
   - Settings: Color format RGB565, Output format C array
3. **Replace Arrays**: Copy the generated C arrays into `bmi_dashboard_icons.c`

## Current Icons

- `icon_spo2` - Oxygen molecule (cyan color #00BCD4)
- `icon_heart_rate` - Heart with pulse (red color #F44336) 
- `icon_temperature` - Thermometer (red color #F44336)
- `icon_pulse_wave` - ECG wave pattern (green color #4CAF50)

## Usage

Include the header file and use the icons:

```c
#include "assets/bmi_dashboard_icons.h"

lv_obj_t * img = lv_img_create(parent);
lv_img_set_src(img, &icon_spo2);
```

## Icon Specifications

- Size: 24x24 pixels
- Format: RGB565 (16-bit color)
- Background: Transparent
- Style: Simple, medical/health themed icons