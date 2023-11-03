#pragma once
#include <stdint.h>

extern const unsigned char StageMap[]; // マップデータ

typedef struct
{
    uint16_t steps; // 現在のステップ数
    uint8_t x; // 左端ピクセル
    uint8_t y; // 下端よりひとつ下のピクセル（上端だとマイナスになるので）
    int8_t ySpeed; // y方向の速度(正なら落ちる方向)
    uint8_t done; // ゲームが終了したら1(ミスかゴール)
    uint8_t score; // 現在のスコア(ゴール時にのみ加算される)
    uint8_t isOnFloor; // 床に接しているかどうか
    uint8_t isMissed; // 穴に落ちたか・またはタイムアップしたかどうか（doneかつゴールしていない場合）
} PlatformerState;

typedef uint8_t PlatformerAction;
typedef int16_t Reward;

#define TILE_SIZE 8
#define STAGE_TOP_OFFSET 8 // 画面上端からステージ上端までの距離（タイル単位）
#define STAGE_WIDTH 20 // タイル単位
#define STAGE_HEIGHT 10 // タイル単位
#define CHAR_HEIGHT 2 // キャラクターの高さ（タイル単位）
#define TILE_ID_EMPTY 0x80
#define TILE_ID_FLOOR 0x81
#define TILE_ID_GOAL 0x82
#define MAX_EPISODE_LEN 1000 // このステップ数で終了
#define REWARD_SCALE 128 // 報酬のスケール。
#define REWARD_MISS (-10 * REWARD_SCALE) // 穴に落ちた時・タイムアップの報酬

void PlatformerReset(PlatformerState *state);
Reward PlatformerStep(PlatformerState *state, PlatformerAction action);
