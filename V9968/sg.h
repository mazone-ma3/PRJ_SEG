/* 共通マクロ for MSX2 */

#ifndef SG_H_INCLUDE
#define SG_H_INCLUDE

#define MSX2PP

#define ABS(X) ((X < 0) ? -X : X)

//#define NOERROR		0			/* お決まりのもの *
#define ERROR		-1
enum { NOERROR = 0, SYSERR, SYSEXIT, ERRLV1, ERRLV2, ERRLV3};

#define SCREEN_MAX_X 256

#define SCREEN_MAX_Y 212
#define CHR_TOP 0

#define SCREEN_MIN_X 0
#define SCREEN_MIN_Y 0
#define WAIT1S 60

#define MAX_SPRITE 64			/* 最大許容スプライト表示数 */

#define BACKCOLOR	0x00		/* 背景を塗りつぶす色 */

#define SPBACKCOLOR	0x00	/* スプライトの未表示部分を塗りつぶす色 */

//#define TEKI_NUM_MAX 8

#define STAR_NUM	16						/* スタ－の数 */

#define SHIFT_NUM_Y	3
#define SHIFT_NUM	3		/* 座標系をシフトする回数(固定小数点演算用) */
//#define SHIFT_NUM	1

#define SCRL_SFT 4			/* スクロ－ルのテスト用 */
#define SCRL_MIN 16
#define SCRL_MAX (SCRL_MIN*SCRL_MIN) / 2

/* 関数をメイン内でのみextern扱いにするマクロ */
#ifndef MAIN
#define EXTERN extern
#else
#define EXTERN
#endif

/* 共通変数 */

/*
typedef struct chr_para3{
	unsigned char y, x;
//	short x;
	unsigned char pat_num,atr;
//	unsigned char pal;
} CHR_PARA3;
*/
typedef struct chr_para3{
	unsigned char yl;	// Y -512～+511
	unsigned char yh;	// Y -512～+511
	unsigned char mgy;	// 垂直サイズ (0=256)
	unsigned char ps;	// 透明度(TP)、反転フラグ(RVY/RVX、パレット(PS3-0))
	unsigned char xl;	// X -512～+511
	unsigned char xh;	// X -512～+511
	unsigned char mgx;	// 水平サイズ (0=256)
	unsigned char pat_num;	// Pattern Set パーツ番号(PY/PX) 16dot毎
} CHR_PARA3;

typedef struct chr_para4{
	unsigned char pat_num,atr;
//	unsigned char pal;
} CHR_PARA4;

EXTERN CHR_PARA3 chr_data[MAX_SPRITE * 2];
//EXTERN CHR_PARA3 chr_data2[MAX_SPRITE];
EXTERN CHR_PARA4 old_data[2][MAX_SPRITE];

//EXTERN unsigned char spr_num[2];						/* スプライト最大表示数 */

EXTERN short scrl, scrl_spd;				/* スクロ－ル管理用 */
//EXTERN unsigned char star[5][STAR_NUM];		/* スタ－管理用 */


/**********************************************************************/

#define CONST
// const

enum {
	SPR_OFS_X = -16, //16,
	SPR_OFS_Y = -16, //-16,
};


/* 型 */
typedef struct {
	unsigned char patno;
	signed char x;
	signed char y;
	unsigned char atr;
} SPR_COMB;

typedef struct {
	CONST unsigned char patmax;
	CONST SPR_COMB *data;
} SPR_INFO;

/* マクロ */
//#define SYS_WAIT sys_wait

#define SPR_SET(NO, X, Y) {\
	spr[NO].x = X;\
	spr[NO].y = Y;\
}

EXTERN CHR_PARA3 *pchr_data;

/* スプライト位置を定義するマクロ */

/*
#define DEF_SP_SINGLE_MACRO(X, Y, PAT, PAL) {\
	pchr_data = &chr_data[tmp_spr_count++];\
	pchr_data->pat_num = PAT; \
		pchr_data->atr = 0x00 | PAL;\
}*/
/*
#define DEF_SP_SINGLE_MACRO(X, Y, PAT, PAL) {\
	chr_data[tmp_spr_count].x = ((X >> SHIFT_NUM) + SPR_OFS_X); \
	chr_data[tmp_spr_count].y = ((Y >> SHIFT_NUM_Y) + SPR_OFS_Y - 1); \
	chr_data[tmp_spr_count].pat_num = PAT; \
		chr_data[tmp_spr_count++].atr = 0x00 | PAL;\
}
*/
/*
#define DEF_SP_SINGLE_MACRO(X, Y, PAT, PAL) {\
	pchr_data = &chr_data[tmp_spr_count++];\
	pchr_data->x = (((X) >> SHIFT_NUM) + SPR_OFS_X); \
	pchr_data->y = (((Y) >> SHIFT_NUM_Y) + SPR_OFS_Y - 1); \
	pchr_data->pat_num = (PAT); \
	if((pchr_data->x >= 256) || (pchr_data->x <= -16)){\
		pchr_data->y = 212;\
	}\
	else if(pchr_data->x >= 0){\
		pchr_data->atr = 0x00 | PAL;\
	}\
	else {\
		pchr_data->x += 32;\
		pchr_data->atr = 0x80 | PAL;\
	}\
}*/

short tx,ty;

// V9968 SPMode3
#define DEF_SP_SINGLE_MACRO(NO, X, Y, PAT, PAL) {\
	pchr_data = &chr_data[tmp_spr_count++];\
	tx = (((X) >> SHIFT_NUM) + SPR_OFS_X); \
	pchr_data->xl = tx & 0xff; \
	pchr_data->xh = (tx >> 8) & 0x03; \
	ty = (((Y) >> SHIFT_NUM_Y) + SPR_OFS_Y - 1); \
	pchr_data->yl = ty & 0xff; \
	pchr_data->yh = (ty >> 8) & 0x03; \
	pchr_data->pat_num = (PAT); \
	pchr_data->mgx = 16; \
	pchr_data->mgy = 16; \
	pchr_data->ps = PAL; \
	NO++;\
}

/* マクロ版 複合スプライト処理 */

#define DEF_SP(chrno, X, Y, pat, pal_no, PRO) {\
	chrnum = spr_info[pat].patmax;\
	sprcomb = spr_info[pat].data;\
	tmp_xxx = X;\
	tmp_yyy = Y;\
	while(chrnum--){\
		DEF_SP_SINGLE_MACRO(chrno, tmp_xxx + (sprcomb->x << SHIFT_NUM),\
					 tmp_yyy + (sprcomb->y << SHIFT_NUM_Y),\
					sprcomb->patno, pal_no\
					);\
		sprcomb++;\
	}\
}

/*#define DEF_SP_DOUBLE(X, Y, PAT, PAL) {\
	DEF_SP_SINGLE(X, Y, PAT * 8, PAL);\
	DEF_SP_SINGLE(X, Y, PAT * 8 + 4, PAL);\
}*/
//	printf("<%d %x>" , pat, spr_info[pat].adr);\

#endif
