/* ゲ－ム本体 for FM TOWNS */

#define MAIN
//#define TOWNS

#include <stdio.h>
#include <stdlib.h>
#include <dos.h>
#include <spr.h>
//#include <FMCFRB.H>
#include <snd.h>
//#include <msvdrv.h>
#include <TOWNS/segment.h>

#include "subfunc.h"
#include "sg_init.h"

enum {
	BGMMAX = 2,
	SEMAX = 4
};

void put_strings(int scr, int x, int y,  char *str, char pal);
void put_numd(long j, char digit);
//char str_temp[9];
unsigned int encode;

extern void int_vsync_ent(void);
extern void init_vsync_ent(void);
extern void reset_vsync_ent(void);

volatile int vsync_flag = 0;
//int VECTOR_ADRV;
//short
//int VECTOR_SEGV;
//int VECTOR_REAV;
//int saveIMR_S;

//short
//int datasegment;

#define _disable() asm("cli\n")
#define _enable() asm("sti\n")

//#define DEBUG

#ifdef DEBUG
void //__attribute__((interrupt))
int_vsync(void)
{
	vsync_flag = 1;

	outportb(0x05ca, 0);

	outportb(0x6c, 0); // 1μ秒ウェイト
	outportb(0x10, 0x20);	/* EOI(Slave) */

	outportb(0x6c, 0); // 1μ秒ウェイト
	outportb(0x0, 0x20);	/* EOI(Master) */
}
#endif

int init_vsync(void)
{
	int ret;
	vsync_flag = 0;

	init_vsync_ent();

//	stackAddress = stack+1000;

//	outportb(0x04ea, 0xff);
//	outportb(0x22,0x40);
	outportb(0x5ca,0);
	return 0;
}

void reset_vsync(void)
{
	reset_vsync_ent();
}

//#endif

int spr_count,old_count;

/* スプライト表示デ－タをVRAMに書き込むマクロ */
/* 座標をシフトして書き込みデ－タは随時インクリメント */
/*パレット＋カラーコード＋反転フラグ */
/* BGとのプライオリティ設定 */
#define PUT_SP( x, y, no, atr) {\
	_poke_word(0x130, spram, x); \
	spram += 2; \
	_poke_word(0x130, spram, y); \
	spram += 2; \
	_poke_word(0x130, spram, no); \
	spram += 2; \
	_poke_word(0x130, spram, atr); \
	spram += 2; \
}
/*	*(spram++) = x; \
	*(spram++) = y; \
	*(spram++) = no; \
	*(spram++) = atr; \
}*/

void put_my_hp_dmg(void);
void set_object(void);
void do_putmessage(void);
void do_put_stage(unsigned char no);

/******************************************************************************/
#include "sg_com.h"
/******************************************************************************/

void put_my_hp_dmg(void)
{
	int i, j = 0;
	char str_temp[11];

	if(my_hp < HP_MAX)
		for(i = my_hp; i < HP_MAX; i++)
			str_temp[j++] = ' ';
	for(i = 0; i < my_hp; i++)
		str_temp[j++] = '`';
	str_temp[j] = '\0';

	put_strings(SCREEN2, 0, 29, "LIFE", CHRPAL_NO);
	put_strings(SCREEN2, 13-8, 29, str_temp, CHRPAL_NO);

	my_hp_flag = TRUE;
}


short test_h_f = TRUE;
short soundflag = FALSE;

unsigned char message_count = 0;

void put_strings(int scr, int x, int y,  char *str, char pal)
{
	char chr;
	unsigned short i = 0;

	while((chr = *(str++)) != '\0'){
		if(spr_count >= MAX_SPRITE)
			break;
//		if((chr < 0x30) || (chr > 0x5f))
//			chr = 0x40;
//		DEF_SP_SINGLE(spr_count, (x + (i++)) * 8 + 16, y * 8 + 16, (chr - '0' + CHR_TOP + 256), CHRPAL_NO, 0);
		if((chr >= 0x30)){ // && (chr <= 0x5f)){
			chr_data[spr_count].x = (x + i) * 8 + SPR_OFS_X * 0; \
			chr_data[spr_count].y = (y) * 8 + SPR_OFS_Y * 0 + 2; \
			chr_data[spr_count].pat_num = (chr - '0' + CHR_TOP); \
			chr_data[spr_count].atr = (CHRPAL_NO + 256) | 0x8000; \
			spr_count++; \
		}
		i++;
	}
	message_count = spr_count;
}


void put_numd(long j, char digit)
{
	char i = digit;

	while(i--){
		str_temp[i] = j % 10 + 0x30;
		j /= 10;
	}
	str_temp[digit] = '\0';
}

void score_display(void)
{
	put_numd(score, 7);
	put_strings(SCREEN2, 15-8, 0, str_temp, CHRPAL_NO);
	if((score == SCORE_MAX) || ((score >= hiscore) && ((score % 10) == 0))){
		hiscore = score;
		put_strings(SCREEN2, 0, 0, "HIGH", CHRPAL_NO);
	}
	else
		put_strings(SCREEN2, 0, 0, "SCORE", CHRPAL_NO);
}

void score_displayall(void)
{
//	if(score < hiscore)
//		put_strings(SCREEN2, 8, 22, "SCORE", CHRPAL_NO);
	score_display();
}

void hiscore_display(void)
{
	if(score > hiscore)
		if((score % 10) == 0)
			hiscore = score;

	put_numd(hiscore, 7);

	put_strings(SCREEN2, 9, 12, "HIGH", CHRPAL_NO);
	put_strings(SCREEN2, 9 + 5, 12, str_temp, CHRPAL_NO);
}

//#include "inkey.h"

unsigned char keyscan(void)
{
	unsigned char st, pd;
	unsigned char k5, k6, k7, k8, k9, ka;
	unsigned char paddata;
	static char matrix[16];
	unsigned char keycode = 0;

	_disable();
	KYB_matrix(matrix);
	_enable();

	k5 = matrix[5];
	k6 = matrix[6];
	k7 = matrix[7];
	k8 = matrix[8];
	k9 = matrix[9];
	ka = matrix[0xa];

	paddata = inportb(0x4d0 + 0 * 2); 
	st = (paddata & 0x0f);
	pd = (paddata >> 4) & 0x03;

	if((k5 & 0x04) || (k6 & 0x20) || !(pd & 0x01)) /* Z,SPACE */
		keycode |= KEY_A;
	if((k5 & 0x08) || !(pd & 0x02)) /* X */
		keycode |= KEY_B;
	if((k7 & 0x08) || (k9 & 0x20) || !(st & 0x01)) /* 8 */
		keycode |= KEY_UP1;
	if((k8 & 0x08) || (ka & 0x01) || !(st & 0x02)) /* 2 */
		keycode |= KEY_DOWN1;

	if(!(st & 0x0c)){ /* RL */
		keycode |= KEY_START;
	}else{
		if((k7 & 0x40) || (k9 & 0x80) || !(st & 0x04)) /* 4 */
			keycode |= KEY_LEFT1;
		if((k8 & 0x01) || (ka & 0x02) || !(st & 0x08)) /* 6 */
			keycode |= KEY_RIGHT1;
	}

	return keycode;
}

int opening_demo(void)
{
	signed int i, j;

	spr_count = 0;
	put_strings(SCREEN2, 17, 11, "PROJECT CC", CHRPAL_NO);
	put_strings(SCREEN2, 15, 11, "SINCE 199X", CHRPAL_NO);
	wait_sprite();
	set_sprite();
	fadeinblack(org_pal, CHRPAL_NO, 6);
	j = 4;
	for(i = 0; i < 75 / 2 * 3; i++){
		if(keyscan() || (!(KYB_read( 1, &encode ) != 0x1b))){
			j = 2;
			break;
		}
		sys_wait(1);
		wait_sprite();
		set_sprite();
	}
	fadeoutblack(org_pal, CHRPAL_NO, j);

	return NOERROR;
}

void put_titlelogo(short x, short y)
{
	unsigned char i, j, num = FONTPARTS + CHR_TOP;
	for(j = 0; j < 4; ++j){
		for(i = 0; i < 8; ++i){
			chr_data[spr_count].x = x + i * 16 + SPR_OFS_X; \
			chr_data[spr_count].y = y + j * 16 + SPR_OFS_Y + 2; \
			chr_data[spr_count].pat_num = (num++); \
			chr_data[spr_count].atr = (CHRPAL_NO + 256) | 0x8000; \
			spr_count++; \
		}
	}
}

void put_title(void)
{
	score_displayall();
	hiscore_display();
	put_strings(SCREEN2, 10, 14, "START", CHRPAL_NO);
	put_strings(SCREEN2, 8, 14, "EXIT", CHRPAL_NO);
	put_strings(SCREEN2, 5, 10, "      ij k   ", CHRPAL_NO);
	put_strings(SCREEN2, 4, 10, "a2022 bcdefgh", CHRPAL_NO);
}

int title_demo(void)
{
	int j, keycode;
	unsigned char x = 0;
	int loopcounter = 0;
	unsigned int soundtestno = 0;
	int soundtest = FALSE;

//	init_star();

	/* Set Title-Logo Pattern */
	/* Opening Start */
	j = -16 * 8;
	x = 1;
	spr_count = 0; //old_count = 0;
	put_titlelogo(128-48, 48);
	wait_sprite();
	set_sprite();
	wait_vsync();
	bg_roll();
	do{
		if(j < 0){
			j++;
//			spr_count = 0;
//			wait_vsync2();
			wait_sprite();
			set_sprite();
			wait_vsync();
			set_constrast(j / 8, org_pal, CHRPAL_NO);
			bg_roll();
//			if(!j){
//				put_title();

//				while(keyscan());
//			}
		}

		if(!j){
			spr_count = 0;
			put_titlelogo(128-48, 48);
			put_title();
			put_strings(SCREEN2, 8 + x * 2, 11, "?", CHRPAL_NO);

			if(soundtest == TRUE){
				put_strings(SCREEN2, 3, 2, "SOUND TEST", CHRPAL_NO);
				put_numd(soundtestno, 3);
				put_strings(SCREEN2, 3, 13, str_temp, CHRPAL_NO);
			}else{
//				put_strings(SCREEN2, 3, 2, "              ", CHRPAL_NO);
			}
			wait_sprite();
			bg_roll();
			set_sprite();
			wait_vsync();
//			bg_roll();

			keycode = keyscan();
			if(keycode)
				loopcounter = 0;
			if((keycode & KEY_DOWN1) && (x != 0)){
//				put_strings(SCREEN2, 8 + x * 2, 11, " ", CHRPAL_NO);
				x = 0;
			}
			if((keycode & KEY_UP1) && (x != 1)){
//				put_strings(SCREEN2, 8 + x * 2, 11, " ", CHRPAL_NO);
				x = 1;
			}
//			if(keycode & KEY_START)
//				return SYSEXIT;
			if(keycode & KEY_B){
				if(keycode & KEY_A){
					if(soundtest == FALSE){
						soundtest = TRUE;
					}else{
						soundtest = FALSE;
					}
				}else{
//					if(!MSV_stat_flag())
//						MSV_play_start();

//					SND_pcm_play_stop(71) ;
//					SND_pcm_play( 71, 60, 127, SNDBUFF );
//					if(soundtestno < BGMMAX){
//						playbgm(soundtestno, debugmode);
//					}else{
//						DI;
//						if(soundflag == TRUE)
//							if(se_check())
//								se_stop();
//						S_IL_FUNC(se_play(sndtable[0], soundtestno - BGMMAX));
//						EI;
//					}
				}
			}
			else if(keycode & (KEY_A)){ // | KEY_B)){ //UP1 | KEY_DOWN1 | KEY_RIGHT1 | KEY_LEFT1 | KEY_START)){
				if(!x)
					return SYSEXIT;
				if(keyscan() & KEY_LEFT1)
					return ERRLV2;
				if(keyscan() & KEY_RIGHT1)
					return ERRLV3;
				return ERRLV1;// + (1 - x);
			}
			if(keycode & (KEY_LEFT1 | KEY_RIGHT1)){
				if(soundtest == TRUE){
					if(keycode & KEY_DOWN2){
						++soundtestno;
					}else{
						--soundtestno;
					}
					soundtestno += (BGMMAX + SEMAX);
					soundtestno %= (BGMMAX + SEMAX);
				}
			}
		}
//		wait_vsync2();
		if(++loopcounter == WAIT1S * 30){
//			S_IL_FUNC(bgm_fadeout());
			return NOERROR;
		}
	}while(KYB_read( 1, &encode ) != 0x1b);
	return SYSEXIT;
}

void set_object(void)
{
	short i=0, j=0;

	DEF_SP(spr_count, my_data_x[i], my_data_y[i], my_data_pat_num[i], mypal, 0); /* 0x14); */
	SEARCH_LIST2(MAX_MYSHOT, i, j, myshot_next){
		DEF_SP(spr_count, myshot_x[i], myshot_y[i], myshot_pat_num[i],  CHRPAL_NO, 0);
	}
	SEARCH_LIST2(MAX_TEKI, i, j, teki_next){
		DEF_SP(spr_count, teki_x[i], teki_y[i], teki_pat[i], teki_pal[i], 0);
	}
	SEARCH_LIST2(MAX_TKSHOT, i, j, tkshot_next){
		DEF_SP(spr_count, tkshot_x[i], tkshot_y[i], tkshot_pat[i], CHRPAL_NO, 0);
	}

/*	for(i = 0; i < 16; ++i)
		DEF_SP_SINGLE(spr_count, (i * 16 - SPR_OFS_X) << SHIFT_NUM, (18 * 8 - SPR_OFS_Y) << SHIFT_NUM, 	IMG_SET(8, 0),  CHRPAL_NO, 0);*/
}



void init_star(void)
{
	int i;

	return;

/* スタ－の座標系を初期化 */
	for(i = 0;i < STAR_NUM; i++){
		star[0][i] = (rand() % 256) * 2;
		star[1][i] = ((((i + 1) * 256) / STAR_NUM) + 32);
		star[2][i] = ((rand() % 2) + 1);
//		_FP_OFF(vram) = ((star[0][i] + star[1][i] * 1024) / 2);
		vram = ((star[0][i] + star[1][i] * 512) / 1);
//		star[3][i] = VRAM_getPixelW(vram); //*vram;
		star[4][i] = rand() % 14 + 2;//% 14 + 2; //fff;
	}

	return;

/* スタ－の表示(固定表示) */
	i = STAR_NUM;
	while(i--){
//		_FP_OFF(vram) = (((star[0][i] + (256 / STAR_NUM)) % 256 + 32 + star[1][i] * 1024) / 2);
		vram = (((star[0][i] + (256 / STAR_NUM)) % 256 + 32 + star[1][i] * 1024) / 2);
//		*vram |= star[4][i];
		VRAM_putPixelW(vram, VRAM_getPixelW(vram) | star[4][i]);
	}

}

void bg_roll(void)
{
	int i;

	return;

/* スクロ－ルレジスタ制御 */
/*	outportb(0x440,17);
 	outportb(0x442,scrl * 128 % 256);
	outportb(0x443,scrl * 128 / 256);
	scrl += 512 - (scrl_spd >> SCRL_SFT);
	scrl %= 512;
*/
/* スクロ－ルするスタ－ */

	i = STAR_NUM;
	while(i--){
//		_FP_OFF(vram) = ((star[0][i] + star[1][i] * 1024) / 2);
		vram = ((star[0][i] + star[1][i] * 512) / 1);
//		*vram = star[3][i];
		VRAM_putPixelW(vram, 0); //star[3][i]);
//		vram = ((star[0][i] + star[1][i] * 512) / 1);
//		star[3][i] = VRAM_getPixelW(vram);
		star[0][i] += (star[2][i] + 512);
		star[0][i] %= 256;
		star[1][i] += (star[2][i] * 2 + 512);
		star[1][i] %= 256;
	}
	i = STAR_NUM;
	while(i--){
//		_FP_OFF(vram) = ((star[0][i] + star[1][i] * 1024) / 2);
		vram = ((star[0][i] + star[1][i] * 512) / 1);
//		star[3][i] = *vram;
//		star[3][i] = VRAM_getPixelW(vram);
//		*vram |= star[4][i];
		VRAM_putPixelW(vram, VRAM_getPixelW(vram) | star[4][i]);
	}
}


void do_putmessage(void)
{
	put_strings(SCREEN2, put_x, put_y, put_strdata, CHRPAL_NO);
}

unsigned char stage_chr[4][2] = {
	{9, 0},
	{10, 0},
	{8, 0},
	{8, 1},
};
void do_put_stage(unsigned char no)
{
	no %= 4;
	paint2( FONTPARTS + TITLEPARTS + stage_chr[no][0] + stage_chr[no][1] * 16);
}

/* ゲ－ム本体の処理 */
short game_run(short mode){
	short i;
//	unsigned char keycode;

//	init_star();

	my_hp_flag == TRUE;
	game_init();	/* 変数(広域)初期化 */

	switch(mode){
		case ERRLV2:
			stage = 3;
			my_hp = max_my_hp = 10;
			tkshot_c /= 6;
			score = 1;
//			renshaflag = TRUE;
			break;

		case ERRLV3:
			score = 2;
			my_hp = max_my_hp = 10;
//			renshaflag = TRUE;
//			renshaflag = FALSE;
			break;
	}

//	put_strings(SCREEN2, -3, 2, "SHIELD", CHRPAL_NO);


	spr_count = 0; //old_count = 0;
	do{
		if(mypal_dmgtime){
			--mypal_dmgtime;
			if(!mypal_dmgtime){
				mypal = CHRPAL_NO;
				if(!my_hp){
					mypal = REVPAL_NO;
/*					timeup = 60 * 10;
					scrlspd = 0;

					do{
						spr_count = 0;
						put_strings(SCREEN2, 14, 10, "CONTINUE A", CHRPAL_NO);
						if(timeup != 60 * 10){
							put_numd((long)(timeup / 60), 2);
							put_strings(SCREEN2, 10, 14, str_temp, CHRPAL_NO);
						}
						if(!(--timeup)){
*/							spr_count = 0;
//							put_strings(SCREEN2, 14, 10, "           ", CHRPAL_NO);
							put_strings(SCREEN2, 11, 12, " GAME OVER ", CHRPAL_NO);
//							put_strings(SCREEN2, 10, 14, "  ", CHRPAL_NO);
							scrlspd = 0; //SPR_DIV / 4;
							score_displayall();
							if(combo){
								combo_display();
							}
							put_my_hp_dmg();
							set_object();
							wait_sprite();
							set_sprite();
							for(i = 0; i < 60 ; i++){
								wait_sprite();
								set_sprite();
								wait_vsync();
								bg_roll();
							}
							return ERRLV1;
/*						}
						keycode =  keyscan();
						if(keycode & KEY_B){
							if((timeup -= 5) < 1)
								timeup = 1;
						}
						score_displayall();
						put_my_hp_dmg();
						set_object();
						wait_sprite();
						set_sprite();
						wait_vsync();

						if(timeup == (60*10-1)){
							while(keyscan() & (KEY_A | KEY_START)){
			f					wait_sprite();
								set_sprite();
								wait_vsync();
							}
						}
					}while(!(keyscan() & (KEY_START | KEY_A)));

//					put_strings(SCREEN2, 14, 10, "           ", CHRPAL_NO);
//					put_strings(SCREEN2, 10, 14, "  ", CHRPAL_NO);
					scrlspd = 0; //SPR_DIV / 4;
					score %= 10;
					if(score != 9){
						++score;
					}
					scrdspflag =TRUE;
					my_hp = max_my_hp;
					put_my_hp();
					mypal = REVPAL_NO;
					mypal_dmgtime = DMGTIME * 4;
					noshotdmg_flag = TRUE;
*/				}
			}
		}
		spr_count = 0;

		if(scrdspflag == TRUE){
			if(score > SCORE_MAX)
				hiscore = score = SCORE_MAX;
			score_displayall();
//			scrdspflag = FALSE;
/*			if(score >= hiscore){
				if((score % 10) == 0){
					hiscore = score;
				}
				put_strings(SCREEN2, 28, 0, "HI", CHRPAL_NO);
			}else
				put_strings(SCREEN2, 28, 0, "  ", CHRPAL_NO);
*/		}
		if(my_hp_flag == TRUE){
			put_my_hp_dmg();
		}
		do_putmessage();

		keycode = keyscan();

		switch(game_loop()){
			case SYSEXIT:
				return NOERROR;
			case NOERROR:
				wait_sprite();
				set_sprite();
				wait_vsync();
				bg_roll();
				break;
			default:
				continue;
		}
	}while((scrl_spd != 0) && (KYB_read( 1, &encode ) != 0x1b));

/* 終了処理 */
/* 	spr_off(); */
	return SYSEXIT;
}

void wait_vsync(void)
{
	while(!vsync_flag);
	vsync_flag = 0;
	/* VSYNC(=1)待ち */
//	do{
//		outportb(0x440, 30);
//	}while((inportb(0x0443) & 0x04)); /* 動作中 */
//	do{
//		outportb(0x440, 30);
//	}while(!(inportb(0x0443) & 0x04)); /* 動作中 */
}

void wait_vsync2(void)
{
	while(!vsync_flag);
	vsync_flag = 0;
	/* VSYNC(=1)待ち */
//	do{
//		outportb(0x440, 30);
//	}while((inportb(0x0443) & 0x04)); /* 動作中 */
//	do{
//		outportb(0x440, 30);
//	}while(!(inportb(0x0443) & 0x04)); /* 動作中 */
}

void wait_sprite(void)
{
/* スプライト動作チェック(BUSY=1) */
/* VSYNC割り込みしない場合は2回見ないと誤判断する可能性があった */
//	while(!(inportb(0x044c) & 0x02)); /* 動作中 */
	while((inportb(0x044c) & 0x02)); /* 動作中 */
//	while(!(inportb(0x044c) & 0x02)); /* 動作中 */

/*	_outportb(0x450, 1);	// スプライトコントローラーを切るテスト
	_outportb(0x452, 0x7f);*/
}

void set_sprite(void)
{
	int i, j;
	int pchr_data2;

/* スプライト表示 */
/* EGB/TBIOSは使わずスプライトRAMに直接書き込む */
/* 	SPR_display(2, 0); */
/* SPRAM先頭アドレスをスプライト表示最大数から算出 */
/* 最大数が可変の時はどうなる? */
//	_FP_OFF(spram) = (1024 - MAX_SPRITE) * 8;
//	spram = (1024 - MAX_SPRITE) * 8;

/* 表示数ぶん書き込む */
	if(spr_count > MAX_SPRITE){
		spram = (1024 - MAX_SPRITE) * 8;
		_disable();
		if(total_count & 1){
			pchr_data2 = (int)&chr_data[spr_count - 1];
			for(i = spr_count - 1, j = 0; j < (MAX_SPRITE - message_count); i--, j++){
//				PUT_SP(chr_data[i].x, chr_data[i].y, chr_data[i].pat_num, chr_data[i].atr);
				_memcpyfar( LSEG_CODE_READ_WRITE, pchr_data2, GSEG_SPRITE_PATTERN, spram, 8);
				pchr_data2 -= 8;
				spram += 8;
			}
			pchr_data2 = (int)&chr_data[message_count - 1];
			for(i = message_count - 1; i >= 0; i--){
//				PUT_SP(chr_data[i].x, chr_data[i].y, chr_data[i].pat_num, chr_data[i].atr);
				_memcpyfar( LSEG_CODE_READ_WRITE, pchr_data2, GSEG_SPRITE_PATTERN, spram, 8);
				pchr_data2 -= 8;
				spram += 8;
			}

		}else{
			pchr_data2 = (int)&chr_data[MAX_SPRITE - 1];
			for(i = MAX_SPRITE - 1; i >= 0; i--){
//				PUT_SP(chr_data[i].x, chr_data[i].y, chr_data[i].pat_num, chr_data[i].atr);
				_memcpyfar( LSEG_CODE_READ_WRITE, pchr_data2, GSEG_SPRITE_PATTERN, spram, 8);
				pchr_data2 -= 8;
				spram += 8;
			}
		}
		spr_on(MAX_SPRITE);
		_enable();
		old_count = MAX_SPRITE;
	}else{
		spram = (1024 - spr_count) * 8;
		pchr_data2 = (int)&chr_data[spr_count - 1];
		_disable();
		for(i = spr_count - 1; i >= 0; i--){
//			PUT_SP(chr_data[i].x, chr_data[i].y, chr_data[i].pat_num, chr_data[i].atr);
			_memcpyfar( LSEG_CODE_READ_WRITE, pchr_data2, GSEG_SPRITE_PATTERN, spram, 8);
			pchr_data2 -= 8;
			spram += 8;
		}
		spr_on(spr_count);
		_enable();

/* スプライトの表示数が減った場合､減った分を画面外に消去する */
/* 増える分には問題ない */
/*		if (old_count > spr_count){
			for(i = 0;i < (old_count - spr_count); i++){
				PUT_SP(0,(SCREEN_MAX_Y + 2),0,0x2000);
			}
		}*/
/* このフレ－ムで表示したスプライトの数を保存 */
		old_count = spr_count;
	}


	++total_count;

	if(seflag){
//		if(soundflag == TRUE)
//			if(se_check())
//				se_stop();
//		S_IL_FUNC(se_play(sndtable[0], seflag - 1));	/* 効果音 */
		SND_pcm_play_stop(71) ;
		SND_pcm_play( 71, 60, 127, &SNDBUFF[seflag - 1][0] );

//		MSV_partstop(13);
//		MSV_partplay(13 , msv_se[seflag - 1]);
		seflag = 0;
	}

}

/*void init_chr_data(void)
{
	int i;
	spr_count = 0;
	old_count = MAX_SPRITE;

	for(i = 0;i < MAX_SPRITE; i++){
		chr_data[i].x = 0;
		chr_data[i].y = 0;
		chr_data[i].pat_num = 0;
		chr_data[i].atr = 0;
	}
}*/
