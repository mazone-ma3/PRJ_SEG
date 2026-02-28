/* Game Data (Common) */
#ifndef SP_SHT_H_INCLUDE
#define SP_SHT_H_INCLUDE

/*#include "sp_loop.h"*/

enum {
	IMG_SIZE_X = 16,

	/* スプライト表示範囲 */
	SPR_MAX_X = ((SCREEN_MAX_X - SPR_OFS_X) << SHIFT_NUM),
	SPR_MAX_Y = ((SCREEN_MAX_Y - SPR_OFS_Y) << SHIFT_NUM),

	SPR_MIN_X = (-SPR_OFS_X << SHIFT_NUM),
	SPR_MIN_Y = (-SPR_OFS_Y << SHIFT_NUM),

	SPR_OUT_X = 256,
	SPR_OUT_Y = 256,

	SPR_IN_X = -16,
	SPR_IN_Y = -16,

	SPR_DEL_Y = ((SPR_OUT_Y - SPR_OFS_Y) << SHIFT_NUM),

	/* 自機スプライト情報 */
	JIKI_PARTS_X = 1, //2,	/* 組み合わせパーツ数 */
	JIKI_PARTS_Y = 1,
	JIKI_PARTS = JIKI_PARTS_X * JIKI_PARTS_Y,

	JIKI_INIT_X = ((2 * 8     - SPR_OFS_X) << SHIFT_NUM),	/* 初期表示位置 */
	JIKI_INIT_Y = ((7 * 8 + 4 - SPR_OFS_Y) << SHIFT_NUM),

	JIKI_MAX_X = (SPR_MAX_X - ((16) << SHIFT_NUM)),	/* 自機移動範囲 */
	JIKI_MAX_Y = ((18 * 8) << SHIFT_NUM), //(SPR_MAX_Y - (16 << SHIFT_NUM)),

	JIKI_MIN_X = (SPR_MIN_X), // + 8),
	JIKI_MIN_Y = ((16 * 2) << SHIFT_NUM), //(SPR_MIN_Y),

	/* 当たり判定情報 オフセットとサイズ */
	MY_OFS_X = 9,
	MY_OFS_Y = 9,

	MY_SIZE_X = 1,
	MY_SIZE_Y = 1,


	TK_OFS_X = 0,
	TK_OFS_Y = 0,

	TK_SIZE_X = 16,
	TK_SIZE_Y = 16,


	TKD_OFS_X = 2,
	TKD_OFS_Y = 2,

	TKD_SIZE_X = 4,
	TKD_SIZE_Y = 4,

	/* 表示制限数 */
	MAX_MYSHOT = 16,
	MAX_TEKI = 14 * 2,

	/* 表示先頭プレーンNo. 固定 */
	MYSHOT_OFFSET = JIKI_PARTS,
	TEKI_OFFSET = MYSHOT_OFFSET + MAX_MYSHOT,
	TKSHOT_OFFSET = TEKI_OFFSET + MAX_TEKI, // * 2,

	/* 敵弾表示制限数 */
	MAX_TKSHOT = MAX_SPRITE - TKSHOT_OFFSET

};

/* 当たり判定算出マクロ */

#define CLGN(head_x, head_y, size_x, size_y) {\
	((head_x - MY_OFS_X + size_x   ) << SHIFT_NUM),\
	((head_x - MY_OFS_X - MY_SIZE_X) << SHIFT_NUM),\
	((head_y - MY_OFS_Y + size_y   ) << SHIFT_NUM),\
	((head_y - MY_OFS_Y - MY_SIZE_Y) << SHIFT_NUM),\
}

typedef struct {
	int x1;
	int x2;
	int y1;
	int y2;
} COLIGION;

COLIGION hantei[2] = {
	CLGN(TK_OFS_X, TK_OFS_Y, TK_SIZE_X, TK_SIZE_Y),
	CLGN(TK_OFS_X, TK_OFS_Y, TK_SIZE_X, TK_SIZE_Y),
};


#include "spr_def.h"

enum{
	SINFREQ = 19 * 2,
	SINRATE = 4
};

int sinwave[SINFREQ] = {
	0,  1,  2,  3,  3,  4,  4,  4,  5,  5,  5,  5,  4,  4,  4,  3,  3,  2,  1,
	0, -1, -2, -3, -3, -4, -4, -4, -5, -5, -5, -5, -4, -4, -4, -3, -3, -2, -1,
};

int direction[33][2] = {
	{ 12,  0},	/* 右 = 0 */
	{ 11,  2},
	{ 10,  4},
	{  9,  6},
	{  8,  8},
	{  6,  9},
	{  4, 10},
	{  2, 11},

	{  0, 12},	/* 下 = 8 */
	{ -2, 11},
	{ -4, 10},
	{ -6,  9},
	{ -8,  8},
	{ -9,  6},
	{-10,  4},
	{-11,  2},

	{-12, -0},	/* 左 = 16 */
	{-11, -2},
	{-10, -4},
	{ -9, -6},
	{ -8, -8},
	{ -6, -9},
	{ -4,-10},
	{ -2,-11},

	{  0,-12},	/* 上 = 24 */
	{  2,-11},
	{  4,-10},
	{  6, -9},
	{  8, -8},
	{  9, -6},
	{ 10, -4},
	{ 11, -2},

	{ 0, 0},
};

enum {
	DIR_RIGHT = 0,	/* 縦では上 */
	DIR_DOWN  = 8,	/* 縦では右 */
	DIR_LEFT = 16,	/* 縦では下 */
	DIR_UP   = 24,	/* 縦では左 */
	DIR_OFF = 32	/* 縦では無 */
};

#endif

