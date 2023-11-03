#pragma once
#include "../common/platformer.h"
#define N_STATE_0 (STAGE_WIDTH * TILE_SIZE) // x座標
#define N_STATE_1 3 // 床に接してる、空中で速度が上向き、空中で速度が下向き
#define N_ACTIONS 2 // ジャンプしない、ジャンプする

typedef struct {
    Reward q_table[N_STATE_0 * N_STATE_1][N_ACTIONS];
} QLearningState;

typedef struct {
    Reward totalReward;
    uint8_t isMissed;
    uint8_t steps;
    uint8_t score;
} EpisodeResult;

QLearningState *QLearningStateCreate();
PlatformerAction GetBestAction(const QLearningState *q_state, const PlatformerState* state);
void TestEpisode(const QLearningState *q_state, EpisodeResult *episode_result);
Reward TrainEpisode(QLearningState *q_state);
