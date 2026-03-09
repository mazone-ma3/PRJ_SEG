/* FM MUSIC SAMPLE for FM TOWNS BIOS版含む */
/* GCC */
/* 参考 PC-9801マシン語サウンドプログラミング、FM TOWNSテクニカルデータブック */
/* Free386のドキュメント(SND.C) */

#include <stdio.h>
#include <stdlib.h>
#include <conio.h>

#include "fmdtwg.h"

#include "key.h"
#include "keytow.h"
#include "tone.h"

//#define BIOSVOL

#define ERROR 1
#define NOERROR 0

#define _disable() asm("cli\n")
#define _enable() asm("sti\n")


#define PARTSUU 6

#ifdef BIOS
#define BIOSVOL
#include "snd.h"
char sndwork[SndWorkSize];
#endif

FILE *fmdstream[2];

//extern void *ms_timer_sub_int;
//extern void *ms_timer_a_int;
//extern void *ms_timer_b_int;
//void int_fm(void);

volatile unsigned char COUNT[PARTSUU] = {1,1,1,1,1,1}; // 音長カウンタ
volatile unsigned char STOPFRG[PARTSUU] = {0,0,0,0,0,0}; // WAIT&SYNC&OFF
volatile unsigned char MAXVOL[PARTSUU] = {15,15,15,15,15,15}; // ボリューム
volatile unsigned char LENGTH[PARTSUU] = {5,5,5,5,5,5}; // 基本音長

volatile unsigned short OFFSET[PARTSUU] = {0,0,0,0,0,0}; // 演奏データ実行中アドレス
volatile unsigned short STARTADR[PARTSUU] = {0,0,0,0,0,0}; // 演奏データ開始アドレス
volatile unsigned char FEEDVOL = 0; // フェードアウトレベル
volatile unsigned char FCOUNT = 1; //フェードアウトカウンタ
volatile unsigned char LOOPTIME = 1; // 演奏回数（０は無限ループ）
volatile unsigned char STOPPARTS = 0; //
volatile unsigned char ENDFRG = 0; //
volatile unsigned char NSAVE = 0; //

unsigned char mem[65536];


void SND_int(void)
{
//	return;

	_disable();
//	SND_fm_timer_a_set(0,0);
	SND_fm_timer_b_set(0,0);

	_disable();
	asm volatile (
		"push	%eax\n"
		"leal _int_fm,%eax\n"
		"mov %eax,(ms_timer_b_int)\n"
		"pop	%eax\n"
	);

	SND_fm_timer_b_set(1,199);

	_enable();
}

int SND_fm_read_status(void){
	return inportb(0x4d8);
}

short bload2(char *loadfil, unsigned short offset)
{
	unsigned short size;
	unsigned short address;
	unsigned char buffer[3];

	if ((fmdstream[0] = fopen( loadfil, "rb")) == NULL) {
//		printf("Can\'t open file %s.", loadfil);
		return ERROR;
	}
	fread( buffer, 1, 2, fmdstream[0]);
	address = buffer[0] + buffer[1] * 256;
	fread( buffer, 1, 2, fmdstream[0]);
	size = (unsigned short)(buffer[0] + buffer[1] * 256) - (unsigned short)address;
	address -= offset;
//	printf("Load file %s. Address %x Size %x End %x\n", loadfil, address , size, (unsigned short)address + size);

	fread( &mem[address] , 1, size, fmdstream[0]);
	fclose(fmdstream[0]);
	return NOERROR;
}

/* FMレジスタ設定 ch.1-3 */
#ifdef BIOS

void set_fm(unsigned char bank, unsigned char reg, unsigned char data)
{
//	register char rd0 asm("bh");
//	register char rd1 asm("dh");
//	register char rd2 asm("dl");

	register short rd0 asm("bx");
	register short rd1 asm("dx");

	register long rd3 asm("eax");

	while(SND_fm_read_status() & 0x80);

//	rd0 = bank;
//	rd1 = reg;
//	rd2 = data;

	rd0 = bank * 256;
	rd1 = reg * 256 | data;

	asm volatile(
		"movb	$0x11,%%ah\n"
		"call	sound_bios1\n"

		"movzbl	%%al,%0\n"

		:"=r"(rd3)	/* 値が返るレジスタ変数 */
		:"r"(rd0),"r"(rd1)//,"r"(rd2)	/* 引数として使われるレジスタ変数 */
//		:"%edx","%ebx"		/* 破壊されるレジスタ */
	);

	return rd3;
}


#else
void set_fm(unsigned char bank, unsigned char reg, unsigned char data)
{
	int port;
	if(bank == 1)
		port = 0x4dc;
	else if(bank == 0)
		port = 0x4d8;

	while(inp(0x4d8) & 0x80);
	outportb(port,reg);
//__asm
//	ld	a,(ix+0)
//__endasm;
//	while(inp(0x4d8) & 0x80);
	outportb(port+2,data);
}
#endif

/*  AR  DR  SR  RR  SL  OL  KS  ML DT1 DT2 AME */
enum { AR, DR, SR, RR, SL, OL, KS, MUL,DT1,DT2,AME };
enum { D1R = DR , TL=OL, D1L = SL, D2R=SR};
enum {CON = 4 * 11, FL};

char op[4] = {0, 2, 1, 3};

/* 音色設定 */
void set_tone(unsigned char no, unsigned char ch)
{
	unsigned char i, j, k, bank = 0;
	if(ch >= 3){
		ch -= 3;
		bank = 1;
	}

	for(i = 0; i < 4; ++i){
		j = ch + op[i] * 4;
		k = i * 11;
		set_fm(bank, 0x30 + j, tone_table[no][MUL + k] | tone_table[no][DT1 + k] * 16);
		set_fm(bank, 0x40 + j, tone_table[no][TL + k]);
		set_fm(bank, 0x50 + j, tone_table[no][AR + k] | tone_table[no][KS + k] * 64);
		set_fm(bank, 0x60 + j, tone_table[no][D1R + k] | tone_table[no][AME + k] * 128);
		set_fm(bank, 0x70 + j,tone_table[no][SR + k]); // | tone_table[no][DT2 + k] * 64);
		set_fm(bank, 0x80 + j, tone_table[no][RR + k] | tone_table[no][SL + k] * 16);
//		set_fm(bank, 0x90 + j, 0x0f);
	}
//	j = 0xb0 + ch;
	set_fm(bank, 0xb0 + ch, tone_table[no][CON] | tone_table[no][FL] * 8);// | 0xc0);
//	set_fm(bank, 0xb4 + ch, 0xc0);


/*	j = 0x30 + ch;
	for(i = 0; i < 28; ++i){
		set_fm(bank, j, tone_table[no][i]);
		j += 4;
	}
	j += 0x10;
		set_fm(bank, j, tone_table[no][i]);
*/
}

/* 音程設定 */
void set_key(unsigned char no, unsigned char ch)
{
	char i, j, bank = 0;
	if(ch >= 3){
		ch -= 3;
		bank = 1;
	}
	set_fm(bank, 0xa4 + ch, key_table[no][0]);
	set_fm(bank, 0xa0 + ch, key_table[no][1]);
}


char key[6] = {0, 1, 2, 4, 5, 6};

void stop(void)
{
	unsigned char i;
	_disable();
	SND_fm_timer_b_set(0,0);
	for(i = 0; i < PARTSUU; ++i){
		set_fm(0, 0x28, 0x00 | key[i]);	/* off */
	}
	_enable();
//	SND_end();
}

unsigned char count = 0;

void int_fm(void)
{
	unsigned char i, j, no, ch;
	unsigned char data;

playloop:
playloop2:
//	if(FEEDVOL == 15)
//		stop();
//	if(FEEDVOL)
//		feedsub();
	for(i = 0; i < PARTSUU; ++i){
		if(STOPFRG[i] >= 254){
			/* 同期待ち・演奏終了 演奏スキップ */
			continue;
		}
		--COUNT[i];
		if(!(COUNT[i])){
			/* 演奏処理 */
			for(;;){
				data = mem[OFFSET[i]];
				switch(data){
					case 226:	/* 音量変更 V */
						data = mem[--OFFSET[i]];
						MAXVOL[i] = data;
						break;
					case 227:	/* 標準音調変更 T */
						data = mem[--OFFSET[i]];
						LENGTH[i] = data;
						break;
					case 228:	/* 音色変更 */
						no = mem[--OFFSET[i]];
						set_tone(no, i);
						break;
					case 225:	/* 直接出力 Y */
						ch = mem[--OFFSET[i]];
						no = mem[--OFFSET[i]];
						set_fm(0, ch, no);
						break;
					case 255:	/* ループ */
						OFFSET[i] = STARTADR[i] + 1;
						ENDFRG = 1;
						break;
					case 254:	/* 同期 */
						STOPFRG[i] = 254;
						--OFFSET[i];
						++STOPPARTS;
						goto playend;
						break;
					default:
						/* 演奏 */
						STOPFRG[i] = data & 0x7f;
						set_fm(0, 0x28, 0x00 | key[i]);	/* off */
						if((data & 0x7f) != 0){
							set_key((data & 0x7f) - 1, i);	/* key */
							set_fm(0, 0x28, 0xf0 | key[i]);	/* on */
						}
						if(data & 0x80){	/* 音長が設定されている */
							data = LENGTH[i];
						}else{
							data = mem[--OFFSET[i]];
						}
						COUNT[i] = data;
						--OFFSET[i];
						goto playend;
						break;
				}
				--OFFSET[i];
			}
		}
playend:
		continue;
	}
	if(PARTSUU == STOPPARTS){	/* 演奏パート数=停止パート数 */
		j = PARTSUU;
		for(i = 0; i < PARTSUU; ++i){
			if(STOPFRG[i] != 255){	/* 演奏停止でない */
				STOPFRG[i] = 0;	/* 演奏中 */
				COUNT[i] = 1;	/* 音長カウンタを1にする */

				--j;		/* 演奏停止パート数の計算 */
			}
		}
		STOPPARTS = j;	/* 演奏停止パート数 */
		goto playloop2;
	}

	/* 割り込み終了 */
	if((--ENDFRG) == 0){	/* 終了カウンタが0なら終了 */

		ENDFRG = 0;
		if(!LOOPTIME)
			return;			/* 無限ループ */
		if(--LOOPTIME){
			goto playloop;	/* ループ回数が0以外ならループ */
		}
		/* 演奏自己停止 */
		stop();
	}
	ENDFRG = 0;
}

/*
int	main(int argc,char **argv)
{
	unsigned short no = 0;
	unsigned char i, ch = 0;
	unsigned char noise = 0;

//	SND_init(sndwork);

	if (argc < 2){
		return ERROR;
	}

	if (argc >= 3){
		no = atoi(argv[2]);
		if((no % 256) > 9)
			no = 0;
	}

	if (argc >= 4){
		noise = atoi(argv[3]);
	}

//	printf("Hello,world.\n");

//	getchar();
}
*/

int load_fmdbgm(char *filename)
{
//	if(bload2(argv[1], 0x1000*0) == ERROR)
	if(bload2(filename, 0x1000*0) == ERROR)
		return ERROR;
//		exit(1);
	return NOERROR;
}

void play_fmdbgm(void)
{
	unsigned short no = 0;
	unsigned char i, ch = 0;
	unsigned char noise = 0;

#ifdef BIOSVOL
//	printf("BIOS初期化中");
//	SND_init(sndwork);
//	printf("BIOS初期化終了");

#endif
	FEEDVOL = 0;
	FCOUNT = 1;
	STOPPARTS = 0;
	ENDFRG = 0;
	NSAVE = 0;

	for(i = 0; i < PARTSUU; ++i){
		int j = 0xdb00-0x1100 + i * 2 + (no % 256) * 12 * 2;
		COUNT[i] = 1;
		STARTADR[i] = mem[j] + mem[j+1] * 256;
		if(!STARTADR[i]){
			STOPFRG[i] = 255;
			MAXVOL[i] = 255;
			STOPPARTS++;
		}else{
			MAXVOL[i] = 0;
			STOPFRG[i] = 0;
		}
		STARTADR[i] -= 0x1000;
		OFFSET[i] = STARTADR[i];
		set_tone(noise, i);
	}
	LOOPTIME = 0; //no / 256;

	/* ミュート解除 */
#ifdef BIOSVOL
//	SND_elevol_all_mute(-1);
//	SND_elevol_mute(0x33);
#else
//	outp(0x4e0,0x3f);
//	outp(0x4e1, (inp(0x4e1) | 0x0c) & 0x8f);
//	outp(0x4e2,0x3f);
//	outp(0x4e3, (inp(0x4e3) | 0x0c) & 0x8f);
	outp(0x4ec, 0x40); //(inp(0x4ec) | 0x40) & 0xc0);
	outp(0x4d5, 0x02);
//	outp(0x60,0x4);
#endif

	SND_int();
//	getchar();
}

void stop_fmdbgm(void)
{
	stop();
//	return 0;
}
