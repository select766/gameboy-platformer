#pragma once
#include "../common/platformer.h"
#define N_STATE_0 (STAGE_WIDTH * TILE_SIZE) // x座標
#define N_STATE_1 3 // 床に接してる、空中で速度が上向き、空中で速度が下向き
#define N_ACTIONS 2 // ジャンプしない、ジャンプする

typedef struct {
    double q_table[N_STATE_0 * N_STATE_1][N_ACTIONS];
    double gamma, alpha, epsilon;
} QLearningState;

QLearningState *QLearningStateCreate();
double TestEpisode(QLearningState *q_state);
double TrainEpisode(QLearningState *q_state);
