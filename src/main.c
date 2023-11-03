#include <stdio.h>
#include <stdlib.h>
#include <gb/gb.h>
#include <gbdk/console.h>

#include <gbdk/platform.h>
#include <gbdk/font.h>
#include <gbdk/bcd.h>

#include "bg_tile.h"
#include "fg_tile.h"

#define BGTileCount 4
#define FGTileCount 2
#define TILE_SIZE 8
#define STAGE_TOP_OFFSET 8 // 画面上端からステージ上端までの距離（タイル単位）
#define STAGE_WIDTH 20  // タイル単位
#define STAGE_HEIGHT 10 // タイル単位
#define CHAR_HEIGHT 2   // キャラクターの高さ（タイル単位）
#define TILE_ID_EMPTY 0x80
#define TILE_ID_FLOOR 0x81
#define TILE_ID_GOAL 0x82


// ステージ。
const unsigned char MapData[] = {
0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x82,
0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x83,
0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x83,
0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x83,
0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x83,
0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x81,0x81,0x80,0x80,0x83,
0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x81,0x81,0x80,0x80,0x83,
0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x81,0x81,0x81,0x81,0x80,0x80,0x83,
0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x81,0x80,0x80,0x81,0x81,0x81,0x81,0x80,0x80,0x83,
0x81,0x81,0x81,0x81,0x81,0x81,0x80,0x80,0x81,0x81,0x81,0x81,0x81,0x81,0x81,0x81,0x81,0x81,0x81,0x81,
};

typedef struct
{
    uint8_t x; // 左端ピクセル
    uint8_t y; // 下端よりひとつ下のピクセル（上端だとマイナスになるので）
    int8_t ySpeed;
    uint8_t done; // ゴールしたら1
    uint16_t steps; // 現在のステップ数
    uint8_t score; // 現在のスコア(ゴール時にのみ加算される)
    uint8_t isOnFloor; // 床に接しているかどうか
    uint8_t isMissed; // 穴に落ちたかどうか
    const unsigned char *mapData;
} GameState;

typedef uint8_t GameAction;

void initGameState(GameState *state)
{
    // mapDataは初期化されない
    state->x = 0;
    state->y = TILE_SIZE * (STAGE_HEIGHT - 1); // 一番下のタイルの上端
    state->ySpeed = 0;
    state->done = 0;
    state->steps = 0;
    state->score = 0;
    state->isOnFloor = 0;
    state->isMissed = 0;
}

void stepGameState(GameState *state, GameAction action)
{
    if (state->done)
    {
        // ゴール済みならもう動かない
        return;
    }
    // 地面の高さを判定
    // x座標 [x, x + TILE_SIZE) にあるタイルを調べる
    // この方法では、浮いているブロックは扱えない
    uint8_t floorY = STAGE_HEIGHT * TILE_SIZE;
    {
        uint8_t tileXLow = state->x / TILE_SIZE;
        uint8_t tileXHigh = (state->x + TILE_SIZE - 1) / TILE_SIZE;
        for (uint8_t tileX = tileXLow; tileX <= tileXHigh; tileX++)
        {
            for (uint8_t tileY = 0; tileY < STAGE_HEIGHT; tileY++)
            {
                uint8_t tileId = state->mapData[tileY * STAGE_WIDTH + tileX];
                if (tileId != TILE_ID_EMPTY)
                {
                    uint8_t tileYTop = tileY * TILE_SIZE;
                    if (tileYTop < floorY)
                    {
                        floorY = tileYTop;
                    }
                    break;
                }
            }
        }
    }

    if (state->y >= floorY)
    {
        // 床に接している
        state->y = floorY;
        if (action)
        {
            state->ySpeed = -8;
        }
        else if (state->ySpeed > 0)
        {
            state->ySpeed = 0;
        }
    }
    else
    {
        // 浮いている
        state->ySpeed += 1;
    }
    state->y += state->ySpeed;

    // めりこみ補正(不完全)
    if (state->y >= floorY)
    {
        // 床に接している
        state->y = floorY;
        state->ySpeed = 0;
        state->isOnFloor = 1;
    }
    else
    {
        state->isOnFloor = 0;
    }

    // キャラクターの右辺が接しているものを判定
    uint8_t charRight = state->x + TILE_SIZE;
    // x座標 charRightかつ、y座標 [y, y + CHAR_HEIGHT * TILE_SIZE) にあるタイルを調べる
    uint8_t xCollision = 0;
    uint8_t goal = 0;
    {
        uint8_t tileX = charRight / TILE_SIZE;
        uint8_t tileYLow = (state->y - CHAR_HEIGHT * TILE_SIZE) / TILE_SIZE;
        uint8_t tileYHigh = (state->y - 1) / TILE_SIZE;
        for (uint8_t tileY = tileYLow; tileY <= tileYHigh; tileY++)
        {
            uint8_t tileId = state->mapData[tileY * STAGE_WIDTH + tileX];
            if (tileId == TILE_ID_EMPTY)
            {
                continue;
            }
            else if (tileId == TILE_ID_FLOOR)
            {
                xCollision = 1;
            }
            else
            {
                // ゴール
                goal = 1;
            }
        }
    }

    if (state->y >= STAGE_HEIGHT * TILE_SIZE)
    {
        // 穴に落ちた
        state->done = 1;
        state->isMissed = 1;
    }
    else if (goal)
    {
        state->done = 1;
        // ゴールした時の高さに応じたスコア。ステージ下端からキャラクターの下端までの距離がスコアになる。最小で1ブロック分。
        state->score = STAGE_HEIGHT * TILE_SIZE - state->y;
    }
    else
    {
        if (!xCollision)
        {
            state->x += 1;
        }
    }

    state->steps++;
}

void oneGame()
{
    GameState state;
    initGameState(&state);
    state.mapData = MapData;

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
        GameAction action = 0;
        if ((button & J_A) && !(lastButton & J_A))
        {
            action = 1;
        }
        lastButton = button;
        // 状態更新
        stepGameState(&state, action);
    }
}

void main()
{
    printf("PRESS START");
    DISPLAY_OFF;
    SHOW_BKG;
    SHOW_SPRITES;
    SPRITES_8x16;
    // 初回のputs/printfで、フォントデータ（背景データ）の設定、タイルの初期化が行われる。フォントデータと干渉しない場所で独自のタイルを設定する。文字が出力された座標だけでなく画面全体のタイルが初期化されることに注意。
    set_bkg_data(128, BGTileCount, BGTileLabel);
    set_bkg_tiles(0, STAGE_TOP_OFFSET, STAGE_WIDTH, STAGE_HEIGHT, MapData);

    set_sprite_data(0, FGTileCount, FGTileLabel);
    set_sprite_tile(0, 0);
    DISPLAY_ON;

    while (1)
    {
        wait_vbl_done();
        // 入力
        uint8_t button = joypad();
        if (button & J_START)
        {
            oneGame();
        }
    }
}
