#include <stdio.h>
#include <stdlib.h>
#include "qlearning.h"

int main(int argc, char** argv) {
    srand(2);
    FILE* result_file = fopen(argv[1], "wb");
    if (result_file == NULL) {
        printf("Error opening file!\n");
        exit(1);
    }
    QLearningState *q_state = QLearningStateCreate();
    q_state->alpha = atof(argv[2]);
    q_state->epsilon = atof(argv[3]);
    q_state->gamma = atof(argv[4]);
    for (int epoch = 0; epoch < 10; epoch++) {
        for (int i = 0; i < 100; i++) {
           TrainEpisode(q_state);
        }
        double total_reward = 0.0;
        for (int i = 0; i < 100; i++) {
            total_reward += TestEpisode(q_state);
        }
        printf("Average reward: %f\n", total_reward / 100.0);
    }
    fwrite(q_state, sizeof(QLearningState), 1, result_file);
    fclose(result_file);
    return 0;
}
