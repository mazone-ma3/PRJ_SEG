/* SG_COM.H ゲーム共通部分 */

#include <stdio.h>
#include <stdlib.h>

#include "inkey.h"

//#include <time.h>

#include "sg.h"
//#include "subfunc.h"
#include "sg_main.h"


#include "list.h"

/*#include "sp_ene.h"*/
#include "sg_sht.h"
#include "sg_ene.h"

/* プロトタイプ指定 */

void game_init(void);		/* ゲームの初期化 */
short game_loop(void);		/* ゲーム本体 */

void wait_vsync(void);
void set_sprite(void);

#define FALSE 0
#define TRUE 1

#define CHRPAL_NO 0
#define REVPAL_NO 1

#define INIT_HSC 5000

unsigned char scrlspd = 0;
long score = 0, hiscore = INIT_HSC;

#define SCREEN2 0

unsigned char hit_check = TRUE;//FALSE;

typedef void (*SUBFUNC)(void);//(unsigned char i); 


#define DMGTIME 3
#define SHOTCOUNT 32
#define URAMAX 4

#define SCORE_MAX 99999999L
#define STAGEMAX 99


/*
1周目 普通
2周目 打ち返し
3周目 通常弾高速
4周目 打ち返し3方向
5周目以降 インターバルなし
*/

#define HP_MAX 10
#define SELENMAX 75

int command_num;
int waitcount = 0;
int schedule_ptr;
int *command;

unsigned char noshotdmg_flag;
unsigned char uramode;

unsigned char my_hp_flag = TRUE;

unsigned char seflag;


/* 自機情報 */
/* 自機デ－タ */
short my_data_x[2], my_data_y[2];
char my_data_xx[2], my_data_yy[2];

unsigned char my_data_pat_num[2], my_data_atr[2]; //
unsigned char my_data_pal[2]; //

unsigned char my_data_count[2], my_data_hp[2];

/* 自機弾情報 */
/* 自機弾座標管理 */
short myshot_x[MAX_MYSHOT];
short myshot_y[MAX_MYSHOT];
int myshot_xx[MAX_MYSHOT];
int myshot_yy[MAX_MYSHOT];

unsigned char myshot_pat_num[MAX_MYSHOT]; //

unsigned char myshot_next[MAX_MYSHOT + 2];
unsigned char myshot_free[MAX_MYSHOT + 2];

unsigned char mypal; //

unsigned char mypal_dmgtime;
unsigned char my_movecount;
unsigned char my_hp;
unsigned char max_my_hp;

/* 敵情報 */
/* 敵座標管理 */
short teki_x[MAX_TEKI];
short teki_y[MAX_TEKI];

unsigned int tkcount[MAX_TEKI];		/* 敵移動用カウンタ */
unsigned char teki_pat[MAX_TEKI];			/* 敵パターン番号 */
unsigned char teki_dir[MAX_TEKI];				/* 敵移動方向保存用変数 */

unsigned char teki_next[MAX_TEKI + 2];			/* 敵表示フラグ */
unsigned char teki_free[MAX_TEKI + 2];			/* 敵表示フラグ */

unsigned int tkshotcount[MAX_TEKI];	/* 敵弾発射カウンタ */

unsigned char teki_hp[MAX_TEKI];
unsigned char teki_pal[MAX_TEKI];
unsigned char teki_dmgtime[MAX_TEKI];

unsigned char teki_move[MAX_TEKI];
unsigned char teki_shotinfo[MAX_TEKI];

unsigned char teki_num = 0;
unsigned char stage;
unsigned char tmp;
short timeup;
unsigned char trgcount = 0;	/* ショット間隔リミッタ */
unsigned char trgcount2 = 0;	/* 連射リミッタ */
unsigned char trgnum = 0;
unsigned char total_count = 0;
unsigned char renshaflag = TRUE; /*FALSE;*/

/* 敵弾情報 */
/* 敵弾座標管理 */
short tkshot_x[MAX_TKSHOT];
short tkshot_y[MAX_TKSHOT];

unsigned char tkshot_next[MAX_TKSHOT + 2];
unsigned char tkshot_free[MAX_TKSHOT + 2];

int tkshot_xx[MAX_TKSHOT];			/* 敵弾移動増分X */
int tkshot_yy[MAX_TKSHOT];			/* 敵弾移動増分Y */

unsigned char tkshot_pat[MAX_TKSHOT];

unsigned char tkshotnum;						/* 敵弾 現在の表示数 */
unsigned char tkshot_c;						/* 敵弾発射間隔 初期値 */

unsigned char scrdspflag;						/* スコア文字更新フラグ */

/* テンポラリ用グローバル変数 */
unsigned char chrnum;
unsigned char chrpal;
const SPR_COMB *sprcomb;

short tmp_x, tmp_y;
short tmp_xx, tmp_yy;
short tmp_xxx, tmp_yyy;

//unsigned char spr_count[2];

COLIGION *p_coli;

short xxx;
unsigned char yyy, sprpal_no; // spratr, patno,

short X, Y;
unsigned char PAT, PAL;

unsigned char keycode = 0;
unsigned char wake_count = 0;
unsigned char muki = PAT_JIKI_C;

/******************************************************************************/

/* 敵弾消去マクロ */
//	tkshot_y[NO] = tmp_y = SPR_DEL_Y;\

#define del_tkshot(NO) {\
	tkshotnum--;\
}

#define put_my_hp() my_hp_flag = TRUE;

unsigned char str_temp[11];

//int i,j;
unsigned char i,j, j2, k;
//int k;



void my_dmg(void)
{
	if(my_hp)
	{
		if(!mypal_dmgtime){
			--my_hp;
			put_my_hp();
			seflag = 1;
		}
	}
	mypal = REVPAL_NO;
	mypal_dmgtime = DMGTIME * 2;

	noshotdmg_flag = TRUE;
}



unsigned char x = 0;
int loopcounter = 0;
unsigned int soundtestno = 0;
int soundtest = FALSE;


/* ゲ－ムのル－プ */
unsigned char a = 0, b = 0;
//unsigned char i,j;
//int i, j,
short int  xx, yy,*p_x,*p_y;
unsigned char pat_no;

/*****************************/
/* 敵機弾移動方向決定(32way) */
/*****************************/

//int
unsigned char l = 0;

unsigned char tekishot_dir(void)
{
//	unsigned char l = 0;
	l = 0;

	tmp_yy = my_data_x[0] - tmp_x;
	tmp_xx = my_data_y[0] - tmp_y + scrlspd;

	if(tmp_xx < 0){
		tmp_xx = -tmp_xx;
		if(tmp_yy < 0){
			l = 16;
			tmp_yy = -tmp_yy;
		}else{
			l = 24;
		}
	}else if(tmp_yy < 0){
		l = 8;
		tmp_yy = -tmp_yy;
	}

	if((l == 24) || (l == 8)){
		if(tmp_yy < tmp_xx){
			tmp_yy *= 8;
			if((tmp_yy) < tmp_xx)
				l += 0;
			else if((tmp_yy) < tmp_xx * 3)
				l += 1;
			else if((tmp_yy) < (tmp_xx * 5))
				l += 2;
			else if((tmp_yy) < (tmp_xx * 7))
				l += 3;
			else
				l += 4;
		}else{
			tmp_xx *= 8;
			if((tmp_yy * 7) < (tmp_xx))
				l += 4;
			else if((tmp_yy * 5) < (tmp_xx))
				l += 5;
			else if((tmp_yy * 3) < (tmp_xx))
				l += 6;
			else if((tmp_yy) < (tmp_xx))
				l += 7;
			else{
				l += 8;
				l %= 32;
			}
		}
	}else{
		if(tmp_yy < tmp_xx){
			tmp_yy *= 8;
			if((tmp_yy) < tmp_xx){
				l += 8;
				l %= 32;
			}
			else if((tmp_yy) < (tmp_xx * 3)){
				l += 7;
			}
			else if((tmp_yy) < (tmp_xx * 5))
				l += 6;
			else if((tmp_yy) < (tmp_xx * 7))
				l += 5;
			else
				l += 4;
		}else{
			tmp_xx *= 8;
			if((tmp_yy * 7) < (tmp_xx))
				l += 4;
			else if((tmp_yy * 5) < (tmp_xx))
				l += 3;
			else if((tmp_yy * 3) < (tmp_xx))
				l += 2;
			else if(tmp_yy < (tmp_xx))
				l += 1;
		}
	}
	return l;
}

/**********************/
unsigned char dir, pat;

void shottome(void) //unsigned char dir, unsigned char pat)
{
//	int j;
//	unsigned char j;

	if(tkshot_free[MAX_TKSHOT] != END_LIST){
		ADD_LIST(MAX_TKSHOT, k, tkshot_next, tkshot_free);

		if(uramode > 1){
			tkshot_x[k] = tmp_x + (4 << SHIFT_NUM) + (tkshot_xx[k] = direction[dir][0] * 3 / 2);
			tkshot_y[k] = tmp_y + (4 << SHIFT_NUM_Y) + (tkshot_yy[k] = direction[dir][1] * 3 / 2);
		}else{
			tkshot_x[k] = tmp_x + (4 << SHIFT_NUM) + (tkshot_xx[k] = direction[dir][0]);
			tkshot_y[k] = tmp_y + (4 << SHIFT_NUM_Y) + (tkshot_yy[k] = direction[dir][1]);
		}
		if(pat == PAT_TKSHOT1)
			if(tkshot_xx[k] > 0)
				pat = PAT_TKSHOT3;
		tkshot_pat[k] = pat;
		tkshotnum++;
	}
}

void shottome2(void) //unsigned char dir, unsigned char pat)
{
//	int j;
//	unsigned char j;
//	return;

	if(tkshot_free[MAX_TKSHOT] != END_LIST){
		ADD_LIST(MAX_TKSHOT, k, tkshot_next, tkshot_free);
		tkshot_x[k] = tmp_x + (4 << SHIFT_NUM) + (tkshot_xx[k] = direction[dir][0] * 3 / 2);
		tkshot_y[k] = tmp_y + (4 << SHIFT_NUM_Y) + (tkshot_yy[k] = direction[dir][1] * 3 / 2);

		if(pat == PAT_TKSHOT1)
			if(tkshot_xx[k] > 0)
				pat = PAT_TKSHOT3;
		tkshot_pat[k] = pat;
		tkshotnum++;
	}

}

/**********************/
/* 敵機弾発射サブ関数 */
/**********************/
void shotcheck1(void)//(unsigned char i)	/* NORMAL */
{
	if(tkshotcount[i] != 0){
		tkshotcount[i]--;
		return;
	}

	tkshotcount[i] = tkshot_c;
	dir = tekishot_dir();
	pat = PAT_TKSHOT1;
	shottome(); //tekishot_dir(), PAT_TKSHOT1);
}

void shotcheck2(void)//(unsigned char i)	/* 3 WAY */
{
//	int l;
//	unsigned char l;

	if(tkshotcount[i] != 0){
		tkshotcount[i]--;
		return;
	}

	tkshotcount[i] = tkshot_c;
	l = tekishot_dir();

	dir = l;
	pat = PAT_TKSHOT1;
	shottome(); //l, PAT_TKSHOT1);
	dir = (l + 2) % 32;
	pat = PAT_TKSHOT1;
	shottome(); //(l + 2) % 32, PAT_TKSHOT1);
	dir = (l - 2 + 32) % 32;
	pat = PAT_TKSHOT1;
	shottome(); //(l - 2 + 32) % 32, PAT_TKSHOT1);
}

void shotcheck3(void)//(unsigned char i)	/* RANDOM */
{
	if(tkshotcount[i] != 0){
		tkshotcount[i]--;
		return;
	}

	tkshotcount[i] = tkshot_c;
	dir = rand() % 32;
	pat = PAT_TKSHOT1;
	shottome(); //rand() % 32, PAT_TKSHOT1);
}

void shotcheck4(void)//(unsigned char i)	/* LASER */
{
	if(tkshotcount[i] == 0){
		return;
	}
	if(tkshotcount[i]-- < (SHOTCOUNT - 1)){
		if(tkshotcount[i] & 2){
			dir = DIR_DOWN;
			pat = PAT_TKSHOT2;
			shottome(); //DIR_DOWN, PAT_TKSHOT2);
		}
	}
}

void dummy(void)//(unsigned char i)
{
}

/************/

#define SHOTCHECKMAX 7

SUBFUNC shotcheck_tbl[SHOTCHECKMAX] ={
	shotcheck1,
	shotcheck2,
	shotcheck3,
	shotcheck4,
	dummy,
	dummy,
	dummy,
};

void do_shotcheck(void)//(unsigned char i)
{
/*	SUBFUNC p;*/
//	int j;

	j = teki_shotinfo[i];
	if(j < SHOTCHECKMAX)
		if(j >= 0)
//			shotcheck1();
			shotcheck_tbl[j](); //(i);
}

/******************/
/* 敵機弾移動サブ */
/******************/

void move_tekishot(void)
{
//	int i, j, k;
//	unsigned char i, j, k;

	SEARCH_LIST2(MAX_TKSHOT, i, j, tkshot_next){
		k = tkshot_pat[i];
		tmp_x = (tkshot_x[i] += (tkshot_xx[i]));
//		tmp_y = tkshot[i].y;

//		DEF_SP(tmp_x, tmp_y, k, CHRPAL_NO);

		/* 敵機弾画面外消去 */
		if((unsigned short)(tmp_x) > ((SCREEN_MAX_X + 16) << SHIFT_NUM)){
			del_tkshot(i);
			tmp_y = tkshot_y[i];
			DEL_LIST(MAX_TKSHOT, i, j, tkshot_next, tkshot_free);
		}else{
			tmp_y = tkshot_y[i] += (tkshot_yy[i] - scrlspd);
			if((unsigned short)(tmp_y) > ((SCREEN_MAX_Y + 16) << SHIFT_NUM_Y)){
				del_tkshot(i);
				DEL_LIST(MAX_TKSHOT, i, j, tkshot_next, tkshot_free);

			}/* 敵機弾と自機の当たり判定→自機ダメージ */
			else{
//				if(hit_check == TRUE){

					 if((tmp_xx = my_data_x[0] - tmp_x) < ((TKD_OFS_X - MY_OFS_X + TKD_SIZE_X) << SHIFT_NUM)){
						if(tmp_xx > ((TKD_OFS_X - MY_OFS_X - MY_SIZE_X) << SHIFT_NUM)){
							if((tmp_yy = my_data_y[0] - tmp_y) < ((TKD_OFS_Y - MY_OFS_Y + TKD_SIZE_Y) << SHIFT_NUM_Y)){
								if(tmp_yy > ((TKD_OFS_Y - MY_OFS_Y - MY_SIZE_Y) << SHIFT_NUM_Y)){
									/* 自機ダメージ */
									del_tkshot(i);
									DEL_LIST(MAX_TKSHOT, i, j, tkshot_next, tkshot_free);
									my_dmg();
								}
							}
						}
//					}
				}
#ifdef MSX2
				X = tmp_x;
				Y =  tmp_y + (0 << SHIFT_NUM_Y);
				PAT = k * 8;
				PAL  = CHRPAL_NO;
				DEF_SP_SINGLE(); //tmp_x, tmp_y + (0 << SHIFT_NUM_Y), k * 8, CHRPAL_NO);
//				DEF_SP_SINGLE(tmp_x, tmp_y, 8 * 4, CHRPAL_NO);
#else
				DEF_SP(spr_count, tmp_x, tmp_y, k, CHRPAL_NO, PRO_TEKISHOT);
#endif
			}
		}
	}
}

void check_revshot(void)
{
//	int l;
	return;
	if(uramode > 2){
		l = tekishot_dir();

		dir = l;
		pat = PAT_TKSHOT1;
		shottome2(); //l, PAT_TKSHOT1);
		dir = (l + 2) % 32;
		pat = PAT_TKSHOT1;
		shottome2(); //(l + 2) % 32, PAT_TKSHOT1);
		dir = (l - 2 + 32) % 32;
		pat = PAT_TKSHOT1;
		shottome2(); //(l - 2 + 32) % 32, PAT_TKSHOT1);
	}else{
		dir = tekishot_dir();
		pat = PAT_TKSHOT1;
		shottome2(); //tekishot_dir(), PAT_TKSHOT1);
	}
}

/***********************/
/* 敵爆発パターンその1 */
/***********************/
void move_teki_bomb(void)//(unsigned char i)
{
	switch(tkcount[i]){
		case 0:
			seflag = 1;
//			teki_pat[i] = PAT_BAKU1;
			teki_pal[i] = REVPAL_NO;
			break;

		case 5:
//			teki_pat[i] = PAT_BAKU2;
			teki_pal[i] = REVPAL_NO;
			break;

		case 10:
//			teki_pat[i] = PAT_BAKU3;
			teki_pal[i] = REVPAL_NO;
			if(uramode)
				check_revshot();
			break;

		case 13:
			if(uramode)
				check_revshot();
			break;

		case 15:
			if(uramode)
				check_revshot();
//			teki_hp[i] = 0;
//			teki_y[i] = SPR_DEL_Y;

			teki_hp[i] = 3;
			teki_pat[i] = PAT_STAR1;
			teki_move[i] = 11;
			teki_pal[i] = CHRPAL_NO;

			break;
	}

	tkcount[i]++;
}

//#define DEBUG

//#ifndef DEBUG

void move_teki_pat0(void)//(unsigned char i)
{
//	tmp_x -= sinwave[tkcount[i] / SINRATE] * ((3 << SHIFT_NUM) / 2);

//	if(++tkcount[i] >= (SINFREQ * SINRATE))
//		tkcount[i] = 0;

//	tmp_x += sinwave[tkcount[i] / SINRATE] * ((3 << SHIFT_NUM) / 2);

	/* 敵機縦方向移動処理 */
//	if(!(tmp_y > (128 << SHIFT_NUM_Y)))
//		tmp_y += ((3 << SHIFT_NUM_Y) / 2);	/* ゆっくり */
//	else
		tmp_y += (2 << SHIFT_NUM_Y);	/* 高速 */

	/* 敵機画面外消去(下方向) */
	if(tmp_y > (SPR_MAX_Y)){
		teki_hp[i] = 0;
	}
}

/***********************/
/* 敵移動パターンその1 */
/* HENTAI SINWAVE      */
/***********************/
void move_teki_pat1(void)//(unsigned char i)
{
	tmp_x -= sinwave[tkcount[i] / SINRATE] * ((3 << SHIFT_NUM) / 2);

	if(++tkcount[i] >= (SINFREQ * SINRATE))
		tkcount[i] = 0;

	tmp_x += sinwave[tkcount[i] / SINRATE] * ((3 << SHIFT_NUM) / 2);

	/* 敵機縦方向移動処理 */
	if(!(tmp_y > (128 << SHIFT_NUM_Y)))
		tmp_y += ((3 << SHIFT_NUM_Y) / 2);	/* ゆっくり */
	else
		tmp_y += (2 << SHIFT_NUM_Y);	/* 高速 */

	/* 敵機画面外消去(下方向) */
	if(tmp_y > (SPR_MAX_Y)){
		teki_hp[i] = 0;
	}
}

/***********************/
/* 敵移動パターンその2 */
/* FIRE2               */
/***********************/
//int dir, 
unsigned char dir2;

void move_teki_pat2(void)//(unsigned char i)
{

	if(++tkcount[i] < 800){
	if(tmp_x < (16 << SHIFT_NUM))
		teki_dir[i] = tekishot_dir();

	if(tmp_y < (16 << SHIFT_NUM_Y))
		teki_dir[i] = tekishot_dir();

	if(tmp_x > (SCREEN_MAX_X << SHIFT_NUM))
		teki_dir[i] = tekishot_dir();

	if(tmp_y > (SCREEN_MAX_Y << SHIFT_NUM_Y))
		teki_dir[i] = tekishot_dir();
	}else{
		/* 敵機画面外消去(左方向) */
		if(tmp_x < 0)
			teki_hp[i] = 0;

		/* 敵機画面外消去(右方向) */
		if(tmp_x > (SPR_MAX_X))
			teki_hp[i] = 0;

		/* 敵機画面外消去(上方向) */
		if(tmp_y < 0)
			teki_hp[i] = 0;

		/* 敵機画面外消去(下方向) */
		if(tmp_y > (SPR_MAX_Y))
			teki_hp[i] = 0;
	}
	dir = teki_dir[i];
	tmp_x +=  (direction[dir][0]);
	tmp_y +=  (direction[dir][1]);
}

/***********************/
/* 敵移動パターンその3 */
/* GURUGURU            */
/***********************/
void move_teki_pat3(void)//(unsigned char i)
{
//	int dir, dir2;

	if(tkcount[i] > 800){
		dir = teki_dir[i];
		/* 敵機画面外消去(左方向) */
		if(tmp_x < 0)
			teki_hp[i] = 0;

		/* 敵機画面外消去(右方向) */
		if(tmp_x > (SPR_MAX_X))
			teki_hp[i] = 0;

	}else if((++tkcount[i] % 7) == 1){
		dir2 = tekishot_dir();
		dir = teki_dir[i];
//		if( (DIR_OFF + dir - dir2 + DIR_OFF/2) % DIR_OFF > (DIR_DOWN + DIR_OFF/2) )
		if((32 + dir - dir2) % 32 > 16)
			dir++;
		else
			dir--;
		dir = teki_dir[i] = (dir + DIR_OFF) % DIR_OFF;
	}else
		dir = teki_dir[i];

	tmp_x +=  (direction[dir][0]);
	tmp_y +=  (direction[dir][1]);

//	teki_pat[i] = teki5_pat[dir / 2];

	/* 敵機画面外消去(上方向) */
	if(tmp_y < 0)
		if(direction[dir][1] < 0)
			teki_hp[i] = 0;

	/* 敵機画面外消去(下方向) */
	if(tmp_y > (SPR_MAX_Y))
		teki_hp[i] = 0;


}

/***********************/
/* 敵移動パターンその4 */
/* FIRE1               */
/***********************/
void move_teki_pat4(void)//(unsigned char i)
{
//	int dir;

	if(tkcount[i] > 800){
		dir = teki_dir[i];
		tmp_x +=  (direction[dir][0]);
		tmp_y +=  (direction[dir][1]);

		/* 敵機画面外消去(左方向) */
		if(tmp_x < 0)
			teki_hp[i] = 0;

		/* 敵機画面外消去(右方向) */
		if(tmp_x > (SPR_MAX_X))
			teki_hp[i] = 0;

	}else if(!tkcount[i] || !(tkcount[i] % 192)){
		++tkcount[i];
		dir = teki_dir[i] = tekishot_dir();

		tmp_x +=  (direction[dir][0]);
		tmp_y +=  (direction[dir][1]);
	}else if(((tkcount[i] % 192) < 80)){ 
		++tkcount[i];
		dir = teki_dir[i];

		tmp_x +=  (direction[dir][0]);
		tmp_y +=  (direction[dir][1]);

		if(tmp_x < (16 << SHIFT_NUM))
			teki_dir[i] = tekishot_dir();
		if(tmp_y < (16 << SHIFT_NUM_Y))
			teki_dir[i] = tekishot_dir();
		if(tmp_x > (SCREEN_MAX_X << SHIFT_NUM))
			teki_dir[i] = tekishot_dir();
		if(tmp_y > (SCREEN_MAX_Y << SHIFT_NUM_Y))
			teki_dir[i] = tekishot_dir();

	}else
		++tkcount[i];

	/* 敵機画面外消去(上方向) */
	if(tmp_y < 0)
		if(direction[dir][1] < 0)
			teki_hp[i] = 0;

	/* 敵機画面外消去(下方向) */
	if(tmp_y > (SPR_MAX_Y))
		teki_hp[i] = 0;

}

/***********************/
/* 敵移動パターンその5 */
/* HERIZAKO            */
/***********************/
void move_teki_pat5(void)//(unsigned char i)
{
//	int dir;

	if(!tkcount[i]){
		dir = teki_dir[i] = tekishot_dir();
		++tkcount[i];

	}else{
		dir = teki_dir[i];
		if((dir != DIR_OFF) && (dir != DIR_UP)){
			if(++tkcount[i] > 102){
				tkcount[i] = 1; //96;
				dir = DIR_OFF;
//				if(direction[dir][0] > 0){
//					++dir;	/* 左回転 */
//					dir %= DIR_OFF;
//				}else{
//					--dir;	/* 右回転 */
//				}
				teki_dir[i] = dir;
			}
		}
		else if(dir == DIR_OFF){
			if(++tkcount[i] > 30){
				tkcount[i] = 96;
				dir = DIR_UP;
				teki_dir[i] = dir;
			}
		}
	}
	tmp_x +=  (direction[dir][0]);
	tmp_y +=  (direction[dir][1]);


	/* 敵機画面外消去(上方向) */
	if(dir == DIR_UP){
		if(tmp_y < 0){
			teki_hp[i] = 0;
		}
	}
	if(tmp_y > (SPR_MAX_Y)){
		teki_hp[i] = 0;
	}
}

/***********************/
/* 敵移動パターンその6 */
/* レーザー            */
/***********************/
void move_teki_pat6(void)//(unsigned char i)
{
//	int dir;

	dir = teki_dir[i];
	if(tkcount[i] == (120 + 16)){
		dir = tekishot_dir();
		teki_shotinfo[i] = 0;
	}else if(tkcount[i] == (40 + 16)){
		dir = DIR_OFF;
		tkshotcount[i] = SHOTCOUNT * 2;
		teki_shotinfo[i] = 3;
	}

	teki_dir[i] = dir;
	++tkcount[i];

	tmp_x +=  (direction[dir][0]);
	tmp_y +=  (direction[dir][1]);


	/* 敵機画面外消去(下方向) */
	if(tmp_y > (SPR_MAX_Y)){
		teki_hp[i] = 0;
	}
	/* 敵機画面外消去(上方向) */
	if(tmp_y < 0){
		if(direction[dir][1] < 0){
			teki_hp[i] = 0;
		}
	}
	/* 敵機画面外消去(左方向) */
	if(tmp_x < 0)
		teki_hp[i] = 0;

	/* 敵機画面外消去(右方向) */
	if(tmp_x > (SPR_MAX_X))
		teki_hp[i] = 0;
}

/*************************/

void boss_tkappend(int i, int pat, int move, int dir, int shot)
{
//	int j;

	if(teki_free[MAX_TEKI] == END_LIST)
		return;

	ADD_LIST(MAX_TEKI, j, teki_next, teki_free);

	teki_pat[j] = pat;
	tkcount[j] = 0;
	tkshotcount[j] = SHOTCOUNT - 2;

	teki_move[j] = move;
	teki_y[j] = teki_y[i];
	teki_x[j] = teki_x[i];

	teki_hp[j] = 4;
	teki_pal[j] = CHRPAL_NO;
	teki_shotinfo[j] = shot;
	teki_dir[j] = dir;
	teki_num++;
}

/*************************/
/* ボス移動パターンその1 */
/*************************/
void move_boss_pat1(void)//(unsigned char i)
{
//	int dir;
	dir = teki_dir[i];

	if(++tkcount[i] == (40 - 4)){
		dir = DIR_RIGHT;
	}else if((tkcount[i] % 600) == (40 - 4)){
		tkshotcount[i] = SHOTCOUNT * 3 / 2;
		dir = DIR_DOWN;
	}else if(tkcount[i] > (40 - 4)){
		switch(dir){
			case DIR_UP:
				if(tmp_y < ((64-8) << SHIFT_NUM_Y)){
					dir = DIR_OFF;
					tkshotcount[i] = SHOTCOUNT * 3 / 2;
					boss_tkappend(i, PAT_TEKI4, 6, DIR_DOWN - 4, 0);
					boss_tkappend(i, PAT_TEKI4, 6, DIR_DOWN + 4, 0);
					boss_tkappend(i, PAT_TEKI4, 6, DIR_DOWN, 0);
				}
				break;

			case DIR_RIGHT:
				if(tmp_x > ((256 - 24) << SHIFT_NUM)){
					dir = DIR_LEFT;
				}
				break;

			case DIR_DOWN:
				if(tmp_y > (192 << SHIFT_NUM_Y)){
					tkshotcount[i] = SHOTCOUNT * 3 / 2;
					if(tkcount[i] < 3500)
						dir = DIR_UP;
				}
				break;

			case DIR_LEFT:
				if(tmp_x < (40 << SHIFT_NUM)){
					dir = DIR_RIGHT;
				}
				break;

			case DIR_OFF:
				if((tkcount[i] % 200) == 50){
					dir = DIR_LEFT;
				}
				break;
		}
	}
	/* 敵機画面外消去(下方向) */
	if(tmp_y > (SPR_MAX_Y))
		teki_hp[i] = 0;

	tmp_x +=  (direction[dir][0]);
	tmp_y +=  (direction[dir][1]);
	teki_dir[i] = dir;

#ifdef MSX2
	if(total_count % 2)
		teki_pat[i] = PAT_BOSS1_2;
	else
		teki_pat[i] = PAT_BOSS1;
#else
	teki_pat[i] = PAT_BOSS1;
#endif
}

/*************************/
/* スター移動パターン    */
/*************************/
void move_star_pat1(void)
{
	/* 縦方向移動処理 */
	if(!(tmp_y > (128 << SHIFT_NUM)))
		tmp_y += ((3 << SHIFT_NUM) / 2);	/* ゆっくり */
	else
		tmp_y += (2 << SHIFT_NUM);	/* 高速 */

	/* 画面外消去(下方向) */
	if(tmp_y > (SPR_MAX_Y)){
		teki_hp[i] = 0;
	}
}

/************/

#define MOVETEKIMAX 12

SUBFUNC moveteki_tbl[MOVETEKIMAX] = 
{
	move_teki_bomb,
	move_teki_pat1,//	move_teki_pat1,
	move_teki_pat2,//	move_teki_pat2,
	move_teki_pat3,//	move_teki_pat3,
	move_teki_pat4,//	move_teki_pat4,
	move_teki_pat5,//	move_teki_pat5,
	move_teki_pat6,//	move_teki_pat6,
	dummy,
	dummy,
	move_boss_pat1,//	move_boss_pat1,
	dummy,
	move_star_pat1,
};

/************/
/* 敵機移動 */
/************/
void move_teki(void)
{
//	int i, j, k, l;
//	unsigned char i, j, k, l;

	SEARCH_LIST2(MAX_TEKI, i, k, teki_next){

		/* 敵機移動処理 */
		tmp_x = teki_x[i];
		tmp_y = teki_y[i];

		l = teki_move[i];

		if(l >= 0)
			if(l < MOVETEKIMAX)
				moveteki_tbl[l](); //(i);
			else
				return;
//				move_teki_pat0(); //i);

		teki_x[i] = tmp_x;
		teki_y[i] = tmp_y;

		if(teki_hp[i] > 2){
//			if(hit_check == TRUE){

				/* 敵機と自機弾の接触判定 → 敵機、自機弾消滅 スコア増加 */
				SEARCH_LIST2(MAX_MYSHOT, j, j2, myshot_next){
//					if((tmp_yy = myshot[j].y - tmp_x) < (16 << SHIFT_NUM_Y)){
//						if(tmp_yy > ((TK_OFS_Y - MY_OFS_Y - 8) << SHIFT_NUM_Y)){
					if((tmp_xx = myshot_x[j] - tmp_x) < (16 << SHIFT_NUM)){
//						if(tmp_xx > (-20 << SHIFT_NUM)){
						if(tmp_xx > (-(4 << SHIFT_NUM))){
							if((tmp_yy = myshot_y[j] - tmp_y) < (16 << SHIFT_NUM_Y)){
//								if(tmp_yy > (-20 << SHIFT_NUM_Y)){
								if(tmp_yy > (-(4 << SHIFT_NUM_Y))){
									if(myshot_y[j] != SPR_DEL_Y){

										if(teki_pat[i] != PAT_STAR1){

											/* 敵ダメージ */
											teki_pal[i] = REVPAL_NO;
											teki_dmgtime[i] = DMGTIME;

//											if(teki_hp[i] > 2)
												--teki_hp[i];

											if(uramode){
												dir = tekishot_dir();
												pat = PAT_TKSHOT1;
												shottome2(); //tekishot_dir(), PAT_TKSHOT1);
											}

											/* ノーマル弾なら画面外に消去 */
//											myshot_x[j] = SPR_DEL_X;
//											myshot_y[j] = SPR_DEL_Y; //0; //SPR_DEL_Y;	/* 0 */
											/*myshot[j].yy = 0;*/

											trgnum--;
											DEL_LIST(MAX_MYSHOT, j, j2, myshot_next, myshot_free);

											score += 10;		/* 敵の点数を足す */
											scrdspflag = TRUE;
										}
										break;
									}
								}
							}
						}
					}
				}
//			}
		}

		if(teki_hp[i] > 2){
//			if(hit_check == TRUE){
				/* 敵機と自機の接触判定 → 自機ダメージ */
				p_coli = &hantei[0];
				if((tmp_xx = my_data_x[0] - tmp_x) < p_coli->x1){
					if(tmp_xx > p_coli->x2){
						if((tmp_yy = my_data_y[0] - tmp_y) < p_coli->y1){
							if(tmp_yy > p_coli->y2){

								/* 自機ダメージ */
								if(teki_pat[i] != PAT_STAR1){
									my_dmg();
								}else{
									seflag = 4;
									teki_hp[i] = 0;
									score += 500;
									scrdspflag = TRUE;
								}
							}
						}
					}
				}
//			}

			if(teki_pat[i] != PAT_STAR1)
				do_shotcheck(); //i);
		}
#ifdef MSX2
		X = teki_x[i];
		Y =teki_y[i] ;
		PAT = teki_pat[i];
		PAL = teki_pal[i];
		DEF_SP_DOUBLE(); //teki_x[i], teki_y[i], teki_pat[i], teki_pal[i]);
#else
		DEF_SP(spr_count, teki_x[i], teki_y[i], teki_pat[i], teki_pal[i], PRO_TEKI);
#endif
//		DEF_SP_SINGLE(teki_x[i], teki_y[i], 2*4, teki_pal[i]);
//		DEF_SP_SINGLE(teki_x[i], teki_y[i], 3*4, teki_pal[i]);

		/* 敵消滅 → 爆発パターンへ */
		if(teki_hp[i] <= 0){
//			teki_y[i] = SPR_DEL_Y;
			DEL_LIST(MAX_TEKI, i, k, teki_next, teki_free);
			--teki_num;
		}
		else if(teki_hp[i] == 2){
			teki_hp[i] = 1;
			teki_move[i] = 0;
			tkcount[i]  = 0;
			teki_dmgtime[i] = 0;
			teki_pal[i] = CHRPAL_NO;

			score += 100;		/* 敵の点数を足す */
			scrdspflag = TRUE;

		}else if(teki_dmgtime[i]){
			teki_dmgtime[i]--;
			if(!teki_dmgtime[i])
				teki_pal[i] = CHRPAL_NO;
		}
//		DEF_SP(teki[i].x, teki[i].y, teki_pat[i], teki_pal[i]);
	}
}

/**********/
/* 敵出現 */
/**********/
long teki_x2;

void com_tkappend(void) //unsigned char teki_no)
{
//	int i;
//	teki_no = i;

	if(teki_free[MAX_TEKI] == END_LIST)
		return;

	ADD_LIST(MAX_TEKI, j, teki_next, teki_free);

	teki_pat[j] = i; //teki_no;
	tkcount[j] = 0;
	tkshotcount[j] = SHOTCOUNT - 2;

	teki_move[j] = command[schedule_ptr + 2];
	teki_x2 = (((command[schedule_ptr + 3] - 7) << SHIFT_NUM));
	teki_x2 *= 256;
	teki_x2 /= 144;
	teki_x[j] = teki_x2;
	teki_y[j] = (command[schedule_ptr + 4] << SHIFT_NUM_Y);

	teki_hp[j] = command[schedule_ptr + 5];
	teki_pal[j] = CHRPAL_NO;

	teki_shotinfo[j] = command[schedule_ptr + 6];

	teki_dir[j] = DIR_DOWN;
	teki_num++;
}


/**************************************/

void com_waitcount(void)//(unsigned char i)
{
	waitcount = i;
}

void com_tkalldel(void)//(unsigned char i)
{
	waitcount = 1;
}

void com_bgmchange(void)//(unsigned char i)
{
/*	playbgm(i, 0);*/
}

void com_bgmfadeout(void)//(unsigned char i)
{
/*	S_IL_FUNC(bgm_fadeout());*/
}

void com_dummy(void)//(unsigned char i)
{
	waitcount = 1;
}

void com_putmessage(void)//(unsigned char i)
{
	put_strings(SCREEN2, command[schedule_ptr + 1], command[schedule_ptr + 2], (char *)message[command[schedule_ptr + 3]], CHRPAL_NO);
}

void com_putstage(void)//(unsigned char i)
{
	unsigned char str_x = command[schedule_ptr + 1];
	unsigned char str_y = command[schedule_ptr + 2];

	if(stage < STAGEMAX)
		++stage;

	put_strings(SCREEN2, str_x, str_y, "STAGE", CHRPAL_NO);
	if(stage > 0){
		if(stage < 10)
			put_numd((long)stage, 1);
		else
			put_numd((long)stage, 2);
		put_strings(SCREEN2, str_x + 6, str_y, str_temp, CHRPAL_NO);
	}
}

void com_end(void)//(unsigned char i)
{
	waitcount = 1;
}

void com_se(void)//(unsigned char i)
{
	seflag = command[schedule_ptr + 1];
}

/**************************************/
/* スケジュール解析で行うコマンド
・敵追加
・処理待ち
・BGM変更(F-Out)
・メッセージ表示
・各種エフェクト
*/
/**************************************/

/* スケジュール用ジャンプテーブル enumの値と対応する */
#define COMMAND_MAX 10

SUBFUNC command_tbl[COMMAND_MAX] = {
	com_dummy,
	com_waitcount,
	com_tkalldel,
	com_tkappend,
	com_bgmchange,
	com_bgmfadeout,
	com_putmessage,
	com_putstage,
	com_end,
	com_se,
};

/********************/
/* スケジュール実行 */
/********************/
void do_schedule(void)
{
/*	SUBFUNC p;*/

	while(1){
//	printf("Test %d %d\n", command_num, waitcount);

	/* イベント処理待ちなら戻る */
	if(waitcount){
		switch(command_num){
			case COM_WAITCOUNT:
				--waitcount;
				return;
				break;

			case COM_TKALLDEL:
				if(teki_next[MAX_TEKI] == END_LIST)
					waitcount = 0;
				return;
				break;

			case COM_DUMMY:
				return;
				break;

			case COM_END:
				--waitcount;
				schedule_ptr = 0;
				if(tkshot_c > (1 << SHIFT_NUM))
					tkshot_c /= 2;
				else{
					if(uramode != URAMAX){
						uramode++;
						if(uramode != URAMAX)
							tkshot_c = 6 << SHIFT_NUM;
						else
							tkshot_c = 0;
					}
				}
				if(!noshotdmg_flag){	/* PACIFIST */
					score += 5000; //10000;
					scrdspflag = TRUE;
					seflag = 4;
				}
				noshotdmg_flag = FALSE;

				if(my_hp < max_my_hp)
					my_hp = max_my_hp;
				else
					my_hp += 1;

				if(my_hp > HP_MAX)
					my_hp = HP_MAX;
				put_my_hp();
				break;
		}
	}

	command_num = command[schedule_ptr];	/* 次に実行するコマンド */

	if(command_num >= 0)
		if(command_num < COMMAND_MAX)
/*			if((p = command_tbl[command_num]) != NULL)*/
				i = command[schedule_ptr + 1];
				command_tbl[command_num]();//command[schedule_ptr + 1]);

	schedule_ptr += command_length[command_num];	/* 引数の数だけ先に進める */

	}
}


short game_loop(void){
//	unsigned char i,j;
//	short xx,yy;
//	tmp_spr_count = 0;

/* パッド入力 & 自機移動 */
/*	for(i=0;i<1;i++)*/
	i = 0;
	a = 0;
	b = 0;
	{
		if(keycode & KEY_A) /* Z,SPACE */
			a=1;
		if(keycode & KEY_B) /* X */
			b=1;


		if((keycode & KEY_START) || (keycode & KEY_B)){
//			if(keycode & (KEY_LEFT1))
//				scrlspd = SCRL_MIN;
//			else{
//				put_strings(SCREEN2, 14, 6, "PAUSE", CHRPAL_NO);
//			}
//			do {
				if(scrl_spd)
					put_strings(SCREEN2, 14, 13, "PAUSE", CHRPAL_NO);
//				bg_roll();

//				do_putmessage();

#ifdef MSX2
				game_put();
#else
				DEF_SP(spr_count, my_data_x[i], my_data_y[i], my_data_pat_num[i], mypal, PRO_JIKI); /* 0x14); */
				SEARCH_LIST2(MAX_MYSHOT, i, j, myshot_next){
					DEF_SP(spr_count, myshot_x[i], myshot_y[i], myshot_pat_num[i],  CHRPAL_NO, PRO_JIKISHOT);
				}
				SEARCH_LIST2(MAX_TEKI, i, j, teki_next){
					DEF_SP(spr_count, teki_x[i], teki_y[i], teki_pat[i], teki_pal[i], PRO_TEKI);
				}
				SEARCH_LIST2(MAX_TKSHOT, i, j, tkshot_next){
					DEF_SP(spr_count, tkshot_x[i], tkshot_y[i], tkshot_pat[i], CHRPAL_NO, PRO_TEKISHOT);
				}
#endif
			do{
				wait_vsync();
				set_sprite();
			}while((keyscan() & (KEY_START | KEY_B)));
			do {
				wait_vsync();
				set_sprite();
//				keycode = key_hit_check();
				keycode = keyscan();
				if(keycode & KEY_A){
//				if(keycode & KEY_B){
//					bg_roll();
//					set_sprite();
					return SYSEXIT;		/* 一気に抜ける */
				}
//				if(keycode & KEY_A){
//				if(keycode & KEY_B){
//					scrl_spd = SCRL_MIN;
//					bg_roll();
//					set_sprite();
//					put_strings(SCREEN2, 14, 13, "     ", CHRPAL_NO);
//					return ERRLV2;
//				}
			}while((!(keyscan() & (KEY_START | KEY_B))));
			do{
				wait_vsync();
				set_sprite();
			}while(keyscan() & (KEY_START | KEY_B));
			scrl_spd = SCRL_MIN;
			put_strings(SCREEN2, 14, 13, "     ", CHRPAL_NO);

			return ERRLV1;
//			continue;
		}

/* 00  0=hit 1=Nohit */
/* AB */

		if (b){
			if (scrl_spd < SCRL_MAX)
				scrl_spd++;
		}else if (scrl_spd > SCRL_MIN)
			scrl_spd--;

		if (a & b)
			scrl_spd = 0;

/* 自機移動(斜め方向対応) */
		xx = yy = 0;
		pat_no = PAT_JIKI_C;

		if(keycode & KEY_LEFT1){ /* 4 */
			if(wake_count & 8)
				pat_no = PAT_JIKI_L;
			else
				pat_no = PAT_JIKI_R;
			xx = -1;
			muki = PAT_JIKI_L;
		}
		if(keycode & KEY_RIGHT1){ /* 6 */
			if(wake_count & 8)
				pat_no = PAT_JIKI_R;
			else
				pat_no = PAT_JIKI_L;
			xx = 1;
			muki = PAT_JIKI_R;
		}
		if(keycode & KEY_UP1){ /* 8 */
			pat_no = PAT_JIKI_J;
			yy = -1;
		}
		if(keycode & KEY_DOWN1){ /* 2 */
			pat_no = PAT_JIKI_J;
			yy = 1;
		}
//		if(!xx)
//			muki = pat_no;
		my_data_pat_num[i] = pat_no;
		++wake_count;

		/* 斜めの時の処理(手抜き版) */
		if ((xx == 0) || (yy == 0)){
			xx *= 3; yy *= 3;
		}
		else{
			xx *= 2; yy *= 2;
		}
		xx <<= SHIFT_NUM;
		yy <<= SHIFT_NUM_Y;

		p_x=&my_data_x[i];
		p_y=&my_data_y[i];

		*p_x+=xx;
		*p_y+=yy;

	/* 自機が移動できる範囲を設定 */
		if(*p_y <= JIKI_MIN_Y)
			*p_y = JIKI_MIN_Y;
			else if(*p_y >= JIKI_MAX_Y)
				*p_y = JIKI_MAX_Y;
			if(*p_x <= JIKI_MIN_X)
				*p_x = JIKI_MIN_X;
			else if(*p_x >= JIKI_MAX_X)
				*p_x = JIKI_MAX_X;

/*		DI();
		write_vram_adr(spr_page, 0x7600);
		EI();
*/

#ifdef MSX2
		pchr_data = chr_data;
		X = *p_x;
		Y = *p_y;
		PAT = my_data_pat_num[i];
		PAL = mypal;
		DEF_SP_DOUBLE(); //*p_x, *p_y, my_data_pat_num[i], mypal);
#else
//		DEF_SP(spr_count, 16 << SHIFT_NUM, 16 << SHIFT_NUM_Y, PAT_JIKI_J, CHRPAL_NO, PRO_JIKI);


		DEF_SP(spr_count, *p_x, *p_y, my_data_pat_num[i], mypal, PRO_JIKI);
//		DEF_SP(spr_count, *p_x, *p_y, PAT_JIKI_C, mypal, PRO_JIKI);
#endif
//		EI();
//		DEF_SP_SINGLE(*p_x + (2 << SHIFT_NUM), *p_y, 2*4, mypal);
//		DEF_SP_SINGLE(*p_x + (2 << SHIFT_NUM), *p_y, 3*4, mypal);

		/* 自機弾発射 */
		if(trgcount)
			trgcount--;
		if(renshaflag == FALSE)
			if(trgcount2)
				trgcount2--;

		if (a  //&& ((MAX_MYSHOT - trgnum) >= 1) 
			&& (muki != PAT_JIKI_C)){
			noshotdmg_flag = TRUE;
			if(trgcount2){
				if(!trgcount){
					trgcount = 5;

					if(myshot_free[MAX_MYSHOT] != END_LIST){
						ADD_LIST(MAX_MYSHOT, tmp, myshot_next, myshot_free);
						myshot_x[tmp] = my_data_x[i] + (1 << SHIFT_NUM);
						myshot_y[tmp] = my_data_y[i];

						myshot_yy[tmp] = 0;
						if(muki == PAT_JIKI_R)
							myshot_xx[tmp] = (6 << SHIFT_NUM);
						else if(muki == PAT_JIKI_L)
							myshot_xx[tmp] = -(6 << SHIFT_NUM);

						if(muki == PAT_JIKI_R)
							myshot_pat_num[tmp] = PAT_MYSHOT2;
						else
							myshot_pat_num[tmp] = PAT_MYSHOT1;

						trgnum++;
					}
/*					if(myshot_free[MAX_MYSHOT] != END_LIST){
						ADD_LIST(MAX_MYSHOT, tmp, myshot_next, myshot_free);
						myshot_x[tmp] = my_data_x[i] + (13 << SHIFT_NUM);
						myshot_y[tmp] = my_data_y[i];

						myshot_xx[tmp] = 0;
						myshot_yy[tmp] = -(6 << SHIFT_NUM);

						myshot_pat_num[tmp] = PAT_MYSHOT1;

						trgnum++;
					}
*/				}
			}
		}else
			trgcount2 = 60 / 3;
	}

	/** スケジュール解析を実行 **/
	do_schedule();

	/* 自機弾移動 */
	SEARCH_LIST2(MAX_MYSHOT, i, j, myshot_next){
		tmp_x = myshot_x[i];
		tmp_y = myshot_y[i];

		if(tmp_y < SPR_DEL_Y){
			tmp_x += myshot_xx[i];
			tmp_y += myshot_yy[i];
		}
//		/* 自機弾画面外消去 */
		if((tmp_x < (((SCREEN_MIN_X + SPR_OFS_X) << SHIFT_NUM) + 16))
		|| (tmp_x > (((SCREEN_MAX_X - SPR_OFS_X) << SHIFT_NUM) + 16))){
//			tmp_y = SPR_DEL_Y;
//		}
//		if(tmp_y == SPR_DEL_Y){
			trgnum--;

//			DEF_SP(tmp_x, tmp_y, myshot[i].pat_num,  CHRPAL_NO);
//			DEF_SP_SINGLE(tmp_x, tmp_y + (4 << SHIFT_NUM), 6*4, CHRPAL_NO);
			DEL_LIST(MAX_MYSHOT, i, j, myshot_next, myshot_free);

		}else{
			myshot_x[i] = tmp_x;
			myshot_y[i] = tmp_y;
//			DI();
//			write_vram_adr(spr_page, 0x7600 + tmp_spr_count * 4);

#ifdef MSX2
			X = tmp_x;
			Y = tmp_y + (4 << SHIFT_NUM_Y);
			PAT =  myshot_pat_num[i] * 8;
			PAL = CHRPAL_NO;
			DEF_SP_SINGLE(); //tmp_x, tmp_y + (4 << SHIFT_NUM_Y), myshot_pat_num[i] * 8, CHRPAL_NO);
#else
			DEF_SP(spr_count, tmp_x, tmp_y, myshot_pat_num[i],  CHRPAL_NO, PRO_JIKISHOT);
#endif
//			EI();
//			DEF_SP_SINGLE(tmp_x, tmp_y + (4 << SHIFT_NUM), 0*4, CHRPAL_NO);
		}

	}

//	DI();
//	write_vram_adr(spr_page, 0x7600 + tmp_spr_count * 4);
//	EI();
	move_teki();
	move_tekishot();
//	EI();

#ifdef DEBUG_
	/* スプライト数表示 */
	if(old_count[spr_page] != tmp_spr_count){
		put_numd((long)(tmp_spr_count), 2);
		put_strings(SCREEN2, 27, 24, str_temp, CHRPAL_NO);
	}
#endif
//	if(jiffy_flag){
//		jiffy_flag = 0;
/*		put_numd((long)(total_count), 2);
		put_strings(SCREEN2, 14, 14, str_temp, CHRPAL_NO);
		total_count = 0;*/
//	}
//	spr_count[spr_page] = tmp_spr_count;

	return NOERROR;
}

unsigned char color_flag[MAX_SPRITE];

/* 変数初期化処理 */
void game_init(void){
	int i;

//	srand(time(NULL));	/* 乱数の初期化 */
	for(i = 0; i < MAX_SPRITE * 2; ++i){
//		chr_data[i].x = 0;
//		chr_data[i].y = 212;
		chr_data[i].pat_num = 255;
		chr_data[i].atr = 0x80;
	}

#ifdef MSX2
	for(i = 0; i < MAX_SPRITE; ++i){
		color_flag[i] = 0;
		for(j = 0; j < 2; ++j){
			old_data[j][i].pat_num = 255;
			old_data[j][i].atr = 0;
//			old_data[j][i].pal = 0;
		}
	}
#endif

	stage = 0;
	waitcount = 0;
	schedule_ptr = 0;
	command_num = COM_DUMMY;
	command = (int *)stg1_data;
	uramode = 0;
//	renshaflag = FALSE;
	renshaflag = TRUE;

	trgcount = 0;	/* ショット間隔リミッタ */
	trgcount2 = 0;	/* 連射リミッタ */
	trgnum = 0;
	total_count = 0;

//	scrl = 0;
	scrl_spd = SCRL_MIN;

	mypal = CHRPAL_NO;
	mypal_dmgtime = 0;
	my_movecount = 0;

	/* 敵表示情報初期化 */
	INIT_LIST(MAX_MYSHOT, i, myshot_next, myshot_free);
	INIT_LIST(MAX_TKSHOT, i, tkshot_next, tkshot_free);
	INIT_LIST(MAX_TEKI, i, teki_next, teki_free);


	score = 0;
	tkshot_c = (6 << SHIFT_NUM);
	max_my_hp = 4; //7;

	my_hp = max_my_hp;

	for(i = 0; i < MAX_TKSHOT; i++){
		tkshot_xx[i] = 0;
		tkshot_yy[i] = 0;
	}
	tkshotnum = 0;
	scrdspflag = TRUE;
	noshotdmg_flag = FALSE;

	seflag = 0;

/* リスト初期化(まだ未使用) */
/*	for(i=0;i<10;i++){
		start[i].next = &fin[i];
		start[i].prev = NULL;
		fin[i].next = NULL;
		fin[i].prev = &start[i];
	}
*/

	/* SPRITE 初期化 */
/*	for(i = 0; i < 256; i++)
		spr[i].y = SPR_DEL_Y;*/

/* 自機座標の初期化 */
	my_data_x[0] = 128 << SHIFT_NUM;
	my_data_y[0] = (18 * 8) << SHIFT_NUM;
	my_data_pat_num[0] = PAT_JIKI_C;
	my_data_x[1] = 192 << SHIFT_NUM;		/* 二人プレイを想定 */
	my_data_y[1] = 120 << SHIFT_NUM;
	my_data_pat_num[1] = PAT_JIKI_C;


/* 自弾座標の初期化 */
/*	for(i=0; i<10; i++){
		shot_data[i].x = 0 << SHIFT_NUM;
		shot_data[i].xx = 0;
		shot_data[i].y = SPR_DEL_Y;
		shot_data[i].yy = 0;
		shot_data[i].pat_num = PAT_MYSHOT1;
	}*/


/* 敵座標の初期化 */
/* 全スプライト表示数最大180とすると4枚かさねでせいぜい30程度 */
/*	for(i=0; i<TEKI_NUM_MAX; i++){
		teki_data[i].x = (rand() % SCREEN_MAX_Y) << SHIFT_NUM;
		teki_data[i].xx = 0;
		teki_data[i].y = (rand() % SCREEN_MAX_X) << SHIFT_NUM;
		teki_data[i].yy = ((rand() & 7) + 2) << SHIFT_NUM;
		teki_data[i].pat_num = PAT_TEKI1;
	}
*/
}


//#endif
