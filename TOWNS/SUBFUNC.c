/* 画面初期化とスプライト関係とパッド */

//#define TOWNS

#include "sg.h"
#include "subfunc.h"

#include <egb.h>
#include <snd.h>
#include <spr.h>
#include <dos.h>
#include <conio.h>

char egb_work[1536];

/* 画面設定 */
void grp_set(void){

	char para[64];

	EGB_init(egb_work, 1536);

/* 31kHz出力用 */
	EGB_resolution(egb_work, 0, 3);			/* ペ－ジ0は640x480/16 */
	EGB_resolution(egb_work, 1, 5);			/* ペ－ジ1は256x512/32768 */
	EGB_displayPage(egb_work, 1, 3);		/* 上にくるペ－ジは1で両方とも表示 */

/* もし15kHz出力したいならこうする */
/* 	EGB_resolution(egb_work, 0, 8);			/* ペ－ジ0は512x256/32768 */
/* 	EGB_resolution(egb_work, 1, 11);		/* ペ－ジ1は256x512/32768 */

	EGB_writePage(egb_work, 0);				/* ペ－ジ0(BG)の設定 */
	EGB_displayStart(egb_work, 2, 2, 2);		/* 表示拡大率(縦横2倍) */
	EGB_displayStart(egb_work, 3, 640, 480);	/* EGB画面の大きさ(640x480) */
	EGB_displayStart(egb_work, 0, 0, 0);		/* 表示開始位置 */
	EGB_displayStart(egb_work, 1, 0, 0);		/* 仮想画面中の移動 */

	EGB_writePage(egb_work, 1);				/* ペ－ジ1(スプライト)の設定 */
	EGB_displayStart(egb_work, 2, 2, 2);		/* */
	EGB_displayStart(egb_work, 3, 256, 240);	/* */
	EGB_displayStart(egb_work, 0, 32, 0);		/* */
	EGB_displayStart(egb_work, 1, 0, 2);		/* 下に2ドットずらす(仕様) */

	EGB_color(egb_work, 0, 0x8000);				/* ペ－ジ1をクリアスクリ－ン */
	EGB_color(egb_work, 2, 0x8000);				/* 透明色で埋める */
	EGB_writePage(egb_work, 1);
	EGB_clearScreen(egb_work);
}

/* 色塗り(テスト用) */
void grp_fill(int backcolor){
	char para[64];

	EGB_writePage(egb_work, 0);
	EGB_paintMode(egb_work, 0x22);
	EGB_color(egb_work, 0, backcolor);
	EGB_color(egb_work, 2, backcolor);
	WORD(para + 0) = 32;
	WORD(para + 2) = 0;
	WORD(para + 4) = 256 - 32;
	WORD(para + 6) = 239;
	EGB_rectangle(egb_work, para);
}

/* 画面を戻す(コンソ－ルからの起動対策) */
void grp_term(void){
	EGB_color(egb_work, 0, 0x8000);				/* ペ－ジ1をクリアスクリ－ン */
	EGB_color(egb_work, 2, 0x8000);				/* 透明色で埋める */
	EGB_writePage(egb_work, 1);
	EGB_clearScreen(egb_work);

	EGB_resolution(egb_work, 0, 4);		/* ペ－ジ0は640x400/16 */
	EGB_resolution(egb_work, 1, 4);		/* ペ－ジ1は640x400/16 */
	EGB_displayPage(egb_work, 0, 3);

	EGB_writePage(egb_work, 0);			/* ペ－ジ0をクリアスクリ－ン */
	EGB_clearScreen(egb_work);
	EGB_displayStart(egb_work,0,0,0);
	EGB_displayStart(egb_work, 1, 0, 0);
	EGB_displayStart(egb_work,2,1,1);
	EGB_displayStart(egb_work, 3, 640, 400);
	EGB_writePage(egb_work, 1);			/* ペ－ジ1をクリアスクリ－ン */
	EGB_clearScreen(egb_work);
	EGB_displayStart(egb_work,0,0,0);
	EGB_displayStart(egb_work, 1, 0, 0);
	EGB_displayStart(egb_work,2,1,1);
	EGB_displayStart(egb_work, 3, 640, 400);
}

/* スプライトパタ－ン設定 */
void spr_set(int spbackcolor){
/* スプライト定義 */
	SPR_init();
	spr_clear();
 	spr_fill(spbackcolor);
/*	spr_fill2(7); */
/* 	spr_make(); */
}

/* スプライト面の塗りつぶし設定 */
/* タイリングにすると疑似半透明にもなる */
void spr_fill(int spbackcolor){
	int i;

/* ダブルバッファなので2度行う */
/* バッファ0 */
//	_FP_OFF(vram) = 0x40000;
	vram = 0x40000;
	i = 256 * 2 * 2;
	while(i--){
		_poke_word(0x120, vram, spbackcolor);
		vram += 2;
	}
//		*vram++ = spbackcolor;

/* バッファ1 */
//	_FP_OFF(vram) = 0x60000;
	i = 256 * 2 * 2;
	while(i--){
		_poke_word(0x120, vram, spbackcolor);
		vram += 2;
	}
//		*vram++ = spbackcolor;
}

/* 疑似半透明 */
/* [100000 00000 00000][0001 11 00 111 0 0111] */
/* 0x642 0xc63 0x1ce7 */
/* [8000 7fff] */
void spr_fill2(int level){
	int i,j;

	j = ((((level << 5) | level) << 5) | level);

/* バッファ0 */
//	_FP_OFF(vram) = 0x40000;
	vram = 0x40000;
	i = 128;
	while(i--){
		_poke_word(0x120, vram++, 0x8000);
		_poke_word(0x120, vram++, j);
//		*vram++ = 0x8000;
//		*vram++ = j;
	}
	i = 128;
	while(i--){
		_poke_word(0x120, vram++, j);
		_poke_word(0x120, vram++, 0x8000);
//		*vram++ = j;
//		*vram++ = 0x8000;
	}
/* バッファ1 */
//	_FP_OFF(vram) = 0x60000;
	vram = 0x60000;
	i = 128;
	while(i--){
		_poke_word(0x120, vram++, j);
		_poke_word(0x120, vram++, 0x8000);
//		*vram++ = j;
//		*vram++ = 0x8000;
	}
	i = 128;
	while(i--){
		_poke_word(0x120, vram++, 0x8000);
		_poke_word(0x120, vram++, j);
//		*vram++ = 0x8000;
//		*vram++ = j;
	}
}

/* スプライトを全て画面外に移す */
void spr_clear(void){
	int i;
//	_FP_OFF(spram) = 0;
	spram = 0;
	for(i = 0; i < 1024; i++){
		_poke_word(0x130, spram, 0);
		spram += 2;
//		*(spram++) = 0;
		_poke_word(0x130, spram, SCREEN_MAX_Y + 2);
		spram += 2;
//		*(spram++) = SCREEN_MAX_Y + 2; ///240;
		_poke_word(0x130, spram, 0);
		spram += 2;
//		*(spram++) = 0x0; //3ff;
		_poke_word(0x130, spram, 0x2000);
		spram += 2;
//		*(spram++) = 0x2000; //2fff;
	}
}
/* スプライト配置 */
/*   SPR_setAttribute(P_num, X_sum, Y_sum, Attr, Col_tbl); */
/*   SPR_setPotition(Size, P_num, X_sum, Y_sum, X, Y); */

void spr_on(int num){
//	SPR_display(1, num);
	outp(0x450, 0);
	outp(0x452, (1024 - num) % 256);

	outp(0x450, 1);
	outp(0x452, 0x80 | (((1024 - num) / 256) & 0x03));
}

void spr_off(void){
	outp(0x450, 0);
	outp(0x452, 0xff);
	outp(0x450, 1);
	outp(0x452, 0x7f);
//	SPR_display(0,1);
}

/* パッドの読込 */
void pad_read(int port, int *a, int *b, int *pd){
	int data, ab;

/*	SND_joy_in_2(port, &data); */
 	data = inportb(0x4d0 + port * 2); 

/*	*s = ((data >> 6) & 0x03) ^ 0x03;*/
	ab = ((data >> 4) & 0x03) ^ 0x03;
	*a = ab & 0x01;
	*b = (ab >> 1) & 0x01;

	*pd = (data & 0x0f) ^ 0x0f;
}

