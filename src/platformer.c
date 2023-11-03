#include "platformer.h"

void initGameState(GameState *state)
{
    // mapDataは初期化されない
    state->x = 0;
    state->y = TILE_SIZE * (STAGE_HEIGHT - 1); // 一番下のタイルの上端
    state->ySpeed = 0;
    state->done = 0;
    state->steps = 0;
    state->score = 0;
    state->isOnFloor = 0;
    state->isMissed = 0;
}

void stepGameState(GameState *state, GameAction action)
{
    if (state->done)
    {
        // ゴール済みならもう動かない
        return;
    }
    // 地面の高さを判定
    // x座標 [x, x + TILE_SIZE) にあるタイルを調べる
    // この方法では、浮いているブロックは扱えない
    uint8_t floorY = STAGE_HEIGHT * TILE_SIZE;
    {
        uint8_t tileXLow = state->x / TILE_SIZE;
        uint8_t tileXHigh = (state->x + TILE_SIZE - 1) / TILE_SIZE;
        for (uint8_t tileX = tileXLow; tileX <= tileXHigh; tileX++)
        {
            for (uint8_t tileY = 0; tileY < STAGE_HEIGHT; tileY++)
            {
                uint8_t tileId = state->mapData[tileY * STAGE_WIDTH + tileX];
                if (tileId != TILE_ID_EMPTY)
                {
                    uint8_t tileYTop = tileY * TILE_SIZE;
                    if (tileYTop < floorY)
                    {
                        floorY = tileYTop;
                    }
                    break;
                }
            }
        }
    }

    if (state->y >= floorY)
    {
        // 床に接している
        state->y = floorY;
        if (action)
        {
            state->ySpeed = -8;
        }
        else if (state->ySpeed > 0)
        {
            state->ySpeed = 0;
        }
    }
    else
    {
        // 浮いている
        state->ySpeed += 1;
    }
    state->y += state->ySpeed;

    // めりこみ補正(不完全)
    if (state->y >= floorY)
    {
        // 床に接している
        state->y = floorY;
        state->ySpeed = 0;
        state->isOnFloor = 1;
    }
    else
    {
        state->isOnFloor = 0;
    }

    // キャラクターの右辺が接しているものを判定
    uint8_t charRight = state->x + TILE_SIZE;
    // x座標 charRightかつ、y座標 [y, y + CHAR_HEIGHT * TILE_SIZE) にあるタイルを調べる
    uint8_t xCollision = 0;
    uint8_t goal = 0;
    {
        uint8_t tileX = charRight / TILE_SIZE;
        uint8_t tileYLow = (state->y - CHAR_HEIGHT * TILE_SIZE) / TILE_SIZE;
        uint8_t tileYHigh = (state->y - 1) / TILE_SIZE;
        for (uint8_t tileY = tileYLow; tileY <= tileYHigh; tileY++)
        {
            uint8_t tileId = state->mapData[tileY * STAGE_WIDTH + tileX];
            if (tileId == TILE_ID_EMPTY)
            {
                continue;
            }
            else if (tileId == TILE_ID_FLOOR)
            {
                xCollision = 1;
            }
            else
            {
                // ゴール
                goal = 1;
            }
        }
    }

    if (state->y >= STAGE_HEIGHT * TILE_SIZE)
    {
        // 穴に落ちた
        state->done = 1;
        state->isMissed = 1;
    }
    else if (goal)
    {
        state->done = 1;
        // ゴールした時の高さに応じたスコア。ステージ下端からキャラクターの下端までの距離がスコアになる。最小で1ブロック分。
        state->score = STAGE_HEIGHT * TILE_SIZE - state->y;
    }
    else
    {
        if (!xCollision)
        {
            state->x += 1;
        }
    }

    state->steps++;
}
