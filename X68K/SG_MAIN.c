/* ゲ－ム本体 for X680x0 with XSP */

#include "common.h"

#define MAIN
#define X68K

#include <string.h>
#include <x68k/iocs.h>

#ifdef XSP

#include "XSP2lib.H"
//#include "PCM8Afnc.H"

#endif

#include "sg_init.h"
#include "play.h"

enum {
	BGMMAX = 2,
	SEMAX = 4
};


void wait_vsync(void);
void put_numd(long j, char digit);
//char str_temp[9];
void do_put_stage(unsigned char no);

unsigned char spr_page = 1;
volatile unsigned char spr_flag = 0, spr_next = 0;
//unsigned char 
//int tmp_spr_count = 0;
//int old_count[2];

unsigned short old_count[2];
volatile unsigned short tmp_spr_count = 0;
unsigned short spr_count = 0;

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

void put_my_hp_dmg(void);

/******************************************************************************/
#include "sg_com.h"
/******************************************************************************/

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

//unsigned char wake_count = 0;
//unsigned char muki = PAT_JIKI_C;

short test_h_f = TRUE;
short soundflag = FALSE;

void put_strings(int scr, int x, int y,  char *str, char pal)
{
	char chr;
	unsigned short i = 0;
	unsigned short *bgram;
	bgram = (unsigned short *)0xebc000; /* BG0 */
//	bgram = (unsigned short *)0xebe000; /* BG1 */
	bgram += (x * 2 + (y) * 0x80) / 2;

	while((chr = *(str++)) != '\0'){
		if((chr < 0x30)) //|| (chr > 0x5f))
			chr = PCG_SPACE;
		else
			chr -= '0';
		*(bgram++) = chr;
	}
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
	put_strings(SCREEN2, 15-8, 0 , str_temp, CHRPAL_NO);
	if((score == SCORE_MAX) || ((score >= hiscore) && ((score % 10) == 0))){
		hiscore = score;
		put_strings(SCREEN2, 0, 0, "HIGH ", CHRPAL_NO);
	}else
		put_strings(SCREEN2, 0, 0, "SCORE", CHRPAL_NO);
}

void score_displayall(void)
{
//	put_strings(SCREEN2, 9, 22, "SCORE", CHRPAL_NO);
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

#include "inkey.h"

unsigned char keyscan(void)
{
	unsigned char k5, k6, k7, k8, k9, st, pd;
	unsigned char *reg = (unsigned char *)0xe9a001;
	unsigned short paddata;
	unsigned char keycode = 0;

	k5 = _iocs_bitsns(5);
	k6 = _iocs_bitsns(6);
	k7 = _iocs_bitsns(7);
	k8 = _iocs_bitsns(8);
	k9 = _iocs_bitsns(9);

	paddata = reg[0];
	st = (paddata & 0x0f); // ^ 0x0f;
	pd = ((paddata >> 5) & 0x03); // ^ 0x03;

	if((k5 & 0x04) || (k6 & 0x20) || !(pd & 0x01)) /* Z,SPACE */
		keycode |= KEY_A;
	if((k5 & 0x08) || !(pd & 0x02)) /* X */
		keycode |= KEY_B;
	if((k8 & 0x10) || (k7 & 0x10) || !(st & 0x01)) /* 8 */
		keycode |= KEY_UP1;
	if((k9 & 0x10) || (k7 & 0x40) || !(st & 0x02)) /* 2 */
		keycode |= KEY_DOWN1;

	if(!(st & 0x0c)){ /* RL */
		keycode |= KEY_START;
	}else{
		if((k8 & 0x80) || (k7 & 0x08) || !(st & 0x04)) /* 4 */
			keycode |= KEY_LEFT1;
		if((k9 & 0x02) || (k7 & 0x20) || !(st & 0x08)) /* 6 */
			keycode |= KEY_RIGHT1;
	}

	return keycode;
}

int opening_demo(void)
{
	signed int i, j;

	put_strings(SCREEN2, 16, 11, "PROJECT CC", CHRPAL_NO);
	put_strings(SCREEN2, 14, 11, "SINCE 199X", CHRPAL_NO);
	fadeinblack(org_pal, CHRPAL_NO, 6);
	j = 4;
	for(i = 0; i < 75 / 2 * 3; i++){
		if(keyscan() || ((_iocs_bitsns(0) & 2))){
			j = 2;
			break;
		}
		sys_wait(1);
	}
	fadeoutblack(org_pal, CHRPAL_NO, j);

	return NOERROR;
}

void put_title(void)
{
	put_strings(SCREEN2, 9, 14, "START", CHRPAL_NO);
	put_strings(SCREEN2, 7, 14, "EXIT", CHRPAL_NO);
	put_strings(SCREEN2, 4, 10, "      ij k   ", CHRPAL_NO);
	put_strings(SCREEN2, 3, 10, "a2022 bcdefgh", CHRPAL_NO);
	score_displayall();
	hiscore_display();
}

int title_demo(void)
{
	int j; //, keycode;
	unsigned char x = 0;
	int loopcounter = 0;
	unsigned int soundtestno = 0;
	int soundtest = FALSE;

//	init_star();

	/* Set Title-Logo Pattern */
	/* Opening Start */
	j = -16 * 8;
	x = 1;
//	spr_count = old_count = 0;
	do{
		if(j < 0){
			j++;
			spr_count = 0;
			set_constrast(j / 8, org_pal, CHRPAL_NO);
			if(!j){
//				put_strings(SCREEN2, 8, 7, "NORMAL", CHRPAL_NO);
//				put_strings(SCREEN2, 6, 7, "HARD", CHRPAL_NO);

				put_title();

//				while(keyscan());
			}
		}

		if(!j){
			spr_count = 0;
			put_strings(SCREEN2, 7 + x * 2, 11, "?", CHRPAL_NO);
			keycode = keyscan();
			if(keycode)
				loopcounter = 0;
			if((keycode & KEY_DOWN1) && (x != 0)){
				put_strings(SCREEN2, 7 + x * 2, 11, " ", CHRPAL_NO);
				x = 0;
			}
			if((keycode & KEY_UP1) && (x != 1)){
				put_strings(SCREEN2, 7 + x * 2, 11, " ", CHRPAL_NO);
				x = 1;
			}
//			if(keycode & KEY_START)
//				return SYSEXIT;
			if(keycode & KEY_B){
				if(keycode & KEY_A){
					if(soundtest == FALSE){
						put_strings(SCREEN2, 2, 2, "SOUND TEST", CHRPAL_NO);
						put_numd(soundtestno, 3);
						put_strings(SCREEN2, 2, 13, str_temp, CHRPAL_NO);
						soundtest = TRUE;
					}else{
						put_strings(SCREEN2, 2, 2, "              ", CHRPAL_NO);
						soundtest = FALSE;
					}
				}else{
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
					put_numd(soundtestno, 3);
					put_strings(SCREEN2, 2, 13, str_temp, CHRPAL_NO);
				}
			}
		}
		wait_vsync();
		bg_roll();
		if(++loopcounter == WAIT1S * 30){
			return NOERROR;
		}
	}while(!(_iocs_bitsns(0) & 2));
	return SYSEXIT;
}


void init_star(void)
{
	int i;
/* スタ－の座標系を初期化 */
	for(i = 0;i < STAR_NUM; i++){
		star[0][i] = (i + 1) * (256 / STAR_NUM);
		star[1][i] = rand() % 512;
		star[2][i] = (rand() % 2) + 1;

		/* VRAMのアドレスを算出 */
		vram = (unsigned short *)0xc00000 + (star[0][i] + star[1][i]*512);
		star[3][i] = *vram;	/* 元の色を記憶する */

		star[4][i] = rand() % 14 + 2;
	}
/* スタ－の表示(固定表示) */
	i = STAR_NUM;
	while(i--){
		vram = (unsigned short *)0xc00000 + (star[0][i] + ((256 / STAR_NUM) / 2) + 
			(star[1][i] * 256)) * 2;
//		*vram |= star[4][i];
	}
}

void bg_roll(void)
{
	int i;
	register unsigned short *scroll_x = (unsigned short *)0xe80018;	/* GRP0 */
	register unsigned short *scroll_y = (unsigned short *)0xe8001a;	/* GRP0 */

/* スクロ－ルレジスタ制御 */

		scrl += 512 - (scrl_spd >> SCRL_SFT);
//		scrl--;
		scrl %= 512;

//		if(bg_mode){
			*scroll_x = scrl;
			*scroll_y = scrl;
//		}

//		return;

/* スクロ－ルするスタ－の計算 */

		i = STAR_NUM;
		while(i--){
			vram = (unsigned short *)0xc00000 + (star[0][i] + (star[1][i] * 512)) ;
			*vram = star[3][i];
			star[0][i] += (star[2][i] + 512);
			star[0][i] %= 512;
			star[1][i] += (star[2][i] + 512);
//			star[1][i]++;
			star[1][i] %= 512;
//		}
//		i = STAR_NUM;
//		while(i--){
			vram = (unsigned short *)0xc00000 + (star[0][i] + (star[1][i] * 512)) ;
			star[3][i] = *vram;
//			*vram |= star[4][i];
		}
}

unsigned char stage_chr[4] = {
	(0x18+1)*4,
	(0x18+2)*4,
	(0x18+0)*4,
	(0x18+16)*4,
};

void do_put_stage(unsigned char no)
{
	unsigned char x,y;
	unsigned short *bgram;

	no %= 4;

	for(y = 0; y < 32; y+=2){
//	for(y = 18; y < 20; ++y){
		x = 0;
		bgram = (unsigned short *)0xebe000;
		bgram += x + y * 64; /* BG1 */
		for(x = 0; x < 16; ++x){
			*(bgram++) = stage_chr[no]; //(0x18+1)*4;
			*(bgram++) = stage_chr[no]+2; //(0x18+1)*4;
		}
		bgram +=  32; /* BG1 */
		for(x = 0; x < 16; ++x){
			*(bgram++) = stage_chr[no]+1; //(0x18+1)*4;
			*(bgram++) = stage_chr[no]+3; //(0x18+1)*4;
		}
	}

}

/* ゲ－ム本体の処理 */
short game_run(short mode){
	short i;
//	unsigned char keycode;

//	init_star();

	my_hp_flag = TRUE;
	game_init();	/* 各変数の初期化 */

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

	put_strings(SCREEN2, 0, 31, "LIFE", CHRPAL_NO);

//	spr_count = old_count = 0;
/* ゲームのメインループ */
	do{
		if(mypal_dmgtime){
			--mypal_dmgtime;
			if(!mypal_dmgtime){
				mypal = CHRPAL_NO;
				if(!my_hp){
/*					timeup = 60 * 10 * 0 + 1;
					scrlspd = 0;
					put_strings(SCREEN2, 11, 12, "CONTINUE A", CHRPAL_NO);
					while(keyscan() & (KEY_A | KEY_START)){
						wait_vsync();
						set_sprite();
					}
//				do{
//						put_numd((long)(timeup / 60), 2);
//						put_strings(SCREEN2, 10, 14, str_temp, CHRPAL_NO);

//						if(!(--timeup)){
*/							put_strings(SCREEN2, 11, 12, "           ", CHRPAL_NO);
							put_strings(SCREEN2, 11, 12, " GAME OVER ", CHRPAL_NO);
							put_strings(SCREEN2, 15, 16, "  ", CHRPAL_NO);
							scrlspd = 0; //SPR_DIV / 4;
							for(i = 0; i < 60 ; i++){
								wait_vsync();
								set_sprite();
								bg_roll();
							}
							return ERRLV1;
/*						}
						keycode =  keyscan();
						if(keycode & KEY_B){
							if((timeup -= 5) < 1)
								timeup = 1;
						}
						wait_vsync();
						set_sprite();
					}while(!(keycode & (KEY_START | KEY_A)));

					put_strings(SCREEN2, 11, 12, "           ", CHRPAL_NO);
					put_strings(SCREEN2, 15, 16, "  ", CHRPAL_NO);
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

		if(scrdspflag == TRUE){
			if(score > SCORE_MAX){
				hiscore = score = SCORE_MAX;
			}
			score_display();
			scrdspflag = FALSE;
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
			my_hp_flag = FALSE;
		}

		keycode = keyscan();
//		tmp_spr_count = 0;
		spr_count = 0;
		switch(game_loop()){
			case SYSEXIT:
				return NOERROR;
			case NOERROR:
				wait_vsync();
				set_sprite();
				bg_roll();
				break;
			default:
				spr_count = 0;
				continue;
		}
	}while((scrl_spd != 0) && !(_iocs_bitsns(0) & 2));

/* 終了処理 */
/* 	spr_off(); */
	return SYSEXIT;
}

#ifdef XSP

void set_sprite_new(void);

void wait_vsync(void)
{
	/* 垂直同期 */
		xsp_vsync2(2);
		set_sprite_new();
}

#else

volatile unsigned char vsync_flag = 0;

void wait_vsync(void)
{

//	unsigned char volatile *vsync = (unsigned char *)0xe88001;	/* MFP */
/* VSYNC待ち */
//	while(!((*vsync) & 0x10));	/* 調査中 */
//	while((*vsync) & 0x10);		/* 調査中 */
	while(!vsync_flag);
	vsync_flag = 0;

//	set_sprite_new();
}

#endif

#ifdef XSP

void set_sprite(void)
{
}

void set_sprite_new(void)
{
	int i;

	for(i = 0; i < spr_count; i++)
		/* スプライトの表示登録 */
//		xsp_set(chr_data[i].x, chr_data[i].y, chr_data[i].pat_num,  chr_data[i].atr);//0x013F

		xsp_set_st(&chr_data[i]);
//		xsp_set(player.x, player.y, player.pt, player.info);
		/*
			↑ここは、
				xsp_set_st(&player);
			と記述すれば、より高速に実行できる。
		*/

	/* スプライトを一括表示する */
	xsp_out();

	if(seflag){
//		if(soundflag == TRUE)
//			if(se_check())
//				se_stop();
//		S_IL_FUNC(se_play(sndtable[0], seflag - 1));	/* 効果音 */
//		if(mcd_status >= 0){
//			pcm_play(&SNDBUFF[seflag - 1][0], pcmsize[seflag - 1]);
//		}
		_iocs_adpcmmod(0);
		_iocs_adpcmout(&SNDBUFF[seflag - 1][0], 4 * 256 + 3, pcmsize[seflag - 1]);
		seflag = 0;
	}
}

#else

void set_sprite(void)
{
	int i, j;

//	wait_vsync();

/* スプライト表示 */
	spdata = (unsigned short *)&chr_data2[spr_page]; //(unsigned short *)0xeb0000;

/*	printf("\x1b[0;0H%d \n", spr_count);*/

/* 表示数ぶん書き込む */
	if(spr_count > MAX_SPRITE){
		if(total_count & 1){
/*			for(i = spr_count - MAX_SPRITE, j = 0; j < MAX_SPRITE; i++, j++){
				PUT_SP(chr_data[i].x, chr_data[i].y, chr_data[i].pat_num, chr_data[i].atr);
			}*/
			memcpy(spdata, &chr_data[spr_count - MAX_SPRITE], MAX_SPRITE * 4 * 2);
		}else{
/*			for(i = 0; i < MAX_SPRITE; i++){
				PUT_SP(chr_data[i].x, chr_data[i].y, chr_data[i].pat_num, chr_data[i].atr);
			}*/
			memcpy(spdata, chr_data, MAX_SPRITE * 4 * 2);
		}
		tmp_spr_count = MAX_SPRITE;
	}else{
/*		for(i = 0; i < spr_count; i++){
			PUT_SP(chr_data[i].x, chr_data[i].y, chr_data[i].pat_num, chr_data[i].atr);
		}*/
		memcpy(spdata, chr_data, spr_count * 4 * 2);
		spdata += (spr_count * 4);

/* スプライトの表示数が減った場合､減った分を画面外に消去する */
/* 増える分には問題ない */
		if (old_count[spr_page] > spr_count){
			for(i = 0;i < (old_count[spr_page] - spr_count); i++){
				PUT_SP(0,SCREEN_MAX_Y,0,0);
			}
		}
		tmp_spr_count = spr_count;
	}
/* このフレ－ムで表示したスプライトの数を保存 */
	old_count[spr_page] = tmp_spr_count;
	tmp_spr_count = 0;

	spr_flag = 1;
	spr_next = spr_page;
	spr_page ^= 0x01;


	++total_count;

//	wait_vsync();

/*	if(spr_flag){
		spr_flag = 0;
		spram = (unsigned short *)0xeb0000;
		memcpy(spram, chr_data2[spr_next], MAX_SPRITE * 4 * 2);
	}*/

	if(seflag){
//		if(soundflag == TRUE)
//			if(se_check())
//				se_stop();
//		S_IL_FUNC(se_play(sndtable[0], seflag - 1));	/* 効果音 */
//		if(mcd_status >= 0){
//			pcm_play(&SNDBUFF[seflag - 1][0], pcmsize[seflag - 1]);
//		}
		_iocs_adpcmmod(0);
		_iocs_adpcmout(&SNDBUFF[seflag - 1][0], 4 * 256 + 3, pcmsize[seflag - 1]);
		seflag = 0;
	}

//	while(spr_flag != 0);
}

#ifndef XSP

extern void VSYNC_handler(void);

//void 
void  __attribute__((interrupt)) int_vsync(void)
{
//	asm volatile(
//		"rte"
//	);
//	++score;
//	scrdspflag =TRUE;
	/* 割り込み on */
	asm volatile("andi.w	#0x0f8ff,%sr\n");

	vsync_flag = 1;

	VSYNC_handler();

	if(spr_flag){
		spr_flag = 0;
		spram = (unsigned short *)0xeb0000;
		memcpy(spram, chr_data2[spr_next], MAX_SPRITE * 4 * 2);
	}
}

#endif

#endif
