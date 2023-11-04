#include <stdio.h>
#include <stdlib.h>
#include <gb/gb.h>
#include <gbdk/console.h>
#include <gbdk/platform.h>

#include "../int_learning/config.h"
#include "../int_learning/qlearning.h"
#include "train.h"
#include "ui.h"
#include "manual_play.h"

#define STEP_CYCLE 2
#define DONE_WAIT 30 // エピソード終了後の待ちフレーム数(*STEP_CYCLE)

typedef struct
{
    QLearningState *q_state;
    PlatformerState state;
    uint8_t cycle;
    uint8_t updated;
    int16_t trained_episodes;
    uint8_t done_wait;
    int16_t episode_reward;
    int16_t last_score;
    int16_t best_score;
    // int16_t best_at_episode;
    uint8_t time_frame;
    uint8_t time_sec;
    uint8_t time_min;
} TrainState;

static TrainState *ts;

// VBL割り込み関数側では、現在のモデルを用いて独立にエピソードを実行し、車の動きを可視化する。
void train_vbl()
{
    const QLearningState *q_state = ts->q_state;

    ts->time_frame++;
    if (ts->time_frame == 60)
    {
        ts->time_frame = 0;
        ts->time_sec++;
        if (ts->time_sec == 60)
        {
            ts->time_sec = 0;
            ts->time_min++;
        }

        gotoxy(14, 2);
        if (ts->time_sec < 10)
        {
            // %02dという指定ができないので、0埋めは自分でやる
            printf("%d:0%d", ts->time_min, ts->time_sec);
        }
        else
        {
            printf("%d:%d", ts->time_min, ts->time_sec);
        }
    }

    if (--ts->cycle == 0)
    {
        ts->cycle = STEP_CYCLE;
        // 表示
        // y座標は、move_spriteのオフセット16と、上端座標計算のstate.y-16が相殺
        move_sprite(SPRITE_RUNNER, (uint8_t)(ts->state.x + 8), (uint8_t)(ts->state.y + STAGE_TOP_OFFSET * TILE_SIZE));
        if (ts->done_wait)
        {
            ts->done_wait--;
            if (!ts->done_wait)
            {
                PlatformerReset(&ts->state);
                ts->episode_reward = 0;
            }
        }
        else
        {
            // 行動取得
            PlatformerAction action = GetBestAction(q_state, &ts->state);
            // 状態更新
            ts->episode_reward += PlatformerStep(&ts->state, action) / REWARD_SCALE;
            if (ts->state.done)
            {
                ts->last_score = ts->episode_reward;
                if (ts->episode_reward > ts->best_score)
                {
                    ts->best_score = ts->episode_reward;
                    // ts->best_at_episode = ts->trained_episodes;
                }
                ts->done_wait = DONE_WAIT;
                gotoxy(0, 3);
                printf("SCORE %d    \nBEST  %d    \nHUMAN %d", ts->last_score, ts->best_score, human_best_score);
            }
        }
    }

    if (ts->updated)
    {
        ts->updated = 0;
        gotoxy(0, 2);
        printf("%d episodes", ts->trained_episodes);
    }
}

void train_main()
{
    clear_print_area();
    gotoxy(0, 0);
    printf("AI TRAINING MODE");
    ts = malloc(sizeof(TrainState));
    PlatformerReset(&ts->state);
    QLearningState *q_state = QLearningStateCreate();
    ts->q_state = q_state;
    ts->cycle = STEP_CYCLE;
    ts->updated = 1;
    ts->trained_episodes = 0;
    ts->done_wait = 0;
    ts->episode_reward = 0;
    ts->last_score = 0;
    ts->best_score = 0;
    // ts->best_at_episode = 0;
    ts->time_frame = 0;
    ts->time_sec = 0;
    ts->time_min = 0;

    __critical
    {
        add_VBL(train_vbl);
    }

    // main関数側では、割り込み処理をしていない間ずっとモデルの学習を行う。
    int trained_episodes = 0;
    while (1)
    {
        initrand((unsigned int)trained_episodes);
        TrainEpisode(q_state);
        // テストにランダム性はないため、ここで評価は不要。表示側で実行したエピソードのスコアを表示する。

        trained_episodes++;
        ts->trained_episodes = trained_episodes;
        ts->updated = 1;
    }
}
