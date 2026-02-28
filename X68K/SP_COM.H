
#include <stdio.h>
#include <stdlib.h>
#include <time.h>


#include "sp.h"
#include "subfunc.h"
#include "sp_main.h"


#include "list.h"

/*#include "sp_ene.h"*/
#include "sp_sht.h"
#include "sp_ene.h"

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

int scrlspd = 0;
long score = 0, hiscore = INIT_HSC;

#define SCREEN2 0

typedef void (*SUBFUNC)(int i); 


#define DMGTIME 3
#define SHOTCOUNT 32
#define URAMAX 4

#define SCORE_MAX 99999999L
#define STAGEMAX 99


/* スプライト表示デ－タをVRAMに書き込むマクロ */
/* 座標をシフトして書き込みデ－タは随時インクリメント */
/*パレット＋カラーコード＋反転フラグ */
/* BGとのプライオリティ設定 */
#define PUT_SP( x, y, no, atr) {\
	*(spdata++) = x; \
	*(spdata++) = y; \
	*(spdata++) = no; \
	*(spdata++) = atr; \
}

CHR_PARA teki_data[MAX_TEKI];		/* 敵デ－タ */


/*CHR_PARA shot_data[30];*/		/* 自機の弾デ－タ */

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
CHR_PARA my_data[2];		/* 自機デ－タ */

/* 自機弾情報 */
/*CHR_OBJ*/
CHR_PARA myshot[MAX_MYSHOT + 2];				/* 自機弾座標管理 */
CHR_PARA2 myshot2[MAX_MYSHOT + 2];				/* 自機弾座標管理2 */
unsigned char myshot_next[MAX_MYSHOT + 2];
unsigned char myshot_free[MAX_MYSHOT + 2];

int mypal;
int mypal_dmgtime;
int my_movecount;
int my_hp;
int max_my_hp;

/* 敵情報 */
SPR teki[MAX_TEKI];					/* 敵座標管理 */
unsigned int tkcount[MAX_TEKI];		/* 敵移動用カウンタ */
unsigned char teki_pat[MAX_TEKI];			/* 敵パターン番号 */
int teki_dir[MAX_TEKI];				/* 敵移動方向保存用変数 */

unsigned char teki_next[MAX_TEKI + 2];			/* 敵表示フラグ */
unsigned char teki_free[MAX_TEKI + 2];			/* 敵表示フラグ */

unsigned int tkshotcount[MAX_TEKI];	/* 敵弾発射カウンタ */

unsigned char teki_hp[MAX_TEKI];
int teki_pal[MAX_TEKI];
int teki_dmgtime[MAX_TEKI];

int teki_move[MAX_TEKI];
int teki_shotinfo[MAX_TEKI];

int teki_num = 0;
int stage;
int tmp;
int timeup;
unsigned char trgcount = 0;	/* ショット間隔リミッタ */
unsigned char trgcount2 = 0;	/* 連射リミッタ */
int trgnum = 0;
int total_count = 0;
int renshaflag = TRUE; /*FALSE;*/

/* 敵弾情報 */
SPR tkshot[MAX_TKSHOT];				/* 敵弾座標管理 */

unsigned char tkshot_next[MAX_TKSHOT + 1];
unsigned char tkshot_free[MAX_TKSHOT + 1];

int tkshot_xx[MAX_TKSHOT];			/* 敵弾移動増分X */
int tkshot_yy[MAX_TKSHOT];			/* 敵弾移動増分Y */

unsigned char tkshot_pat[MAX_TKSHOT];

int tkshotnum;						/* 敵弾 現在の表示数 */
int tkshot_c;						/* 敵弾発射間隔 初期値 */

int scrdspflag;						/* スコア文字更新フラグ */


/*SPR_COMB spr[MAX_SPRITE * 3];*/
/*const */

/* テンポラリ用グローバル変数 */
unsigned char chrnum;
int chrpal;
const SPR_COMB *sprcomb;

int tmp_x, tmp_y;
int tmp_xx, tmp_yy;

int spr_count; //,old_count;
int *px, *py;

COLIGION *p_coli;

/******************************************************************************/

/* 敵弾消去マクロ */
//	tkshot[NO].y = tmp_y = SPR_DEL_Y;\

#define del_tkshot(NO) {\
	tkshotnum--;\
}

#define put_my_hp() my_hp_flag = TRUE;

void put_my_hp_dmg(void)
{
	unsigned char i, j = 0;

	if(my_hp < HP_MAX)
		for(i = my_hp; i < HP_MAX; i++)
			str_temp[j++] = ' ';
	for(i = 0; i < my_hp; i++)
		str_temp[j++] = '`';
	str_temp[j] = '\0';

	put_strings(SCREEN2, 13-8, 31, str_temp, CHRPAL_NO);

	my_hp_flag = TRUE;
}

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


/*****************************/
/* 敵機弾移動方向決定(32way) */
/*****************************/

int tekishot_dir(void)
{
	int l = 0;

	tmp_yy = my_data[0].x - tmp_x;
	tmp_xx = my_data[0].y - tmp_y + scrlspd;

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
void shottome(int dir, int pat)
{
	int j;

	if(tkshot_free[MAX_TKSHOT] != END_LIST){
		ADD_LIST(MAX_TKSHOT, j, tkshot_next, tkshot_free);

		if(uramode > 1){
			tkshot[j].x = tmp_x + (4 << SHIFT_NUM) + (tkshot_xx[j] = direction[dir][0] * 3 / 2);
			tkshot[j].y = tmp_y + (4 << SHIFT_NUM) + (tkshot_yy[j] = direction[dir][1] * 3 / 2);
		}else{
			tkshot[j].x = tmp_x + (4 << SHIFT_NUM) + (tkshot_xx[j] = direction[dir][0]);
			tkshot[j].y = tmp_y + (4 << SHIFT_NUM) + (tkshot_yy[j] = direction[dir][1]);
		}
		if(pat == PAT_TKSHOT1)
			if(tkshot_xx[j] > 0)
				pat = PAT_TKSHOT3;
		tkshot_pat[j] = pat;
		tkshotnum++;
	}
}

void shottome2(int dir, int pat)
{
	int j;

	if(tkshot_free[MAX_TKSHOT] != END_LIST){
		ADD_LIST(MAX_TKSHOT, j, tkshot_next, tkshot_free);
		tkshot[j].x = tmp_x + (4 << SHIFT_NUM) + (tkshot_xx[j] = direction[dir][0] * 3 / 2);
		tkshot[j].y = tmp_y + (4 << SHIFT_NUM) + (tkshot_yy[j] = direction[dir][1] * 3 / 2);

		if(pat == PAT_TKSHOT1)
			if(tkshot_xx[j] > 0)
				pat = PAT_TKSHOT3;

		tkshot_pat[j] = pat;
		tkshotnum++;
	}

}


/**********************/
/* 敵機弾発射サブ関数 */
/**********************/
void shotcheck1(int i)	/* NORMAL */
{
	if(tkshotcount[i] != 0){
		tkshotcount[i]--;
		return;
	}

	tkshotcount[i] = tkshot_c;
	shottome(tekishot_dir(), PAT_TKSHOT1);
}

void shotcheck2(int i)	/* 3 WAY */
{
	int l;

	if(tkshotcount[i] != 0){
		tkshotcount[i]--;
		return;
	}

	tkshotcount[i] = tkshot_c;
	l = tekishot_dir();

	shottome(l, PAT_TKSHOT1);
	shottome((l + 2) % 32, PAT_TKSHOT1);
	shottome((l - 2 + 32) % 32, PAT_TKSHOT1);
}

void shotcheck3(int i)	/* RANDOM */
{
	if(tkshotcount[i] != 0){
		tkshotcount[i]--;
		return;
	}

	tkshotcount[i] = tkshot_c;
	shottome(rand() % 32, PAT_TKSHOT1);
}

void shotcheck4(int i)	/* LASER */
{
	if(tkshotcount[i] == 0){
		return;
	}
	if(tkshotcount[i]-- < (SHOTCOUNT - 1)){
		if(tkshotcount[i] & 2)
			shottome(DIR_DOWN, PAT_TKSHOT2);
	}
}

void dummy(int i)
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

void do_shotcheck(int i)
{
/*	SUBFUNC p;*/
	int j;

	j = teki_shotinfo[i];
	if(j < SHOTCHECKMAX)
		if(j >= 0)
			shotcheck_tbl[j](i);
}

/******************/
/* 敵機弾移動サブ */
/******************/
void move_tekishot(void)
{
	int i, j, k;

	SEARCH_LIST2(MAX_TKSHOT, i, j, tkshot_next){
		k = tkshot_pat[i];
		tmp_x = (tkshot[i].x += (tkshot_xx[i]));

		/* 敵機弾画面外消去 */
		if((unsigned short)(tmp_x) > ((SCREEN_MAX_X + 16) << SHIFT_NUM)){
			del_tkshot(i);
			tmp_y = tkshot[i].y;
			DEL_LIST(MAX_TKSHOT, i, j, tkshot_next, tkshot_free);
		}else{
			tmp_y = tkshot[i].y += (tkshot_yy[i] - scrlspd);
			if((unsigned short)(tmp_y) > ((SCREEN_MAX_Y + 16) << SHIFT_NUM)){
				del_tkshot(i);
				DEL_LIST(MAX_TKSHOT, i, j, tkshot_next, tkshot_free);

			}/* 敵機弾と自機の当たり判定→自機ダメージ */
			else{
			 if((tmp_xx = my_data[0].x - tmp_x) < ((TKD_OFS_X - MY_OFS_X + TKD_SIZE_X) << SHIFT_NUM)){
				if(tmp_xx > ((TKD_OFS_X - MY_OFS_X - MY_SIZE_X) << SHIFT_NUM)){
					if((tmp_yy = my_data[0].y - tmp_y) < ((TKD_OFS_Y - MY_OFS_Y + TKD_SIZE_Y) << SHIFT_NUM)){
						if(tmp_yy > ((TKD_OFS_Y - MY_OFS_Y - MY_SIZE_Y) << SHIFT_NUM)){
							/* 自機ダメージ */
							del_tkshot(i);
							DEL_LIST(MAX_TKSHOT, i, j, tkshot_next, tkshot_free);
							my_dmg();
						}
					}
				}
			}
		DEF_SP(spr_count, tmp_x, tmp_y, k, CHRPAL_NO, PRO_TEKISHOT);

			}
		}
	}
}

void check_revshot(void)
{
	int l;

	if(uramode > 2){
		l = tekishot_dir();

		shottome2(l, PAT_TKSHOT1);
		shottome2((l + 2) % 32, PAT_TKSHOT1);
		shottome2((l - 2 + 32) % 32, PAT_TKSHOT1);
	}else
		shottome2(tekishot_dir(), PAT_TKSHOT1);
}

/***********************/
/* 敵爆発パターンその1 */
/***********************/
void move_teki_bomb(int i)
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
			teki_hp[i] = 0;
//			teki[i].y = SPR_DEL_Y;
			break;
	}

	tkcount[i]++;
}

/***********************/
/* 敵移動パターンその1 */
/* HENTAI SINWAVE      */
/***********************/
void move_teki_pat1(int i)
{
	tmp_x -= sinwave[tkcount[i] / SINRATE] * ((3 << SHIFT_NUM) / 2);

	if(++tkcount[i] >= (SINFREQ * SINRATE))
		tkcount[i] = 0;

	tmp_x += sinwave[tkcount[i] / SINRATE] * ((3 << SHIFT_NUM) / 2);

	/* 敵機縦方向移動処理 */
	if(!(tmp_y > (128 << SHIFT_NUM)))
		tmp_y += ((3 << SHIFT_NUM) / 2);	/* ゆっくり */
	else
		tmp_y += (2 << SHIFT_NUM);	/* 高速 */

	/* 敵機画面外消去(下方向) */
	if(tmp_y > (SPR_MAX_Y)){
		teki_hp[i] = 0;
	}
}

/***********************/
/* 敵移動パターンその2 */
/* FIRE2               */
/***********************/
void move_teki_pat2(int i)
{
	int dir;

	if(++tkcount[i] < 800){
	if(tmp_x < (16 << SHIFT_NUM))
		teki_dir[i] = tekishot_dir();

	if(tmp_y < (16 << SHIFT_NUM))
		teki_dir[i] = tekishot_dir();

	if(tmp_x > (SCREEN_MAX_X << SHIFT_NUM))
		teki_dir[i] = tekishot_dir();

	if(tmp_y > (SCREEN_MAX_Y << SHIFT_NUM))
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
void move_teki_pat3(int i)
{
	int dir, dir2;

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
void move_teki_pat4(int i)
{
	int dir;

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
		if(tmp_y < (16 << SHIFT_NUM))
			teki_dir[i] = tekishot_dir();
		if(tmp_x > (SCREEN_MAX_X << SHIFT_NUM))
			teki_dir[i] = tekishot_dir();
		if(tmp_y > (SCREEN_MAX_Y << SHIFT_NUM))
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
void move_teki_pat5(int i)
{
	int dir;

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
void move_teki_pat6(int i)
{
	int dir;

	dir = teki_dir[i];
	if(tkcount[i] == 120){
		dir = tekishot_dir();
		teki_shotinfo[i] = 0;
	}else if(tkcount[i] == 40){
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
	int j;

	if(teki_free[MAX_TEKI] == END_LIST)
		return;

	ADD_LIST(MAX_TEKI, j, teki_next, teki_free);

	teki_pat[j] = pat;
	tkcount[j] = 0;
	tkshotcount[j] = SHOTCOUNT - 2;

	teki_move[j] = move;
	teki[j].y = teki[i].y;
	teki[j].x = teki[i].x;

	teki_hp[j] = 10;
	teki_pal[j] = CHRPAL_NO;
	teki_shotinfo[j] = shot;
	teki_dir[j] = dir;
	teki_num++;
}

/*************************/
/* ボス移動パターンその1 */
/*************************/
void move_boss_pat1(int i)
{
	int dir;
	dir = teki_dir[i];

	if(++tkcount[i] == 40){
		dir = DIR_RIGHT;
	}else if((tkcount[i] % 600) == 40){
		tkshotcount[i] = SHOTCOUNT * 3 / 2;
		dir = DIR_DOWN;
	}else if(tkcount[i] > 40){
		switch(dir){
			case DIR_UP:
				if(tmp_y < ((64-8) << SHIFT_NUM)){
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
				if(tmp_y > (192 << SHIFT_NUM)){
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

}
/************/

#define MOVETEKIMAX 10

SUBFUNC moveteki_tbl[MOVETEKIMAX] = 
{
	move_teki_bomb,
	move_teki_pat1,
	move_teki_pat2,
	move_teki_pat3,
	move_teki_pat4,
	move_teki_pat5,
	move_teki_pat6,
	dummy,
	dummy,
	move_boss_pat1,
};

/************/
/* 敵機移動 */
/************/
void move_teki(void)
{
	int i, j, k, l, j2;

	SEARCH_LIST2(MAX_TEKI, i, k, teki_next){

		/* 敵機移動処理 */
		tmp_x = teki[i].x;
		tmp_y = teki[i].y;

		l = teki_move[i];

		if(l >= 0)
			if(l < MOVETEKIMAX)
				moveteki_tbl[l](i);

		teki[i].x = tmp_x;
		teki[i].y = tmp_y;

		if(teki_hp[i] > 2){

			/* 敵機と自機弾の接触判定 → 敵機、自機弾消滅 スコア増加 */
			SEARCH_LIST2(MAX_MYSHOT, j, j2, myshot_next){
//				if((tmp_yy = myshot[j].y - tmp_x) < (16 << SHIFT_NUM)){
//					if(tmp_yy > ((TK_OFS_Y - MY_OFS_Y - 8) << SHIFT_NUM)){
				if((tmp_xx = myshot[j].x - tmp_x) < (16 << SHIFT_NUM)){
//					if(tmp_xx > (-20 << SHIFT_NUM)){
					if(tmp_xx > (-(4 << SHIFT_NUM))){
						if((tmp_yy = myshot[j].y - tmp_y) < (16 << SHIFT_NUM)){
//							if(tmp_yy > (-20 << SHIFT_NUM)){
							if(tmp_yy > (-(4 << SHIFT_NUM))){

								/* 敵ダメージ */
								teki_pal[i] = REVPAL_NO;
								teki_dmgtime[i] = DMGTIME;

								--teki_hp[i];

								if(uramode)
									shottome2(tekishot_dir(), PAT_TKSHOT1);

								/* ノーマル弾なら画面外に消去 */
//								myshot[j].y = 0; /*SPR_DEL_Y;*/
								/*myshot[j].yy = 0;*/
								trgnum--;
								DEL_LIST(MAX_MYSHOT, j, j2, myshot_next, myshot_free);

								score += 10;		/* 敵の点数を足す */
								scrdspflag = TRUE;
								break;
							}
						}
					}
				}
			}
		}

		if(teki_hp[i] > 2){
			/* 敵機と自機の接触判定 → 自機ダメージ */
			p_coli = &hantei[0];
			if((tmp_xx = my_data[0].x - tmp_x) < p_coli->x1){
				if(tmp_xx > p_coli->x2){
					if((tmp_yy = my_data[0].y - tmp_y) < p_coli->y1){
						if(tmp_yy > p_coli->y2){

							/* 自機ダメージ */
							my_dmg();
						}
					}
				}
			}

			do_shotcheck(i);
		}
		DEF_SP(spr_count, teki[i].x, teki[i].y, teki_pat[i], teki_pal[i], PRO_TEKI);

		/* 敵消滅 → 爆発パターンへ */
		if(!teki_hp[i]){
//			teki[i].y = SPR_DEL_Y;
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
	}
}

/**********/
/* 敵出現 */
/**********/
void com_tkappend(int teki_no)
{
	int i;

	if(teki_free[MAX_TEKI] == END_LIST)
		return;

	ADD_LIST(MAX_TEKI, i, teki_next, teki_free);

	teki_pat[i] = teki_no;
	tkcount[i] = 0;
	tkshotcount[i] = 16; //SHOTCOUNT - 2;

	teki_move[i] = command[schedule_ptr + 2];
	teki[i].x = (((command[schedule_ptr + 3] - 7) << SHIFT_NUM) * 256 / 144 );
	teki[i].y = (command[schedule_ptr + 4] << SHIFT_NUM);

	teki_hp[i] = command[schedule_ptr + 5];
	teki_pal[i] = CHRPAL_NO;

	teki_shotinfo[i] = command[schedule_ptr + 6];

	teki_dir[i] = DIR_DOWN;
	teki_num++;
}


/**************************************/

void com_waitcount(int i)
{
	waitcount = i;
}

void com_tkalldel(int i)
{
	waitcount = 1;
}

void com_bgmchange(int i)
{
/*	playbgm(i, 0);*/
}

void com_bgmfadeout(int i)
{
/*	S_IL_FUNC(bgm_fadeout());*/
}

void com_dummy(int i)
{
	waitcount = 1;
}

void com_putmessage(int i)
{
	put_strings(SCREEN2, command[schedule_ptr + 1], command[schedule_ptr + 2], (char *)message[command[schedule_ptr + 3]], CHRPAL_NO);
}

void com_putstage(int i)
{
	char str_x = command[schedule_ptr + 1];
	char str_y = command[schedule_ptr + 2];

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

void com_end(int i)
{
	waitcount = 1;
}

void com_se(int i)
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
					score += 10000;
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
				command_tbl[command_num](command[schedule_ptr + 1]);

	schedule_ptr += command_length[command_num];	/* 引数の数だけ先に進める */

	}
}

