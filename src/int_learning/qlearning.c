#include <stdlib.h>
#include "../common/platformer.h"
#include "qlearning.h"
#include "config.h"

QLearningState *QLearningStateCreate() {
    QLearningState *state = malloc(sizeof(QLearningState));
    for (int i = 0; i < N_STATE_0 * N_STATE_1; i++) {
        for (int j = 0; j < N_ACTIONS; j++) {
            state->q_table[i][j] = 0;
        }
    }
    return state;
}

static int get_state_index(const PlatformerState* state)
{
    int position = (int)(state->x);
    int velocity;
    if (state->isOnFloor)
    {
        velocity = 0;
    }
    else if (state->ySpeed > 0)
    {
        velocity = 1;
    }
    else
    {
        velocity = 2;
    }
    return position * N_STATE_1 + velocity;
}

PlatformerAction GetBestAction(const QLearningState *q_state, const PlatformerState* state) {
    PlatformerAction action = 0;
    int state_index = get_state_index(state);
    Reward max_q = q_state->q_table[state_index][0];
    for (uint8_t i = 1; i < N_ACTIONS; i++) {
        if (q_state->q_table[state_index][i] > max_q) {
            max_q = q_state->q_table[state_index][i];
            action = i;
        }
    }
    return action;
}

void TestEpisode(const QLearningState *q_state, EpisodeResult *episode_result) {
    PlatformerState state;
    PlatformerReset(&state);
    Reward total_reward = 0;
    while (!state.done) {
        // choose action
        PlatformerAction action = GetBestAction(q_state, &state);
        Reward reward = PlatformerStep(&state, action);
        total_reward += reward;
    }
    episode_result->totalReward = total_reward;
    episode_result->isMissed = state.isMissed;
    episode_result->steps = state.steps;
    episode_result->score = state.score;
}

Reward TrainEpisode(QLearningState *q_state) {
    PlatformerState state;
    PlatformerReset(&state);
    Reward total_reward = 0;
    while (!state.done) {
        // choose action
        PlatformerAction action = 0;
        int state_index = get_state_index(&state);
        if (rand() < EPSILON_RAND_MAX) {
            // random action
            action = (rand() & 7) ? 0 : 1;
        } else {
            Reward max_q = q_state->q_table[state_index][0];
            for (uint8_t i = 1; i < N_ACTIONS; i++) {
                if (q_state->q_table[state_index][i] > max_q) {
                    max_q = q_state->q_table[state_index][i];
                    action = i;
                }
            }
        }
        Reward reward = PlatformerStep(&state, action);
        total_reward += reward;

        // update model
        Reward max_q_next = 0;
        if (!state.done) {
            int next_state_index = get_state_index(&state);
            max_q_next = q_state->q_table[next_state_index][0];
            for (uint8_t i = 1; i < N_ACTIONS; i++) {
                if (q_state->q_table[next_state_index][i] > max_q_next) {
                    max_q_next = q_state->q_table[next_state_index][i];
                }
            }
        }

        Reward q_value = q_state->q_table[state_index][action];
        q_state->q_table[state_index][action] = ((reward + max_q_next - (max_q_next >> GAMMA_BITSHIFT) - q_value) >> ALPHA_BITSHIFT) + q_value;
        // for Reward is double
        // q_state->q_table[state_index][action] = (reward + max_q_next * 0.984375 - q_value) * 0.015625 + q_value;
    }

    return total_reward;
}
