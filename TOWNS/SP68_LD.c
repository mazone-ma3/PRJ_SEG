/* 簡易スプライトロ－ダ */
/* X68KのSP形式をロ－ドする。 */

#include <stdio.h>
#include <stdlib.h>
#include <dos.h>
#include <conio.h>

#include "sg.h"
#include "subfunc.h"
#include "sp68_ld.h"

#define FILE_MAX 1 /* 10 */

FILE *stream[FILE_MAX];


int sp68_load(char *fil, short sprparts)
{
	long i, j, k;
	unsigned char data;
	unsigned char pattern[128];
//	_Far unsigned char *spram_char;
	int spram_char;

//	spram_char = (_Far unsigned char *)spram;
	spram_char = spram;

//	_FP_SEG(spram_char) = 0x130;
//	_FP_OFF(spram_char) = 0x4000;
//	spram_char = 0x4000;

	if ((stream[0] = fopen( fil, "rb")) == NULL) {
		fprintf(stderr, "Can\'t open file %s.", fil);

		fclose(stream[0]);
		return 1;
	}
/*	printf("Loading file:%s\n" ,fil); */

	for (i = 0; i < sprparts; i++){
		fread(pattern, 1, 128, stream[0]);

/* スモ－ルエンディアンに直してブロック再配置 */
		for (j = 0; j < 16; j++){
			for(k=0;k<4;k++){
				data = pattern[j * 4 + k];
//				*(spram_char++) = ((data >> 4) % 16) | ((data % 16) << 4);
				_poke_byte(0x130, spram_char++, ((data >> 4) % 16) | ((data % 16) << 4));
			}
			for(k=0;k<4;k++){
				data = pattern[j * 4 + k + 16 * 4];
//				*(spram_char++) = ((data >> 4) % 16) | ((data % 16) << 4);
				_poke_byte(0x130, spram_char++, ((data >> 4) % 16) | ((data % 16) << 4));
			}
		}
	}
	fclose(stream[0]);

	return 0;
}

int pal68_load(char *fil)
{
	long i, j, k;
	unsigned short data;
	unsigned char pattern[2];
//	_Far unsigned short *palram_char;
	int palram_char;

//	_FP_SEG(palram_char) = 0x130;
//	_FP_OFF(palram_char) = 0x2000;
	palram_char = 0x2000;

	if ((stream[0] = fopen( fil, "rb")) == NULL) {
		fprintf(stderr, "Can\'t open file %s.", fil);

		fclose(stream[0]);
		return 1;
	}
/*	printf("Loading file:%s\n" ,fil); */

	for (i = 0; i < 240; i++){
		k = fread(pattern, 1, 2, stream[0]);
		if(k < 1)
			break;
		data = pattern[1] + pattern[0] * 256;
//		*(palram_char++) = (data >> 1) & 0x7fff;
		_poke_word(0x130, palram_char, (data >> 1) & 0x7fff);
		palram_char += 2;
	}
	fclose(stream[0]);

	return 0;
}


#define MSXWIDTH 256
#define MSXLINE 212
#define SPRSIZEX 8
#define SPRSIZEY 16
//#define SPRPARTS 256

//#define MAX_SPRITE 256

int msxspconv(char *loadfil, short sprparts)
{
	FILE *stream[2];
	unsigned char pattern[10];
	unsigned char msxcolor[MSXWIDTH / 2][MSXLINE];

	long i, j,k,y, x, xx, yy, no, max_xx;

//	_Far unsigned short *spram;
//	int spram_char;
//	spram_char = spram;

	if ((stream[0] = fopen( loadfil, "rb")) == NULL) {
		fprintf(stderr, "Can\'t open file %s.", loadfil);

		fclose(stream[0]);
		return 1;
	}

	fread(pattern, 1, 1, stream[0]);	/* MSX先頭を読み捨てる */
	fread(pattern, 1, 4, stream[0]);	/* MSXヘッダも読み捨てる */
	fread(pattern, 1, 2, stream[0]);	/* MSXヘッダを読み捨てる */

	for(y = 0; y < MSXLINE; ++y){
		for(x = 0; x < MSXWIDTH / 2 ; ++x){
			msxcolor[x][y] = 0;
		}
	}
	for(y = 0; y < MSXLINE; ++y){
		for(x = 0; x < MSXWIDTH / 8; ++x){
			i = fread(pattern, 1, 4, stream[0]);	/* 8dot分 */
			if(i < 1)
				break;

			/* 色分解 */
			msxcolor[0 + x * 4][y] =
				((pattern[1] >>4) & 0x0f) | ((pattern[1] & 0x0f) * 16); 
			msxcolor[1 + x * 4][y] =
				((pattern[0] >>4) & 0x0f) | ((pattern[0] & 0x0f) * 16); 
			msxcolor[2 + x * 4][y] =
				((pattern[3] >>4) & 0x0f) | ((pattern[3] & 0x0f) * 16); 
			msxcolor[3 + x * 4][y] =
				((pattern[2] >>4) & 0x0f) | ((pattern[2] & 0x0f) * 16); 
		}
	}
	fclose(stream[0]);
	max_xx = 128;

//	_FP_SEG(spram) = 0x130;
//	_FP_OFF(spram) = 0x4000;
//	spram = 0x4000;
	j = 0;
	xx=0;
	yy=0;
	x=0;
	for(no = 0; no < sprparts; ++no){
//		printf("\nno =%d ",no);
		for(y = 0; y < SPRSIZEY; ++y){
			for(x = 0; x < SPRSIZEX; x+=2){

				if((x+xx) >= max_xx) {
					xx=0;
					yy+=SPRSIZEY;
				}

//				*(spram++) = msxcolor[x + xx][y + yy] * 256 + msxcolor[x + xx + 1][y + yy];
				_poke_word(0x130, spram, msxcolor[x + xx][y + yy] * 256 + msxcolor[x + xx + 1][y + yy]);
				spram += 2;
			}
		}
		xx+=SPRSIZEX;
	}

	return 0;
}
