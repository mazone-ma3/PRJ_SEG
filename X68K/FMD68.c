/* FM MUSIC SAMPLE for X68K */
/* GCC(elf2x68k) */
/* 参考 PC-9801マシン語サウンドプログラミング、Inside X68000 */
/* プログラマーのためのX68000環境ハンドブック */

#include "fmd68.h"

#include <stdio.h>
#include <stdlib.h>
#include <x68k/iocs.h>

#include "key.h"
#include "keyx68.h"
#include "tone.h"

#define IOCS

FILE *fmdstream[2];

#define ERROR 1
#define NOERROR 0

#define PARTSUU 8

volatile unsigned char COUNT[PARTSUU] = {1,1,1,1,1,1,1,1}; // 音長カウンタ
volatile unsigned char STOPFRG[PARTSUU] = {0,0,0,0,0,0,0,0}; // WAIT&SYNC&OFF
volatile unsigned char MAXVOL[PARTSUU] = {15,15,15,15,15,15,15,15}; // ボリューム
volatile unsigned char LENGTH[PARTSUU] = {5,5,5,5,5,5,5,5}; // 基本音長

volatile unsigned short OFFSET[PARTSUU] = {0,0,0,0,0,0,0,0}; // 演奏データ実行中アドレス
volatile unsigned short STARTADR[PARTSUU] = {0,0,0,0,0,0,0,0}; // 演奏データ開始アドレス
volatile unsigned char FEEDVOL = 0; // フェードアウトレベル
volatile unsigned char FCOUNT = 1; //フェードアウトカウンタ
volatile unsigned char LOOPTIME = 0; // 演奏回数（０は無限ループ）
volatile unsigned char STOPPARTS = 0; //
volatile unsigned char ENDFRG = 0; //
volatile unsigned char NSAVE = 0; //


unsigned char mem[65536];

short bload2(char *loadfil, unsigned short offset)
{
	unsigned short size;
	unsigned short address;
	unsigned char buffer[3];

	if ((fmdstream[0] = fopen( loadfil, "rb")) == NULL) {
		printf("Can\'t open file %s.", loadfil);
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

#define port1 ((volatile unsigned char *)0xe90001)
#define port2 ((volatile unsigned char *)0xe90003)

/* FMレジスタ設定 */
void set_fm(unsigned char reg, unsigned char data)
{
#ifdef IOCS
	register long rd0 asm ("d0");
	register long rd1 asm ("d1");
	register long rd2 asm ("d2");

	rd0 = 0x68;
	rd1 = reg;
	rd2 = data;

asm volatile(
	"trap	#15\n"
	:"=d"(rd0)	/* 値が返るレジスタ変数 */
	:"d"(rd0),"d"(rd1),"d"(rd2)	/* 引数として使われるレジスタ変数 */
//	:"d0"		/* 破壊されるレジスタ */
);

//	_iocs_opmset(reg, data);
#else
	unsigned char a;

	while(*port2 & 0x80);
dummy:
	*port1 = reg;
	while (*port2 & 0x80);
dummy2:
	*port2 = data;
#endif
}

/*  AR  DR  SR  RR  SL  OL  KS  ML DT1 DT2 AME */
enum { AR, DR, SR, RR, SL, OL, KS, MUL,DT1,DT2,AME };
enum { D1R = DR , TL=OL, D1L = SL, D2R=SR};
enum {CON = 4 * 11, FL};

char op[4] = {0, 2, 1, 3};

/* 音色設定 */
void set_tone(int no, int ch)
{
	unsigned char i, j, k;

	for(i = 0; i < 4; ++i){
		j = ch + op[i] * 8;
		k = i * 11;
		set_fm(0x40 + j, tone_table[no][MUL + k] | tone_table[no][DT1 + k] * 16);
		set_fm(0x60 + j, tone_table[no][TL + k]);
		set_fm(0x80 + j, tone_table[no][AR + k] | tone_table[no][KS + k] * 64);
		set_fm(0xa0 + j, tone_table[no][D1R + k] | tone_table[no][AME + k] * 128);
		set_fm(0xc0 + j,tone_table[no][D2R + k] | tone_table[no][DT2 + k] * 64);
		set_fm(0xe0 + j, tone_table[no][RR + k] | tone_table[no][D1L + k] * 16);
	}
	j = 0x20 + ch;
	set_fm(j, tone_table[no][CON] | tone_table[no][FL] * 8 | 0xc0);

/*
	for(i = 0; i < 4; ++i){
	j = 0x40 + ch;
	for(i = 0; i < 24; ++i){
		set_fm(j, tone_table[no][i]);
		j+=8;
	}

	j += 0x20;
	i += 4;
	set_fm(j, tone_table[no][i] | 0xc0);
*/
}

/* 音階設定 */
void set_key(int no, int ch)
{
	set_fm(0x28 + ch, key_table[no]);
	set_fm(0x30 + ch, 5);
}

char key[8] = {0, 1, 2, 3, 4, 5, 6, 7};


void stop(void);

unsigned char count = 0;

//void  __attribute__((interrupt))int_fm(void)
//{
//}

void  __attribute__((interrupt))int_fm(void)
{
	unsigned char i, j, no, ch;
	unsigned char data;
	volatile unsigned char status;

	/* YM2151 ステータス読み出し → タイマーフラグ自動クリア + IRQフラグもクリアされる */
	status = *port2;	// $E90003 を読む（これだけでTimerフラグがリセットされる場合が多い）

	/* 念のため明示的にIRQリセット（YM2151のレジスタ$04に0x80書き込みでIRQフラグクリア） */
	set_fm(0x04, 0x80);	// IRQフラグ & Timerフラグを強制クリア（安全策）

	/* 割り込み on */
	asm volatile("andi.w	#0x0f8ff,%sr\n");

	set_fm(0x14, 0x2a);

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
						set_fm(ch, no);
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
						set_fm(0x08, 0x00 | key[i]);	/* off */
						if((data & 0x7f) != 0){
							set_key((data & 0x7f) - 1, i);	/* key */
							set_fm(0x08, 0xf0 | key[i]);	/* on */
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
			goto playend2;
//			return;			/* 無限ループ */
		if(--LOOPTIME){
			goto playloop;	/* ループ回数が0以外ならループ */
		}
		/* 演奏自己停止 */
		stop();
	}
	ENDFRG = 0;
playend2:
//	asm volatile("andi.w	#0x0f8ff,%sr\n");
}

static volatile uint8_t s_mfpBackup[0x18] = {
	 0, 0, 0, 0,  0, 0, 0, 0,  0, 0, 0, 0,  0, 0, 0, 0,  0, 0, 0, 0,  0, 0, 0, 0,
};
static volatile uint32_t s_vector118Backup = 0;
static volatile uint32_t s_uspBackup = 0;

int init_sndint(void)
{
	int ret = 0;
	/* 割り込み off */
	asm volatile("ori.w	#0x0700,%sr\n");

#ifdef DEBUG
	ret = _iocs_opmintst(int_fm);

	asm volatile (
		"AER		= 0x003\n"
		"IERA		= 0x007\n"
		"IERB		= 0x009\n"
		"ISRA		= 0x00F\n"
		"ISRB		= 0x011\n"
		"IMRA		= 0x013\n"

		"IMRB		= 0x015\n"

//		"	lea.l	int_vsync,%a2\n"

		/* MFP のバックアップを取る */
		"	movea.l	#0x0e88000,%a0\n"			/* a0.l = MFPアドレス */
//		"	lea.l	s_mfpBackup(%pc),%a1\n"		/* a1.l = MFP保存先アドレス */
		"	lea.l	s_mfpBackup,%a1\n"		/* a1.l = MFP保存先アドレス */
		"	move.b	AER(%a0),AER(%a1)\n"		/*  AER 保存 */
//		"	move.b	IERB(%a0),IERB(%a1)\n"		/* IERB 保存 */
		"	move.b	IMRB(%a0),IMRB(%a1)\n"		/* IMRB 保存 */
//		"	move.b	IMRA(%a0),IMRA(%a1)\n"		/* IMRA 保存 */

//		"	move.l	#0x118,s_vector118Backup\n"	/* 変更前の V-disp ベクタ */

		/* 割り込み設定 */
//		"	move.l	%a2,0x118\n"				/* ベクタ書換え */
//		"	bclr.b	#4,AER(%a0)\n"				/* 帰線期間と同時に割り込む */
//		"	bset.b	#6,IMRB(%a0)\n"				/* マスクをはがす */
//		"	bset.b	#6,IERB(%a0)\n"				/* 割り込み許可 */

		"	bclr.b	#3,IMRB(%a0)\n"				/* マスクをはがす */
//		"	bset.b	#3,IMRA(%a0)\n"				/* マスクをはがす */

//		:"=d"(rd0)
//		:"d"(rd0),"d"(rd1),"a"(ra0),"a"(ra1),"a"(ra2) //,"r"(rpc)
	);

#else
//	ret = _iocs_vdispst (int_vsync, 0, 0*256+1);
	ret = _iocs_opmintst(int_fm);
#endif


	/* 割り込み on */
	asm volatile("andi.w	#0x0f8ff,%sr\n");
	return ret;
}

void reset_sndint(void)
{
	asm volatile("ori.w	#0x0700,%sr\n");

#ifdef DEBUG
	_iocs_opmintst (0);

	asm volatile(
		"AER		= 0x003\n"
		"IERA		= 0x007\n"
		"IERB		= 0x009\n"
		"ISRA		= 0x00F\n"
		"ISRB		= 0x011\n"
		"IMRA		= 0x013\n"
		"IMRB		= 0x015\n"

		/* MFP の設定を復帰 */
		"	movea.l	#0x0e88000,%a0\n"					/* a0.l = MFPアドレス */
//		"	lea.l	s_mfpBackup(%pc),%a1\n"			/* a1.l = MFPを保存しておいたアドレス */
		"	lea.l	s_mfpBackup,%a1\n"			/* a1.l = MFPを保存しておいたアドレス */

//		"	move.b	AER(%a1),%d0\n"
//		"	andi.b	#%%0101_0000,%d0\n"
//		"	andi.b	#0x50,%d0\n"
//		"	andi.b	#%%1010_1111,AER(%a0)\n"
//		"	andi.b	#0xaf,AER(%a0)\n"
//		"	or.b	%d0,AER(%a0)\n"					/* AER bit4&6 復帰 */

//		"	move.b	IERB(%a1),%d0\n"
//		"	andi.b	#%%0100_0000,%d0\n"
//		"	andi.b	#0x40,%d0\n"
//		"	andi.b	#%%1011_1111,IERB(%a0)\n"
//		"	andi.b	#0xbf,IERB(%a0)\n"
//		"	or.b	%d0,IERB(%a0)\n"					/* IERB bit6 復帰 */

//		"	move.b	IMRA(%a1),%d0\n"
//		"	andi.b	#0x08,%d0\n"
//		"	andi.b	#0xf7,IMRA(%a0)\n"
//		"	or.b	%d0,IMRA(%a0)\n"					/* IMRA bit3 復帰 */

		"	move.b	IMRB(%a1),%d0\n"
		"	andi.b	#0x08,%d0\n"
		"	andi.b	#0xf7,IMRB(%a0)\n"
		"	or.b	%d0,IMRB(%a0)\n"					/* IMRB bit3 復帰 */

//		"	move.b	IMRB(%a1),%d0\n"
//		"	andi.b	#%%0100_0000,%d0\n"
//		"	andi.b	#0x40,%d0\n"
//		"	andi.b	#%%1011_1111,IMRB(%a0)\n"
//		"	andi.b	#0xbf,IMRB(%a0)\n"
//		"	or.b	%d0,IMRB(%a0)\n"					/* IMRB bit6 復帰 */

		/* V-DISP 割り込み復帰 */
//		"	move.l	s_vector118Backup(%pc),0x118\n"
//		"	move.l	s_vector118Backup,0x118\n"
//		:"=d"(rd0)
//		:"d"(rd0),"d"(rd1),"a"(ra0),"a"(ra1)
	);
#else
	_iocs_opmintst (0);
#endif
	asm volatile("andi.w	#0x0f8ff,%sr\n");
}

void stop(void)
{
	unsigned char i;
	set_fm(0x14, 0x0);
	for(i = 0; i < PARTSUU; ++i){
		set_fm(0x08, 0x00 | key[i]);	/* off */
	}
}
/*
int	main(int argc,char **argv)
{
	unsigned short no = 0;
	unsigned char i, ch = 0;
	unsigned char noise = 0;

dum:	_iocs_b_super(0);		// スーパーバイザモード 最適化防止にラベルを付ける

	if (argc < 2){
		exit(1);
	}


	if (argc >= 3){
		no = atoi(argv[2]);
		if((no % 256) > 9)
			no = 0;
	}

	if (argc >= 4){
		noise = atoi(argv[3]);
	}
}
*/
void load_fmdbgm(char *filename)
{
//	if(bload2(argv[1], 0x1000*0) == ERROR)
	if(bload2(filename, 0x1000*0) == ERROR)
		exit(1);
}

void play_fmdbgm(void)
{
	unsigned short no = 0;
	unsigned char i, ch = 0;
	unsigned char noise = 0;

	FEEDVOL = 0;
	FCOUNT = 1;
	STOPPARTS = 0;
	ENDFRG = 0;
	NSAVE = 0;
	for(i = 0; i < PARTSUU; ++i){
		int j = 0xdb00-0x1100 + i * 2 + (no % 256) * 12 * 2;
		COUNT[i] = 1;
		STARTADR[i] = mem[j] + mem[j+1] * 256;
		if(!(STARTADR[i])){
			STOPFRG[i] = 255;
			MAXVOL[i] = 255;
			STOPPARTS++;
		}else{
			MAXVOL[i] = 0;
			STOPFRG[i] = 0;
		}
		STARTADR[i] -= 0x1000;
		OFFSET[i] = STARTADR[i];
		LENGTH[i] = 5;
		set_tone(noise, i);
	}
	LOOPTIME = 0; //no / 256;

	set_fm(0x12, 191);

	if(init_sndint())
		exit(1);
	set_fm(0x14, 0x2a);
}
//	getchar();

void stop_fmdbgm(void)
{
	stop();
	reset_sndint();

//	exit(0);
}
