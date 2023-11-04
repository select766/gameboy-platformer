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
#include "ui.h"
#include "manual_play.h"

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

static void initialize()
{
    // ゲームボーイカラーの場合、倍速モードを使用する(コンパイラにオプションが必要:-Wm-yc)
    if (_cpu == CGB_TYPE)
    {
        set_default_palette(); // これがないと背景が真っ白になる
        cpu_fast();
    }

    // タイトル画面（かつ、printfの初期化）
    printf("PLATMAKER GB\n       by @select766\n\nA: EDIT STAGE\nSTART: USE PRESET");

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
    set_sprite_tile(SPRITE_RUNNER, 0);
    DISPLAY_ON;
}

static uint8_t title_mode(void)
{
    while (1)
    {
        wait_vbl_done();
        JoypadWithLast button;
        button = joypad_with_last();
        uint8_t new_button = button.current & ~button.last;
        if (new_button & J_A)
        {
            return UI_MODE_EDIT;
        }
        if (new_button & J_START)
        {
            return UI_MODE_MANUAL_PLAY;
        }
    }
}

void main()
{
    initialize();

    uint8_t ui_mode = UI_MODE_TITLE;

    while (1)
    {
        wait_vbl_done();
        switch (ui_mode)
        {
        case UI_MODE_TITLE:
            ui_mode = title_mode();
            break;
        case UI_MODE_EDIT:
            ui_mode = stage_edit_mode();
            break;
        case UI_MODE_MANUAL_PLAY:
            ui_mode = manual_play_mode();
            break;
        case UI_MODE_TRAIN:
            train_main(); // ここからは戻らない
            break;
        }
    }
}
