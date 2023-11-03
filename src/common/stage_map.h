// ステージ。

#define TILE_SIZE 8
#define STAGE_TOP_OFFSET 8 // 画面上端からステージ上端までの距離（タイル単位）
#define STAGE_WIDTH 19 // タイル単位(ゴール含まない)
#define STAGE_HEIGHT 10 // タイル単位
#define CHAR_HEIGHT 2 // キャラクターの高さ（タイル単位）
#define TILE_ID_EMPTY 0x80
#define TILE_ID_FLOOR 0x81
#define TILE_ID_GOAL 0x82

// 解く対象のステージ
extern unsigned char StageMap[STAGE_WIDTH * STAGE_HEIGHT];
// デフォルトのステージ
extern const unsigned char PresetStageMap[];
// ゴール
extern const unsigned char GoalMap[];
