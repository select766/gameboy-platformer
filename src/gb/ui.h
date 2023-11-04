#pragma once
#include <stdint.h>

#define UI_MODE_TITLE 0
#define UI_MODE_EDIT 1
#define UI_MODE_MANUAL_PLAY 2
#define UI_MODE_TRAIN 3
#define SPRITE_RUNNER 0
#define SPRITE_CURSOR 1

typedef struct {
    uint8_t current, last;
} JoypadWithLast;

void clear_line(uint8_t y);
void clear_print_area(void);
JoypadWithLast joypad_with_last(void);
