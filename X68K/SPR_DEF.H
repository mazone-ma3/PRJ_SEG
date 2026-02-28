/* 組み合わせスプライト定義  */

#ifndef SPR_DEF_H_INCLUDE
#define SPR_DEF_H_INCLUDE

#define CONST const

/* 複合スプライト定義補助マクロ */
#define IMG_SET(X, Y) (CHR_TOP + X + IMG_SIZE_X * Y)

#define SPR_FLIP_H 0x4000
#define SPR_FLIP_V 0x8000

/* 1×1 スプライト 定義簡略化マクロ */
#define MK_SPR1(VAR, NO) CONST SPR_COMB VAR[] = {\
	{ NO,  0,  0 ,0,},\
};

/* 左右反転 */
#define MK_SPR1_RL(VAR, NO) CONST SPR_COMB VAR[] = {\
	{ IMG_SET(NO    , 0) | SPR_FLIP_H,  0,  8, 0,},\
};

/* 上下反転 */
#define MK_SPR1_UD(VAR, NO) CONST SPR_COMB VAR[] = {\
	{ IMG_SET(NO    , 0) | SPR_FLIP_V,  8,  0, 0,},\
};

/* 鏡像 */
#define MK_SPR1_RV(VAR, NO) CONST SPR_COMB VAR[] = {\
	{ IMG_SET(NO    , 0) | SPR_FLIP_H | SPR_FLIP_V,  8,  8, 0,},\
};



/* 自機定義 */

CONST SPR_COMB jiki1_comb[JIKI_PARTS] = {
	{  IMG_SET(0, 0),  0,  0,0,},
};

CONST SPR_COMB jiki2_comb[JIKI_PARTS] = {
	{  IMG_SET(1, 0),  0,  0,0,},
};

CONST SPR_COMB jiki3_comb[JIKI_PARTS] = {
	{  IMG_SET(2, 0),  0,  0,0,},
};

CONST SPR_COMB jiki4_comb[JIKI_PARTS] = {
	{  IMG_SET(3, 0),  0,  0,0,},
};

/* 自機ショット定義 */

CONST SPR_COMB myshot1_comb[] = {
	{  IMG_SET(4, 1),  0,  4,0,},
/*	{  IMG_SET(3, 1), 13,  8,0,} */
};

CONST SPR_COMB myshot2_comb[] = {
	{  IMG_SET(4, 1) | SPR_FLIP_H,  0,  4,0,},
//	{  IMG_SET(3, 0),  1,  8,0,},
};

CONST SPR_COMB myshot3_comb[] = {
	{  IMG_SET(3, 0)  | SPR_FLIP_V,  1,  8,0,},
};

/* 敵ショット定義 */

CONST SPR_COMB tkshot1_comb[] = {
//	{  IMG_SET(4, 1), 0, 0,0,},
	{  IMG_SET(1, 2), -4, -4,0,},
};

CONST SPR_COMB tkshot2_comb[] = {
//	{  IMG_SET(4, 1), 0, 0,0,},
	{  IMG_SET(0, 2), 0, 0,0,},
};

CONST SPR_COMB tkshot3_comb[] = {
//	{  IMG_SET(4, 1) | SPR_FLIP_H, 0, 0,0,},
	{  IMG_SET(1, 2) | SPR_FLIP_H, -4, -4,0,},
};

/*
CONST SPR_COMB tkshot2_comb[] = {
	{  IMG_SET(2, 6), -4, 0,0,},
};*/

/* 雑魚敵定義 */

MK_SPR1(teki1_0comb, IMG_SET(0, 1));
MK_SPR1(teki1_1comb, IMG_SET(0, 1));

MK_SPR1(teki3_comb, IMG_SET(1, 1));
MK_SPR1(teki4_comb, IMG_SET(2, 1));


MK_SPR1(teki5_0comb, IMG_SET(0, 5));
MK_SPR1(teki5_1comb, IMG_SET(1, 5));
MK_SPR1(teki5_2comb, IMG_SET(2, 5));
MK_SPR1(teki5_3comb, IMG_SET(3, 5));
MK_SPR1(teki5_4comb, IMG_SET(4, 5));

MK_SPR1_RL(teki5_5comb, IMG_SET(4, 5));
MK_SPR1_RL(teki5_6comb, IMG_SET(3, 5));
MK_SPR1_RL(teki5_7comb, IMG_SET(2, 5));
MK_SPR1_RL(teki5_8comb, IMG_SET(1, 5));

MK_SPR1_RV(teki5_9comb, IMG_SET(1, 5));
MK_SPR1_RV(teki5_acomb, IMG_SET(2, 5));
MK_SPR1_RV(teki5_bcomb, IMG_SET(3, 5));
MK_SPR1_RV(teki5_ccomb, IMG_SET(4, 5));

MK_SPR1_UD(teki5_dcomb, IMG_SET(3, 5));
MK_SPR1_UD(teki5_ecomb, IMG_SET(2, 5));
MK_SPR1_UD(teki5_fcomb, IMG_SET(1, 5));

/*MK_SPR1_RL(teki5_acomb, IMG_SET(4, 0)); */


MK_SPR1(teki6_0comb, IMG_SET(0, 3));
MK_SPR1(teki6_1comb, IMG_SET(1, 3));
MK_SPR1(teki6_2comb, IMG_SET(2, 3));

MK_SPR1(baku1_comb, IMG_SET(0, 0));
MK_SPR1(baku2_comb, IMG_SET(1, 0));
MK_SPR1(baku3_comb, IMG_SET(2, 0));


/*#define WIDE_PARTS(X, Y, Z) \
	{  IMG_SET(X + Y, 0), -28, Z * 8, 0,},\
	{  IMG_SET(X + Y, 4),  4, Z * 8, 0,}*/
/*,\*/

#define ZAKO_PARTS 1
#define BOSS1_PARTS 4*2

CONST SPR_COMB boss1_comb[BOSS1_PARTS] = {
	{  IMG_SET(0, 1),  0 - 24,  0,0,},
	{  IMG_SET(1, 1), 16 - 24,  0,0,},
	{  IMG_SET(2, 1), 32 - 24,  0,0,},
	{  IMG_SET(3, 1), 48 - 24,  0,0,},
	{  IMG_SET(0, 2),  0 - 24, 16,0,},
	{  IMG_SET(1, 2), 16 - 24, 16,0,},
	{  IMG_SET(2, 2), 32 - 24, 16,0,},
	{  IMG_SET(3, 2), 48 - 24, 16,0,},

/*	WIDE_PARTS(0, 1, -1),
	WIDE_PARTS(1, 1, 0),
	WIDE_PARTS(2, 1, 1),
	WIDE_PARTS(3, 1, 2),*/
};

/*CONST SPR_COMB teki10_comb[] = {
	{  8,  0,  0,},
	{  9,  0,  8,},
	{ 24,  8,  0,},
	{ 25,  8,  8,},
};*/


/* 複合スプライト最終定義テーブル */
/* 固さ・アルゴリズムNo. など */
CONST SPR_INFO spr_info[] = {
	{ JIKI_PARTS,    jiki1_comb,},
	{ JIKI_PARTS,    jiki2_comb,},
	{ JIKI_PARTS,    jiki3_comb,},
	{ JIKI_PARTS,    jiki4_comb,},

	{          1,  myshot1_comb,},
//	{          2,  myshot1_comb,},
	{          1,  myshot2_comb,},
	{          1,  myshot3_comb,},

	{ ZAKO_PARTS,    teki1_0comb,},
	{ ZAKO_PARTS,    teki1_1comb,},
	{ ZAKO_PARTS,    teki3_comb,},
	{ ZAKO_PARTS,    teki4_comb,},

	{ ZAKO_PARTS,    teki5_0comb,},
	{ ZAKO_PARTS,    teki5_1comb,},
	{ ZAKO_PARTS,    teki5_2comb,},
	{ ZAKO_PARTS,    teki5_3comb,},
	{ ZAKO_PARTS,    teki5_4comb,},
	{ ZAKO_PARTS,    teki5_5comb,},
	{ ZAKO_PARTS,    teki5_6comb,},
	{ ZAKO_PARTS,    teki5_7comb,},
	{ ZAKO_PARTS,    teki5_8comb,},
	{ ZAKO_PARTS,    teki5_9comb,},
	{ ZAKO_PARTS,    teki5_acomb,},
	{ ZAKO_PARTS,    teki5_bcomb,},
	{ ZAKO_PARTS,    teki5_ccomb,},
	{ ZAKO_PARTS,    teki5_dcomb,},
	{ ZAKO_PARTS,    teki5_ecomb,},
	{ ZAKO_PARTS,    teki5_fcomb,},

	{ ZAKO_PARTS,    teki6_0comb,},
	{ ZAKO_PARTS,    teki6_1comb,},
	{ ZAKO_PARTS,    teki6_2comb,},

	{          1,  tkshot1_comb,},
	{          1,  tkshot2_comb,},
	{          1,  tkshot3_comb,},

	{ ZAKO_PARTS,    baku1_comb,},
	{ ZAKO_PARTS,    baku2_comb,},
	{ ZAKO_PARTS,    baku3_comb,},

	{ ZAKO_PARTS,    teki1_0comb,},
//	{ BOSS1_PARTS,    boss1_comb,},
};

/* 複合スプライトNo.参照用定数 */
enum {
	PAT_JIKI_C,
	PAT_JIKI_R,
	PAT_JIKI_L,
	PAT_JIKI_J,

	PAT_MYSHOT1,
	PAT_MYSHOT2,
	PAT_MYSHOT3,

	PAT_TEKI1,
	PAT_TEKI2,
	PAT_TEKI3,
	PAT_TEKI4,

	PAT_TEKI5_0,
	PAT_TEKI5_1,
	PAT_TEKI5_2,
	PAT_TEKI5_3,
	PAT_TEKI5_4,
	PAT_TEKI5_5,
	PAT_TEKI5_6,
	PAT_TEKI5_7,
	PAT_TEKI5_8,
	PAT_TEKI5_9,
	PAT_TEKI5_A,
	PAT_TEKI5_B,
	PAT_TEKI5_C,
	PAT_TEKI5_D,
	PAT_TEKI5_E,
	PAT_TEKI5_F,

	PAT_TEKI6_0,
	PAT_TEKI6_1,
	PAT_TEKI6_2,

	PAT_TKSHOT1,
	PAT_TKSHOT2,
	PAT_TKSHOT3,

	PAT_BAKU1,
	PAT_BAKU2,
	PAT_BAKU3,

	PAT_BOSS1
};

int teki5_pat[32] = {
	PAT_TEKI5_7,
	PAT_TEKI5_6,
	PAT_TEKI5_5,
	PAT_TEKI5_4,

	PAT_TEKI5_3,
	PAT_TEKI5_2,
	PAT_TEKI5_1,
	PAT_TEKI5_0,
	PAT_TEKI5_F,
	PAT_TEKI5_E,
	PAT_TEKI5_D,
	PAT_TEKI5_C,

	PAT_TEKI5_B,
	PAT_TEKI5_A,
	PAT_TEKI5_9,
	PAT_TEKI5_8,

};

#define PAT_TEKIHEAD PAT_TEKI1

#endif
