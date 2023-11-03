#include <stdio.h>
#include <stdlib.h>
#include <gb/gb.h>
#include <gbdk/console.h>
#include <gbdk/platform.h>

#include "../int_learning/config.h"
#include "../int_learning/qlearning.h"
#include "train.h"

#define STEP_CYCLE 2
#define DONE_WAIT 30 // エピソード終了後の待ちフレーム数(*STEP_CYCLE)

typedef struct
{
    QLearningState *q_state;
    PlatformerState state;
    uint8_t cycle;
    uint8_t updated;
    int epoch;
    uint8_t done_wait;
} TrainState;

static TrainState *ts;

// VBL割り込み関数側では、現在のモデルを用いて独立にエピソードを実行し、車の動きを可視化する。
void train_vbl()
{
    const QLearningState *q_state = ts->q_state;

    if (--ts->cycle == 0)
    {
        ts->cycle = STEP_CYCLE;
        // 表示
        // y座標は、move_spriteのオフセット16と、上端座標計算のstate.y-16が相殺
        move_sprite(0, (uint8_t)(ts->state.x + 8), (uint8_t)(ts->state.y + STAGE_TOP_OFFSET * TILE_SIZE));
        if (ts->done_wait)
        {
            ts->done_wait--;
            if (!ts->done_wait)
            {
                PlatformerReset(&ts->state);
            }
        }
        else
        {
            // 行動取得
            PlatformerAction action = GetBestAction(q_state, &ts->state);
            // 状態更新
            PlatformerStep(&ts->state, action);
            if (ts->state.done)
            {
                ts->done_wait = DONE_WAIT;
            }
        }
    }

    if (ts->updated)
    {
        ts->updated = 0;
        gotoxy(0, 0);
        printf("%d episodes", ts->epoch);
    }
}

void train_main()
{
    // 画面クリア
    gotoxy(0, 0);
    for (uint8_t i = 0; i < 40; i++)
    {
        putchar(' ');
    }
    ts = malloc(sizeof(TrainState));
    PlatformerReset(&ts->state);
    QLearningState *q_state = QLearningStateCreate();
    ts->q_state = q_state;
    ts->cycle = STEP_CYCLE;
    ts->updated = 1;
    ts->epoch = 0;
    ts->done_wait = 0;

    __critical
    {
        add_VBL(train_vbl);
    }

    // main関数側では、割り込み処理をしていない間ずっとモデルの学習を行う。
    int epoch = 0;
    while (1)
    {
        initrand((unsigned int)epoch);
        TrainEpisode(q_state);
        // テストにランダム性はないため、ここで評価は不要。表示側で実行したエピソードのスコアを表示する。

        epoch++;
        __critical
        {
            ts->epoch = epoch;
            ts->updated = 1;
        }
    }
}
