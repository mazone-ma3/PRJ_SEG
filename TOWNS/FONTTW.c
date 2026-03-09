/* MSX-SC5->FM TOWNS SPR CONV. for High-C */

#include <stdio.h>
#include <stdlib.h>

#include <egb.h>
#include <snd.h>
#include <spr.h>
#include <dos.h>
#include <conio.h>

#include "fonttw.h"
#include "sg.h"

//#define MAX_SPRITE 256

FILE *fontstream[2];

unsigned char pattern[10];
unsigned char msxcolor[MSXWIDTH / 2][MSXLINE];

short font_load(char *loadfil, unsigned short sprparts)
{

	long i, j,k,y, x, xx, yy, no, max_xx;

//	_Far unsigned short *spram;
//	int spram;

	if ((fontstream[0] = fopen( loadfil, "rb")) == NULL) {
		fprintf(stderr, "Can\'t open file %s.", loadfil);

		fclose(fontstream[0]);
		return 1;
	}

	fread(pattern, 1, 1, fontstream[0]);	/* MSX先頭を読み捨てる */
	fread(pattern, 1, 4, fontstream[0]);	/* MSXヘッダも読み捨てる */
	fread(pattern, 1, 2, fontstream[0]);	/* MSXヘッダを読み捨てる */

	for(y = 0; y < MSXLINE; ++y){
		for(x = 0; x < MSXWIDTH / 2 ; ++x){
			msxcolor[x][y] = 0;
		}
	}
	for(y = 0; y < MSXLINE; ++y){
		for(x = 0; x < MSXWIDTH / 8; ++x){
			i = fread(pattern, 1, 4, fontstream[0]);	/* 8dot分 */
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
	fclose(fontstream[0]);
	max_xx = 64;

//	_FP_SEG(spram) = 0x130;
//	_FP_OFF(spram) = 0x4000 + 256*SPRSIZEX*SPRSIZEY;
//	spram = (0x4000 + 256*SPRSIZEX*SPRSIZEY);
	j = 0;
	xx=0;
	yy=0;
	x=0;
	for(no = 0; no < sprparts; ++no){
//		printf("\nno =%d ",no);
		for(y = 0; y < SPRSIZEY / 2; ++y){
			for(x = 0; x < SPRSIZEX / 2; x+=2){

				if((x+xx) >= max_xx) {
					xx=0;
					yy+=SPRSIZEY / 2;
				}

//				*(spram++) = msxcolor[x + xx][y + yy] * 256 + msxcolor[x + xx + 1][y + yy];
				_poke_word(0x130, spram,  msxcolor[x + xx][y + yy] * 256 + msxcolor[x + xx + 1][y + yy]);
				spram += 2;
			}
			for(x = 0; x < SPRSIZEX / 2; x+=2){
//				*(spram++) = 0;
				_poke_word(0x130, spram,  0);
				spram += 2;
			}
		}
		for(y = 0; y < SPRSIZEY / 2; ++y){
			for(x = 0; x < SPRSIZEX; x += 2){
//				*(spram++) = 0;
				_poke_word(0x130, spram,  0);
				spram += 2;
			}
		}
		xx+=SPRSIZEX / 2;
	}

	return NOERROR;
}


short title_load(char *loadfil, unsigned short sprparts, long max_xx)
{
	long i, j,k,y, x, xx, yy, no; //, max_xx;

//	_Far unsigned short *spram;
//	int spram;

	if ((fontstream[0] = fopen( loadfil, "rb")) == NULL) {
		fprintf(stderr, "Can\'t open file %s.", loadfil);

		fclose(fontstream[0]);
		return 1;
	}

	fread(pattern, 1, 1, fontstream[0]);	/* MSX先頭を読み捨てる */
	fread(pattern, 1, 4, fontstream[0]);	/* MSXヘッダも読み捨てる */
	fread(pattern, 1, 2, fontstream[0]);	/* MSXヘッダを読み捨てる */

	for(y = 0; y < MSXLINE; ++y){
		for(x = 0; x < MSXWIDTH / 2 ; ++x){
			msxcolor[x][y] = 0;
		}
	}
	for(y = 0; y < MSXLINE; ++y){
		for(x = 0; x < MSXWIDTH / 8; ++x){
			i = fread(pattern, 1, 4, fontstream[0]);	/* 8dot分 */
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
	fclose(fontstream[0]);
//	max_xx = 128;

//	_FP_SEG(spram) = 0x130;
//	_FP_OFF(spram) = 0x4000 + 256*SPRSIZEX*SPRSIZEY;
//	spram = (0x4000 + 256*SPRSIZEX*SPRSIZEY);
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
				_poke_word(0x130, spram,  msxcolor[x + xx][y + yy] * 256 + msxcolor[x + xx + 1][y + yy]);
				spram += 2;
			}
		}
		xx+=SPRSIZEX;
	}

	return 0;
}
