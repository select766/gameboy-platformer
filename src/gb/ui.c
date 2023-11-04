#include <stdio.h>
#include <gb/gb.h>
#include <gbdk/console.h>
#include <gbdk/platform.h>
#include "../common/stage_map.h"
#include "ui.h"

void clear_line(uint8_t y)
{
    gotoxy(0, y);
    printf("                    ");
}

void clear_print_area(void)
{
    for (uint8_t i = 0; i < STAGE_TOP_OFFSET; i++)
    {
        clear_line(i);
    }
}

JoypadWithLast joypad_with_last(void)
{
    static uint8_t last_button = 0;
    uint8_t current = joypad();
    JoypadWithLast s = {current, last_button};
    last_button = current;
    return s;
}
