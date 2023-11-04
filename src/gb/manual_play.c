#include <stdio.h>
#include <gb/gb.h>
#include <gbdk/console.h>
#include "../common/platformer.h"
#include "manual_play.h"
#include "ui.h"

#define DONE_WAIT 30 // エピソード終了後の待ちフレーム数

int16_t human_best_score = 0;

uint8_t manual_play_mode(void)
{
    clear_print_area();

    gotoxy(0, 0);
    printf("MANUAL PLAY MODE\nA: JUMP\nSTART: AI TRAINING");
    
    uint8_t done_wait = 0;
    PlatformerState state;
    PlatformerReset(&state);
    int16_t episode_reward = 0;

    while (1)
    {
        // 30fpsにする（60fpsだと速すぎる）
        wait_vbl_done();
        wait_vbl_done();
        // 表示
        // y座標は、move_spriteのオフセット16と、上端座標計算のstate.y-16が相殺
        move_sprite(SPRITE_RUNNER, (uint8_t)(state.x + 8), (uint8_t)(state.y + STAGE_TOP_OFFSET * TILE_SIZE));
        gotoxy(0, 4);
        printf("SCORE %d    \nBEST  %d", episode_reward, human_best_score);
        // 入力
        JoypadWithLast button;
        button = joypad_with_last();
        uint8_t new_button = button.current & ~button.last;
        if (new_button & J_START)
        {
            return UI_MODE_TRAIN;
        }
        if (done_wait)
        {
            done_wait--;
            if (!done_wait)
            {
                PlatformerReset(&state);
                episode_reward = 0;
                clear_line(4);
            }
        }
        else
        {
            PlatformerAction action = 0;
            if (new_button & J_A)
            {
                // Aを押した瞬間だけジャンプ
                action = 1;
            }
            // 状態更新
            episode_reward += PlatformerStep(&state, action) / REWARD_SCALE;
            if (state.done)
            {
                if (episode_reward > human_best_score)
                {
                    human_best_score = episode_reward;
                }
                done_wait = DONE_WAIT;
            }
        }
    }
}
