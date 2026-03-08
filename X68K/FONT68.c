/* MSX-SC5->X68K-PCG/SPR CONV. for GCC */

#include <stdio.h>
#include <stdlib.h>
//#include <sys\dos.h>
//#include <conio.h>

#include <x68k/dos.h>
#include <x68k/iocs.h>

#include "font68.h"

/************************************************************************/
/*		BIT操作マクロ定義												*/
/************************************************************************/

/* BITデータ算出 */
#define BITDATA(n) (1 << (n))

/* BITセット */
#define BITSET(BITNUM, NUMERIC) {	\
	NUMERIC |= BITDATA(BITNUM);		\
}

/* BITクリア */
#define BITCLR(BITNUM, NUMERIC) {	\
	NUMERIC &= ~BITDATA(BITNUM);	\
}

/* BITチェック */
#define BITTST(BITNUM, NUMERIC) (NUMERIC & BITDATA(BITNUM))

/* BIT反転 */
#define BITNOT(BITNUM, NUMERIC) {	\
	NUMERIC ^= BITDATA(BITNUM);		\
}

unsigned char pal[16][3] = {
	{  0,  0,  0},
	{  0,  0,  0},
	{  3, 13,  3},
	{  7, 15,  7},
	{  3,  3, 15},
	{  5,  7, 15},
	{ 11,  3,  3},
	{  5, 13, 15},
	{ 15,  3,  3},
	{ 15,  7,  7},
	{ 13, 13,  3},
	{ 13, 13,  7},
	{  3,  9,  3},
	{ 13,  5, 11},
	{ 11, 11, 11},
	{ 15, 15, 15},
};


#define MSXWIDTH 256
#define MSXLINE 212
#define PCGSIZEX 4
#define PCGSIZEY 8
//#define PCGPARTS 256
#define MAXSPRITE 128

FILE *stream[2];

unsigned char pattern[10];
unsigned char msxcolor[MSXWIDTH / 2][MSXLINE];
unsigned short *vram_adr;

unsigned char read_pattern[MSXWIDTH * 256 * 2+ 2];

/*パレット・セット*/
/*void pal_set(unsigned char color, unsigned char red, unsigned char blue,
	unsigned char green)
{
	unsigned short *pal_port;
	pal_port = (unsigned short *)(0xe82200 + color * 2);
	if(color){
		*pal_port = (green * 32 * 32 + red * 32 + blue) * 2 + 1;
	}else{
		*pal_port = 0;
	}
}
*/
/*void pal_all(void)
{
	unsigned char i;
	for(i = 0; i < 16; i++)
		pal_set(i, ((pal[i][0] + 1)*2-1) * (pal[i][0] != 0), ((pal[i][2]+1)*2-1) * (pal[i][2] != 0), ((pal[i][1]+1)*2-1) * (pal[i][1] != 0));
}
*/
short font_load(char *loadfil, short offset, short pcgparts)
{
	long i, j,k,y, x, xx, yy, no, max_xx;

	unsigned short *spram;

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
			i = fread(read_pattern, 1, 4, stream[0]);	/* 8dot分 */
			if(i < 1)
				break;

			/* 色分解 */
			msxcolor[0 + x * 4][y] = read_pattern[0]; 
			msxcolor[1 + x * 4][y] = read_pattern[1]; 
			msxcolor[2 + x * 4][y] = read_pattern[2];
			msxcolor[3 + x * 4][y] = read_pattern[3];
		}
	}
	fclose(stream[0]);
	max_xx = 64;
	spram  = (unsigned short *)0xeb8000;
	spram += ((64 * offset));

	j = 0;
	xx=0;
	yy=0;
	x=0;
	for(no = 0; no < pcgparts; ++no){
//		printf("\nno =%d ",no);
		for(y = 0; y < PCGSIZEY; ++y){
			for(x = 0; x < PCGSIZEX; x+=2){

				if((x+xx) >= max_xx) {
					xx=0;
					yy+=PCGSIZEY;
				}

				*(spram++) = msxcolor[x + xx][y + yy] * 256 + msxcolor[x + xx + 1][y + yy];
			}
		}
		xx+=PCGSIZEX;
	}

//	pal_all();

	return 0;
}

unsigned char conv_tbl[16] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14 , 15};


short title_load(char *loadfil, short x, short y, short msxline)
{
	long i, j, count, count2;
	int k=0, l=0, m=0;
//	unsigned char read_pattern[MSXWIDTH * MSXLINE * 2+ 2];
//	unsigned char pattern[10];
	unsigned short x68color[4];
	unsigned char msxcolor[8];
	unsigned char color;
	unsigned char *vram_adr;

	if ((stream[0] = fopen( loadfil, "rb")) == NULL) {
		printf("Can\'t open file %s.", loadfil);

		fclose(stream[0]);
		return 1;
	}
	fread(pattern, 1, 1, stream[0]);	/* MSX先頭を読み捨てる */
	fread(pattern, 1, 4, stream[0]);	/* MSXヘッダも読み捨てる */

	fread(pattern, 1, 2, stream[0]);	/* MSXヘッダを読み捨てる */


//	for(count = 0; count < 1; ++count){
		i = fread(read_pattern, 1, MSXWIDTH * msxline, stream[0]);
		m = 0;
//		if(i < 1)
//			break;
		for(count2 = 0; count2 < MSXWIDTH * msxline / 4; ++count2){
	
			/* 色分解 */
			msxcolor[0] = (read_pattern[m] >>4) & 0x0f;
			msxcolor[1] = read_pattern[m++] & 0x0f;
			msxcolor[2] = (read_pattern[m] >> 4) & 0x0f;
			msxcolor[3] = read_pattern[m++] & 0x0f;
			msxcolor[4] = (read_pattern[m] >>4) & 0x0f;
			msxcolor[5] = read_pattern[m++] & 0x0f;
			msxcolor[6] = (read_pattern[m] >> 4) & 0x0f;
			msxcolor[7] = read_pattern[m++] & 0x0f;

			for(i = 0; i < 4; ++i){
				x68color[i] = 0;
			}
			for(j = 0; j < 8; ++j){
				for(i = 0; i < 4; ++i){
					color = conv_tbl[msxcolor[j]];	/* 色変換 */
					if(BITTST(i, color)){
						BITSET(7-j, x68color[i]);
					}else{
						BITCLR(7-j, x68color[i]);
					}
				}
			}
			for(i = 0; i < 4; ++i){
				pattern[i] = x68color[i];
			}
			vram_adr = (unsigned char *)0xe00000 + k + l + (x + y * 0x80); // * 2;
			*(vram_adr + 0x20000 * 0) = pattern[0];
			*(vram_adr + 0x20000 * 1) = pattern[1];
			*(vram_adr + 0x20000 * 2) = pattern[2];
			*(vram_adr + 0x20000 * 3) = pattern[3];

			k += 1;
			if(k >= (32)){
				k = 0;
				l += 0x80; //(256);
			}
		}
//	}
	fclose(stream[0]);

	return 0;
}

short title_load2(char *loadfil, unsigned short *vram_offset, short msxline)
{
	long i, count, count2;
	int k=0, l=0, m=0;
//	unsigned char read_pattern[MSXWIDTH * MSXLINE * 2+ 2];
//	unsigned char pattern[10];
	unsigned short x68color[4];
	unsigned char msxcolor[8];

	if ((stream[0] = fopen( loadfil, "rb")) == NULL) {
//		printf("Can\'t open file %s.", loadfil);

		fclose(stream[0]);
		return 1;
	}
	fread(pattern, 1, 1, stream[0]);	/* MSX先頭を読み捨てる */
	fread(pattern, 1, 4, stream[0]);	/* MSXヘッダも読み捨てる */

	fread(pattern, 1, 2, stream[0]);	/* MSXヘッダを読み捨てる */


//	for(count = 0; count < 1; ++count){
		i = fread(read_pattern, 1, MSXWIDTH * msxline, stream[0]);
		m = 0;
//		if(i < 1)
//			break;
		for(count2 = 0; count2 < (MSXWIDTH * msxline / 2); ++count2){
	

			/* 色分解 */
			msxcolor[0] = (read_pattern[m] >>4) & 0x0f;
			msxcolor[1] = read_pattern[m++] & 0x0f;

			x68color[0] = msxcolor[0];
			x68color[1] = msxcolor[1];
//			vram_adr = (unsigned short *)0xc00000 + (k + l) * 2; /* GRP0 */
//			vram_adr = (unsigned short *)0xc80000 + (k + l) * 2; /* GRP1 */

			vram_adr = vram_offset + (k + l) * 2;
			*vram_adr = x68color[0];
			vram_adr++;
			*vram_adr = x68color[1];

			k += 1;
			if(k >= (128)){
				k = 0;
				l += (256);
			}
		}
//	}
	fclose(stream[0]);

	return 0;
}
