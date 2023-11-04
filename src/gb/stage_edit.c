#include <stdio.h>
#include <stdlib.h>
#include <gb/gb.h>
#include <gbdk/console.h>

#include "../common/platformer.h"

#define SPRITE_ID 1

void stage_edit_mode()
{
    gotoxy(0, 0);
    printf("STAGE EDIT MODE\nA TO MAKE FLOOR\nSELECT TO EXIT\n");
    wait_vbl_done();
    set_sprite_tile(SPRITE_ID, 2);

    uint8_t x = 1, y = STAGE_HEIGHT - 2;
    uint8_t last_button = 0xFF;
    while (1)
    {
        wait_vbl_done();
        move_sprite(SPRITE_ID, x * TILE_SIZE + 8 + 4, y * TILE_SIZE + STAGE_TOP_OFFSET * TILE_SIZE + 16 - 4);
        set_bkg_tiles(0, STAGE_TOP_OFFSET, STAGE_WIDTH, STAGE_HEIGHT, StageMap);

        uint8_t button = joypad();
        uint8_t new_button = button & ~last_button;
        if (new_button & J_SELECT)
        {
            hide_sprite(SPRITE_ID);
            return;
        }
        if (new_button & J_LEFT)
        {
            if (x > 0) {
                x--;
            }
        }
        if (new_button & J_RIGHT)
        {
            if (x < STAGE_WIDTH - 1) {
                x++;
            }
        }
        if (new_button & J_UP)
        {
            if (y > 0) {
                y--;
            }
        }
        if (button & J_DOWN)
        {
            if (y < STAGE_HEIGHT - 1) {
                y++;
            }
        }
        if (new_button & J_A)
        {
            // 高さyまでのタイルを床にする。
            // 例外として、y == STAGE_HEIGHT - 1 かつ現在の床の高さが1の場合は、穴にする。
            uint8_t end_of_empty = 0;
            if ((y == STAGE_HEIGHT - 1) && (StageMap[(STAGE_HEIGHT - 1) * STAGE_WIDTH + x] == TILE_ID_FLOOR) && (StageMap[(STAGE_HEIGHT - 2) * STAGE_WIDTH + x] == TILE_ID_EMPTY))
            {
                // 穴にする
                end_of_empty = STAGE_HEIGHT;
            }
            else
            {
                // 床にする
                end_of_empty = y;
            }
            for (uint8_t i = 0; i < end_of_empty; i++)
            {
                StageMap[i * STAGE_WIDTH + x] = TILE_ID_EMPTY;
            }
            for (uint8_t i = end_of_empty; i < STAGE_HEIGHT; i++)
            {
                StageMap[i * STAGE_WIDTH + x] = TILE_ID_FLOOR;
            }
        }
        last_button = button;
    }
}
