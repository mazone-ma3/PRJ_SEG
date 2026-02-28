/* PLAY MCDRV */

#include <stdio.h>

#include "sp.h"
#include "play.h"

#define FILE_MAX 1 /* 10 */

FILE *play_stream[FILE_MAX];


void  cursor_off(void)
{
	register long rd0 asm ("d0");

	rd0 = 0xaf;

	asm volatile (
			"trap #15\n"
			://"=d"(rd0)	/* 値が返るレジスタ変数 */
			:"d"(rd0)	/* 引数として使われるレジスタ変数 */
//			:"d0"		/* 破壊されるレジスタ */
	);
	return;
}


long mcd_release(void)
{
	register long rd0 asm ("d0");

	rd0 = MCD_RELEASE;

	asm volatile (
			"trap #4"
			:"=d"(rd0)	/* 値が返るレジスタ変数 */
			:"d"(rd0)	/* 引数として使われるレジスタ変数 */
//			:"d0"		/* 破壊されるレジスタ */
	);

	/*printf("Release=%d\n",rd0);*/
/*	cursor_off();*/
	return rd0;
}

void  mcd_fadeout(void)
{
	register long rd0 asm ("d0");
	register short rd1 asm ("d1");

	rd0 = MCD_FADEOUT;
	rd1 = 0x05;

	asm volatile (
			"trap #4"
			:"=d"(rd0)	/* 値が返るレジスタ変数 */
			:"d"(rd0),"d"(rd1)	/* 引数として使われるレジスタ変数 */
			: //"d0"		/* 破壊されるレジスタ */
	);
	return;
}


void mcd_stop(void)
{
	register long rd0 asm ("d0");

	rd0 = MCD_STOPMUSIC;

	asm volatile (
			"move.l #0x05,%0\n"
			"trap #4"
			:// "=d"(rd0)	/* 値が返るレジスタ変数 */
			: "d"(rd0)	/* 引数として使われるレジスタ変数 */
			: //"d0"		/* 破壊されるレジスタ */
	);

	return;
}


long mcd_play(void)
{
	register long rd0 asm ("d0");

	rd0 = MCD_PLAYMUSIC;

	asm volatile (
			"move.l #0x02,%0\n"
			"trap #4"
			://"=d"(rd0)	/* 値が返るレジスタ変数 */
			:"d"(rd0)	/* 引数として使われるレジスタ変数 */
			://"d0"		/* 破壊されるレジスタ */
	);

	/*printf("Play=%d\n",rd0);*/
/*	cursor_off();*/
	return 0; //rd0;
}

void mcd_getplayflg(long *part1, long *part2)
{
	register long rd0 asm ("d0");
	register long rd1 asm ("d1");

	rd0 = MCD_GETPLAYFLG;

	asm volatile (
			"trap #4"
			:"=d"(rd0),"=d"(rd1)	/* 値が返るレジスタ変数 */
			:"d"(rd0),"d"(rd1)	/* 引数として使われるレジスタ変数 */
//			:"d0","d1"		/* 破壊されるレジスタ */
	);

	*part1 = rd0;
	*part2 = rd1;
}

short mcd_setfadelvl(short fadelevel)
{
	register long rd0 asm ("d0");
	register long rd1 asm ("d1");

	rd0 = MCD_SETFADELVL;
	rd1 = fadelevel;

	asm volatile (
			"trap #4"
			:"=d"(rd0)	/* 値が返るレジスタ変数 */
			:"d"(rd0),"d"(rd1)	/* 引数として使われるレジスタ変数 */
//			:"d0","d1"		/* 破壊されるレジスタ */
	);
	return rd0;
}


enum {
	str1 = '-MCD',
	str2 = 'RV0-'
};

long  check_mcd(void)
{
	register long rd0 asm("d0");
	register long ra0 asm("a0");
	register long rsp asm("sp");

	asm volatile(
			"move.l %2,-(%1)\n"
			"moveq.l #-1,%0\n"
			"movea.l 0x90.w,%2\n"
			"cmpa.l #0xffffff,%2\n"
			"bhi keepcheck90\n"
			"cmpi.l %3,-12(%2)\n"
			"bne keepcheck90\n"
			"cmpi.l %4,-8(%2)\n"
			"bne keepcheck90\n"
			"move.l -(%2),%0\n"
		"keepcheck90:\n"
			"tst.l %0\n"
			"movea.l (%1)+,%2\n"
			:"=r"(rd0),"+r"(rsp),"+r"(ra0)
			:"i"(str1),"i"(str2)
//			:"d0","a0"
	);

/*	printf("Check=%d\n",rd0);*/
/*	cursor_off();*/
	return rd0;
//	return 0;
}


long mcd_load(char *fil, unsigned char *playbuffer, int max_mcd_size)
{
	register long *ra0 asm ("a0");
	register long rd0 asm ("d0");
	register long rd1 asm ("d1");

	long size;

	if ((play_stream[0] = fopen( fil, "rb")) == NULL) {
		fprintf(stderr, "Can\'t open file %s.", fil);

		fclose(play_stream[0]);
		return 1;
	}
/*	printf("Loading file:%s\n" ,fil);*/

	size = fread(playbuffer, 1, max_mcd_size, play_stream[0]);
	fclose(play_stream[0]);

/*	printf("Size:%d\n" ,size);*/

	rd0 = MCD_TRANSMDC;
	rd1 = size;
	ra0 = (long *)playbuffer;

	asm volatile (
			"trap #4"
			:"=d"(rd0)	/* 値が返るレジスタ変数 */
			:"d"(rd0),"d"(rd1),"a"(ra0)	/* 引数として使われるレジスタ変数 */
			://"d0"		/* 破壊されるレジスタ */
	);

/*	printf("Code:%d\n" ,rd0);*/
	return rd0;
}

long pcm_load(char *fil, unsigned char *pcmbuffer, int max_pcm_size)
{
	register long *ra0 asm ("a0");
	register long rd0 asm ("d0");
	register long rd1 asm ("d1");

	long size;

	if ((play_stream[0] = fopen( fil, "rb")) == NULL) {
		fprintf(stderr, "Can\'t open file %s.", fil);

		fclose(play_stream[0]);
		return 1;
	}
/*	printf("Loading file:%s\n" ,fil);*/

	size = fread(pcmbuffer, 1, max_pcm_size, play_stream[0]);
	fclose(play_stream[0]);

/*	printf("Size:%d\n" ,size);*/

	rd0 = MCD_TRANSPCM;
	rd1 = size;
	ra0 = (long *)pcmbuffer;

	asm volatile (
			"trap #4"
			:"=d"(rd0)	/* 値が返るレジスタ変数 */
			:"d"(rd0),"d"(rd1),"a"(ra0)	/* 引数として使われるレジスタ変数 */
//			:"d0"		/* 破壊されるレジスタ */
	);

/*	printf("Code:%d\n" ,rd0);*/
	return rd0;
}


void pcm_play(unsigned char *pcmbuffer, int pcm_size)
{
	register long *ra0 asm ("a0");
	register long rd0 asm ("d0");
	register long rd1 asm ("d1");
	register long rd2 asm ("d2");

	rd0 = MCD_PLAYADPCM;
	rd1 = 4 * 256 + 3;
	rd2 = pcm_size;
	ra0 = (long *)pcmbuffer;

	asm volatile (
			"trap #4"
			:"=d"(rd0)	/* 値が返るレジスタ変数 */
			:"d"(rd0),"d"(rd1),"d"(rd2),"a"(ra0)	/* 引数として使われるレジスタ変数 */
//			:"d0"		/* 破壊されるレジスタ */
	);
}