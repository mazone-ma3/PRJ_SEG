/* COMMON MACRO for X68K with XSP*/

#ifndef SP_H_INCLUDE
#define SP_H_INCLUDE

#include "common.h"

#define ABS(X) ((X < 0) ? -X : X)

//#define NOERROR 0
#define ERROR -1
enum { NOERROR = 0, SYSERR, SYSEXIT, ERRLV1, ERRLV2, ERRLV3};

#define PCGPARTS (16 * 4)
#define PCG_SPACE (0x40 - '0')
#define SPRPARTS 128

#define SCREEN_MAX_X 256

#define SCREEN_MAX_Y 256

#ifdef XSP
#define CHR_TOP 0
#else
#define CHR_TOP (PCGPARTS / 4)
#endif

#define SCREEN_MIN_X 0
#define SCREEN_MIN_Y 0
#define WAIT1S 60

#ifdef XSP
#define MAX_SPRITE 512
#else
#define MAX_SPRITE 128
#endif

#define BACKCOLOR	0x0f00
/*#define SPBACKCOLOR	0x8000*/

//#define TEKI_NUM_MAX 30

#define STAR_NUM	64

#define SHIFT_NUM	3

#define SCRL_SFT 4
#define SCRL_MIN 16
#define SCRL_MAX (SCRL_MIN*SCRL_MIN) / 2

#ifndef MAIN
#define EXTERN extern
#else
#define EXTERN
#endif

/* EXTERN */

EXTERN unsigned short *vram;
EXTERN unsigned short *spram;
EXTERN unsigned short *spdata;

typedef struct{
	int x;
	int y;
} SPR;

/* 各キャラクタの構造体(CHR_PARA) */
typedef struct chr_para{
//	struct chr_para *next,*prev;
	short x,y, pat_num,atr;
	//,count,hp;
} CHR_PARA;

typedef struct chr_para2{
	int xx,yy;
} CHR_PARA2;


//EXTERN CHR_PARA start[10],fin[10];		/* リスト構造(未使用) */
EXTERN CHR_PARA chr_data[MAX_SPRITE * 2];
EXTERN CHR_PARA chr_data2[2][MAX_SPRITE];

EXTERN int spr_num;						/* スプライト最大表示数 */

EXTERN short scrl, scrl_spd;
EXTERN unsigned short star[5][STAR_NUM];


/**********************************************************************/

#define CONST const

enum {
	SPR_OFS_X = -16,
	SPR_OFS_Y = -16,
/*	SPR_DIV = 8, */

/*	MAX_SPRITE = 180,*/
};


/* 型 */
/*typedef struct{
	int x;
	int y;
} SPR;
*/
typedef struct{
	int x;
	int y;
	int xx;
	int yy;
	int patno;
	int count;
} CHR_OBJ;


typedef struct {
	int patno;
	int x;
	int y;
	int atr;
} SPR_COMB;

typedef struct {
	CONST int patmax;
	CONST SPR_COMB *data;
} SPR_INFO;

/* マクロ */
#define SYS_WAIT sys_wait

#define SPR_SET(NO, X, Y) {\
	spr[NO].x = X;\
	spr[NO].y = Y;\
}

EXTERN CHR_PARA *pchr_data;

#ifndef XSP
/* スプライト位置を定義するマクロ */
#define DEF_SP_SINGLE( NO, X, Y, PAT, PAL, ATR, PRO) {\
	pchr_data = &chr_data[NO];\
	pchr_data->x = (X >> SHIFT_NUM) + SPR_OFS_X + 16; \
	pchr_data->y = (Y >> SHIFT_NUM) + SPR_OFS_Y + 16; \
	pchr_data->pat_num = PAT + (PAL << 8) + (ATR << 14); \
	pchr_data->atr = 0x0002; \
	NO++; \
}
#else
#define DEF_SP_SINGLE( NO, X, Y, PAT, PAL, ATR, PRO) {\
	pchr_data = &chr_data[NO];\
	pchr_data->x = (X >> SHIFT_NUM) + SPR_OFS_X + 16; \
	pchr_data->y = (Y >> SHIFT_NUM) + SPR_OFS_Y + 16; \
	pchr_data->pat_num = PAT & 0x3fff; \
	pchr_data->atr = (PAL << 8) | (ATR << 14) | 0x20 | PRO | (PAT & 0xc000);\
	NO++; \
}
#endif

/*	chrpal = MK_CHRNO(0, pal_no, 0);\*/
/*		chrno++;\*/

/* マクロ版 複合スプライト処理 */
#define DEF_SP(chrno, X, Y, pat, pal_no, PRO) {\
\
	chrnum = spr_info[pat].patmax;\
	sprcomb = spr_info[pat].data;\
	tmp_xx = X;\
	tmp_yy = Y;\
	while(chrnum--){\
		DEF_SP_SINGLE(chrno, tmp_xx + (sprcomb->x << SHIFT_NUM),\
					 tmp_yy + (sprcomb->y << SHIFT_NUM),\
					sprcomb->patno,\
					pal_no,\
					sprcomb->atr, PRO);\
		sprcomb++;\
	}\
}

/*enum {
	PRO_JIKI = 0,
	PRO_JIKISHOT,
	PRO_TEKI,
	PRO_TEKISHOT
};
*/
enum {
	PRO_TEKISHOT = 0,
	PRO_TEKI,
	PRO_JIKISHOT,
	PRO_JIKI
};
#endif
