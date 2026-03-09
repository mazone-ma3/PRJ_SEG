/* 共通マクロ for FM TOWNS */

#ifndef SP_H_INCLUDE
#define SP_H_INCLUDE

#define TOWNS

#define ABS(X) ((X < 0) ? -X : X)

//#define NOERROR		0			/* お決まりのもの */
#define ERROR		-1
enum { NOERROR = 0, SYSERR, SYSEXIT, ERRLV1, ERRLV2, ERRLV3};

#define FONTPARTS (16 * 4)
#define TITLEPARTS 0 //(8 * 4)
#define SPRPARTS 128

#define SCREEN_MAX_X 256

#define SCREEN_MAX_Y 240
#define CHR_TOP 128

#define SCREEN_MIN_X 0
#define SCREEN_MIN_Y 0
#define WAIT1S 60

#define MAX_SPRITE 200			/* 最大許容スプライト表示数 */

#define BACKCOLOR	0x0f		/* 背景を塗りつぶす色 */

#define SPBACKCOLOR	0x8000	/* スプライトの未表示部分を塗りつぶす色 */

//#define TEKI_NUM_MAX 30

#define STAR_NUM	64						/* スタ－の数 */
/* この値は386-TOWNS(WAIT外した時)用に合わせてある */
/* 486機ならもっといく */

#define SHIFT_NUM	3		/* 座標系をシフトする回数(固定小数点演算用) */
#define SHIFT_NUM_Y	3

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
//EXTERN _Far short *vram;		/* VRAM操作用変数 */
//EXTERN _Far short *spram;		/* スプライトRAM操作用変数 */
EXTERN int vram;		/* VRAM操作用変数 */
EXTERN int spram;		/* スプライトRAM操作用変数 */

typedef struct{
	int x;
	int y;
} SPR;

/* 各キャラクタの構造体(CHR_PARA) */
typedef struct chr_para{
//	struct chr_para *next,*prev;
	int x,y,xx,yy,pat_num,atr,count,hp;
} CHR_PARA;

typedef struct chr_para2{
	short x , y, pat_num, atr;
} CHR_PARA2;

//EXTERN CHR_PARA start[10],fin[10];		/* リスト構造(未使用) */
EXTERN CHR_PARA2 chr_data[MAX_SPRITE * 2];

EXTERN int spr_num;						/* スプライト最大表示数 */

EXTERN short scrl, scrl_spd;				/* スクロ－ル管理用 */
EXTERN short int star[5][STAR_NUM];		/* スタ－管理用 */


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

EXTERN CHR_PARA2 *pchr_data;

/* スプライト位置を定義するマクロ */
#define DEF_SP_SINGLE( NO, X, Y, PAT, PAL, ATR) {\
	pchr_data = &chr_data[NO];\
	pchr_data->x = (X >> SHIFT_NUM) + SPR_OFS_X; \
	pchr_data->y = (Y >> SHIFT_NUM) + SPR_OFS_Y + 2; \
	pchr_data->pat_num = PAT | (ATR << 10); \
	pchr_data->atr = (PAL + 256) | 0x8000; \
	NO++; \
}

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
					sprcomb->atr);\
		sprcomb++;\
	}\
}

#endif
