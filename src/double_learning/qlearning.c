#include <stdlib.h>
#include "../common/platformer.h"
#include "qlearning.h"

QLearningState *QLearningStateCreate() {
    QLearningState *state = malloc(sizeof(QLearningState));
    for (int i = 0; i < N_STATE_0 * N_STATE_1; i++) {
        for (int j = 0; j < N_ACTIONS; j++) {
            state->q_table[i][j] = 0.0;
        }
    }
    return state;
}

static int get_state_index(PlatformerState* state)
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

double TestEpisode(QLearningState *q_state) {
    PlatformerState state;
    PlatformerReset(&state);
    double total_reward = 0.0;
    while (!state.done) {
        // choose action
        PlatformerAction action = 0;
        int state_index = get_state_index(&state);
        double max_q = q_state->q_table[state_index][0];
        for (int i = 0; i < N_ACTIONS; i++) {
            if (q_state->q_table[state_index][i] > max_q) {
                max_q = q_state->q_table[state_index][i];
                action = i;
            }
        }
        double reward = PlatformerStep(&state, action);
        total_reward += reward;
    }
    return total_reward;
}

double TrainEpisode(QLearningState *q_state) {
    PlatformerState state;
    PlatformerReset(&state);
    double total_reward = 0.0;
    while (!state.done) {
        // choose action
        PlatformerAction action = 0;
        int state_index = get_state_index(&state);
        if (rand() / (double)RAND_MAX < q_state->epsilon) {
            // random action
            // ジャンプは低確率(1/8)
            action = (rand() & 7) ? 0 : 1;
        } else {
            double max_q = q_state->q_table[state_index][0];
            for (int i = 0; i < N_ACTIONS; i++) {
                if (q_state->q_table[state_index][i] > max_q) {
                    max_q = q_state->q_table[state_index][i];
                    action = i;
                }
            }
        }
        double reward = PlatformerStep(&state, action);
        total_reward += reward;

        // update model
        int next_state_index = get_state_index(&state);
        double max_q_next = q_state->q_table[next_state_index][0];
        for (int i = 0; i < N_ACTIONS; i++) {
            if (q_state->q_table[next_state_index][i] > max_q_next) {
                max_q_next = q_state->q_table[next_state_index][i];
            }
        }
        q_state->q_table[state_index][action] += q_state->alpha * (reward + q_state->gamma * max_q_next - q_state->q_table[state_index][action]);
    }

    return total_reward;
}
