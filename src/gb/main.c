#include <stdio.h>
#include <stdlib.h>
#include <gb/gb.h>
#include <gbdk/console.h>
#include <gb/cgb.h>

#include "memcpy.h"
#include "../common/platformer.h"
#include "bg_tile.h"
#include "fg_tile.h"
#include "train.h"
#include "stage_edit.h"

#define BGTileCount 4
#define FGTileCount 4

void oneGame()
{
    PlatformerState state;
    PlatformerReset(&state);

    uint8_t lastButton = 0;
    while (1)
    {
        wait_vbl_done();
        wait_vbl_done();
        // 表示
        // y座標は、move_spriteのオフセット16と、上端座標計算のstate.y-16が相殺
        move_sprite(0, (uint8_t)(state.x + 8), (uint8_t)(state.y + STAGE_TOP_OFFSET * TILE_SIZE));
        gotoxy(0, 0);
        printf("STEPS %u SCORE %u       ", state.steps, state.score);
        if (state.done)
        {
            return;
        }
        // 入力
        uint8_t button = joypad();
        PlatformerAction action = 0;
        if ((button & J_A) && !(lastButton & J_A))
        {
            action = 1;
        }
        lastButton = button;
        // 状態更新
        PlatformerStep(&state, action);
    }
}

void main()
{
    // ゲームボーイカラーの場合、倍速モードを使用する(コンパイラにオプションが必要:-Wm-yc)
    if (_cpu == CGB_TYPE)
    {
        set_default_palette(); // これがないと背景が真っ白になる
        cpu_fast();
    }
    printf("START TO PLAY\nB TO TRAIN\nA TO EDIT");
    DISPLAY_OFF;
    SHOW_BKG;
    SHOW_SPRITES;
    SPRITES_8x16;
    memcpy(StageMap, PresetStageMap, sizeof(StageMap));
    // 初回のputs/printfで、フォントデータ（背景データ）の設定、タイルの初期化が行われる。フォントデータと干渉しない場所で独自のタイルを設定する。文字が出力された座標だけでなく画面全体のタイルが初期化されることに注意。
    set_bkg_data(128, BGTileCount, BGTileLabel);
    set_bkg_tiles(0, STAGE_TOP_OFFSET, STAGE_WIDTH, STAGE_HEIGHT, StageMap);
    set_bkg_tiles(STAGE_WIDTH, STAGE_TOP_OFFSET, 1, STAGE_HEIGHT, GoalMap);

    set_sprite_data(0, FGTileCount, FGTileLabel);
    set_sprite_tile(0, 0);
    DISPLAY_ON;

    while (1)
    {
        wait_vbl_done();
        // 入力
        uint8_t button = joypad();
        if (button & J_A)
        {
            stage_edit_mode();
        }
        if (button & J_START)
        {
            oneGame();
        }
        if (button & J_B)
        {
            train_main();
        }
    }
}
