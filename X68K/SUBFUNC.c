/* 入出力設定サブル－チン群 for X680x0 with XSP */

#include "common.h"

#define X68K

#include "sp.h"
#include "subfunc.h"

#ifdef XSP

#include "XSP2lib.H"
//#include "PCM8Afnc.H"

#endif

#include <x68k/iocs.h>

void grp_set(void);
void spr_set(void);
void spr_fill(int);
void spr_clear(void);
/* void spr_make(void); */


/* 画面設定 */
void grp_set(void){
/*	CRTMOD(10); */
	_iocs_crtmod(6);
	_iocs_ms_init();
	_iocs_skey_mod(0, 0, 0);
	_iocs_ms_curof();
	_iocs_b_curoff();
	_iocs_g_clr_on();
}

/* 画面を戻す */
void grp_term(void){
//	_iocs_ms_curon();
	_iocs_skey_mod(-1, 0, 0);
	_iocs_b_curon();
	_iocs_crtmod(16);
}


/* スプライトを全て画面外に移す */
void spr_clear(void){
	int i;
#ifndef XSP
	spram = (unsigned short *)0xeb0000;
#endif
#ifdef XSP
	for(i = 0;i < MAX_SPRITE; i++){
		/* スプライトの表示登録 */
//		xsp_set(0, SCREEN_MAX_Y, 0, 0);
		chr_data[i].x = 0;
		chr_data[i].y = SCREEN_MAX_Y;
		chr_data[i].pat_num = 0;
		chr_data[i].atr = 0;
#else
	for(i = 0;i < 128; i++){
		*(spram++) = 0;
		*(spram++) = SCREEN_MAX_Y; //256;
		*(spram++) = 0;
		*(spram++) = 0; 
#endif
	}
}
/* スプライト配置 */
/*  SPR_setAttribute(P_num, X_sum, Y_sum, Attr, Col_tbl); */
/*  SPR_setPotition(Size, P_num, X_sum, Y_sum, X, Y); */

void spr_on(int num){
	_iocs_sp_on();
}

void spr_off(void){
	_iocs_sp_off();
}

/* スプライトパタ－ン設定 */
void spr_set(void){
	spr_off();

/* スプライト定義 */
/*	SPR_init(); */
	spr_clear(); 
/*	spr_fill(SPBACKCOLOR); */
/*	spr_make(); */
}


/* パッドの読込 */
void pad_read(int port, int *a, int *b, int *pd){
	unsigned char *reg = (unsigned char *)0xe9a001;
	int data;
	int ab;

/*	data = JOYGET(port);*/
	data = reg[port * 2];

/* 方向 */
	*pd = (data & 0x0f) ^ 0x0f;

/* ボタン */
	ab = ((data >> 5) & 0x03) ^ 0x03;
	*a = (ab >> 1) & 0x01;
	*b = ab & 0x01;
}

