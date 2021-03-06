/*
 * Copyright (c) Pavol Rusnak, SatoshiLabs
 *
 * Licensed under TREZOR License
 * see LICENSE file for details
 */

#define CMD(X) (void)(X);
#define DATA(X) (void)(X);

uint32_t trezorui_poll_event(void)
{
    return 0;
}

void display_init(void)
{
}

void display_set_window(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1)
{
}

void display_refresh(void)
{
}

int display_orientation(int degrees)
{
    if (degrees != ORIENTATION) {
        if (degrees == 0 || degrees == 90 || degrees == 180 || degrees == 270) {
            ORIENTATION = degrees;
        }
    }
    return ORIENTATION;
}

int display_backlight(int val)
{
    if (BACKLIGHT != val && val >= 0 && val <= 255) {
        BACKLIGHT = val;
    }
    return BACKLIGHT;
}

void display_save(const char *filename)
{
}
