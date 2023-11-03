#pragma once
#include <stdint.h>
#include <stdlib.h>
#ifdef __SDCC
#include <rand.h>
#endif
#define EPSILON 0.151 // 0.151
#define EPSILON_RAND_MAX (int)(EPSILON * RAND_MAX)
#define ALPHA_BITSHIFT 2 // 0.25 ~= 0.208; x * alphaの代わりに x >> ALPHA_BITSHIFTを使う
#define GAMMA_BITSHIFT 4 // 1-0.0625=0.9375 ~= 0.934; x * gammaの代わりに x - (x >> GAMMA_BITSHIFT)を使う
