#include <stdio.h>
#include <stdlib.h>
#include <gb/gb.h>
#include <gbdk/console.h>

#include "../common/platformer.h"
#include "ui.h"

uint8_t stage_edit_mode()
{
    clear_print_area();
    gotoxy(0, 0);
    printf("STAGE EDIT MODE\nA: MAKE FLOOR\nSTART: EXIT");
    wait_vbl_done();
    set_sprite_tile(SPRITE_CURSOR, 2); // カーソルのスプライト
    uint8_t x = 1, y = STAGE_HEIGHT - 2; // カーソルの位置。スタートの1つ右、床の1つ上を初期値にする。

    while (1)
    {
        wait_vbl_done();
        move_sprite(SPRITE_CURSOR, x * TILE_SIZE + 8 + 4, y * TILE_SIZE + STAGE_TOP_OFFSET * TILE_SIZE + 16 - 4);
        set_bkg_tiles(0, STAGE_TOP_OFFSET, STAGE_WIDTH, STAGE_HEIGHT, StageMap);

        JoypadWithLast button;
        button = joypad_with_last();
        uint8_t new_button = button.current & ~button.last;
        if (new_button & J_START)
        {
            hide_sprite(SPRITE_CURSOR);
            return UI_MODE_MANUAL_PLAY;
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
        if (new_button & J_DOWN)
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
                // ブロックを6段以上積むとジャンプしたときに高さがオーバーフローする問題が起こる。
                if (end_of_empty < 5)
                {
                    end_of_empty = 5;
                }
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
    }
}
