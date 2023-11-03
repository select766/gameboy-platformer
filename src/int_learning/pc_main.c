// PC上で学習を試みる。
#include <stdio.h>
#include <stdlib.h>
#include "config.h"
#include "qlearning.h"

int main() {
    QLearningState *q_state = QLearningStateCreate();
    for (int epoch = 0; epoch < 100; epoch++) {
        srand((unsigned int)epoch);
        for (int i = 0; i < 10; i++) {
            TrainEpisode(q_state);
        }
        // print_table(q_state);
        // fix test case
        srand((unsigned int)1);
        EpisodeResult episode_result;
        TestEpisode(q_state, &episode_result);
        printf("%d,missed:%d,score:%d,steps:%d\n", epoch, episode_result.isMissed, episode_result.score, episode_result.steps);
    }

    FILE* result_file = fopen("trained.bin", "wb");
    fwrite(q_state, sizeof(QLearningState), 1, result_file);
    fclose(result_file);
    return 0;
}
