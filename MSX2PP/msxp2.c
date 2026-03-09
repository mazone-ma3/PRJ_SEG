/* MSX FM呼び出し実験 By m@3 */
/* .COM版 */
/*  fmdmsx.objファイルと演奏ファイルを用意してください */
/* ZSDCC版 */

#include <stdio.h>
#iinclude <stdlib.h>
#include <conio.h>

FILE *stream[2];

#define ERROR 1
#define NOERROR 0

enum {
	WORK_NO,
	WORK_LOOP,
	WORK_NOISE,
	WORK_MIX,
	WORK_TONE,
	WORK_PART,
	WORK_SLOT
};

void DI(void){
__asm
	DI
__endasm;
}

void EI(void){
__asm
	EI
__endasm;
}

/* MSX BLOADデータをファイルからメモリに読み込む */
short msxload(char *loadfil, unsigned short offset)
{
	unsigned short size;
	unsigned char *address;
	unsigned char buffer[2];

	if ((stream[0] = fopen( loadfil, "rb")) == NULL) {
		printf("Can\'t open file %s.", loadfil);
		return ERROR;
	}

	fread( buffer, 1, 1, stream[0]);
	fread( buffer, 1, 2, stream[0]);
	address = (unsigned short *)(buffer[0] + buffer[1] * 256);
	fread( buffer, 1, 2, stream[0]);
	size = (buffer[0] + buffer[1] * 256) - (unsigned short)address;
	fread( buffer, 1, 2, stream[0]);
	address -= offset;
	printf("Load file %s. Address %x Size %x End %x\n", loadfil, address, size, (unsigned short)address + size);

	fread( address , 1, size, stream[0]);
	fclose(stream[0]);
	return NOERROR;
}

short bload(char *loadfil)
{
	unsigned short size;
	unsigned short *address;
	unsigned char buffer[2];

	if ((stream[0] = fopen( loadfil, "rb")) == NULL) {
		printf("Can\'t open file %s.", loadfil);
		return ERROR;
	}
	fread( buffer, 1, 2, stream[0]);
	address = (unsigned short *)(buffer[0] + buffer[1] * 256);
	fread( buffer, 1, 2, stream[0]);
	size = (buffer[0] + buffer[1] * 256) - (unsigned short)address;
	printf("Load file %s. Address %x Size %x End %x\n", loadfil, address, size, (unsigned short)address + size);

	fread( address, 1, size, stream[0]);
	fclose(stream[0]);
	return NOERROR;
}

short bload2(char *loadfil, unsigned short offset)
{
	unsigned short size;
	unsigned char *address;
	unsigned char buffer[2];

	if ((stream[0] = fopen( loadfil, "rb")) == NULL) {
		printf("Can\'t open file %s.", loadfil);
		return ERROR;
	}
	fread( buffer, 1, 2, stream[0]);
	address = (unsigned short *)(buffer[0] + buffer[1] * 256);
	fread( buffer, 1, 2, stream[0]);
	size = (buffer[0] + buffer[1] * 256) - (unsigned short)address;
	address -= offset;
	printf("Load file %s. Address %x Size %x End %x\n", loadfil, address , size, (unsigned short)address + size);

	fread( address , 1, size, stream[0]);
	fclose(stream[0]);
	return NOERROR;
}

char read_slot(unsigned char slot, unsigned short adr) __sdcccall(1)
{
__asm
;	push	ix
	push	bc
	push	de
	push	de
	pop	hl
	call	#0x000c	; RDSLT
	pop	de
	pop	bc
;	pop	ix
__endasm;
}

void enable_opll(unsigned char slot) __sdcccall(1)
{
__asm
	push	iy
	push	ix
	push	bc
	push	de
	push	hl
	ld	hl,#08000h
	ld	d,a
	ld	e,0
	push	de
	pop	iy
	ld ix,#0x04113	; INIOPL

	call	#0x001c	; CALSLT
	pop	hl
	pop	de
	pop	bc
	pop	ix
	pop	iy
__endasm;
}

#define exptbl_pointer ((volatile unsigned char *)0xfcc1)

char slot_chr[5];
unsigned char i, j, k, l, opll_mode = 0, cpu, opll_slot = 0;
unsigned char *mem;

void play_bgm(int no, int noise)
{
	mem =(unsigned char *)0xbb09;
	mem[WORK_NO] = no % 256;
	mem[WORK_LOOP] = no / 256;
//	mem[WORK_MIX] = noise;
	mem[WORK_TONE] = noise % 256;
	if(noise / 256)
		mem[WORK_PART] = 12;
	else
		mem[WORK_PART] = 6;

__asm
	DI
__endasm;
	for(i = 0; i < 4; ++i){
		if(!exptbl_pointer[i]){
			printf("%d basic slot. \n", i);
			k = i;
			slot_chr[4] = '\0';
			for(l = 0; l < 4; ++l){
				slot_chr[l] = read_slot(k, 0x401c + l);
			}
			if(!strcmp(slot_chr, "OPLL")){
				printf("found ");
				for(l = 0; l < 4; ++l){
					slot_chr[l] = read_slot(k, 0x4018 + l);
				}
				if(!strcmp(slot_chr, "APRL")){
					printf("internal ");
					opll_mode = 1;
					opll_slot = k;
				}else{
					printf("external ");
					if(!opll_mode){
						opll_mode = 2;
						opll_slot = k;
					}
				}
				printf("OPLL basic slot %d \n", i);
			}
		}else{
			printf("%d expand slot. \n", i);
			for(j = 0; j < 4; ++j){
				k = j * 4 + i | 0x80;
				slot_chr[4] = '\0';
				for(l = 0; l < 4; ++l){
					slot_chr[l] = read_slot(k, 0x401c + l);
				}
				if(!strcmp(slot_chr, "OPLL")){
					printf("found ");
					for(l = 0; l < 4; ++l){
						slot_chr[l] = read_slot(k, 0x4018 + l);
					}
					if(!strcmp(slot_chr, "APRL")){
						printf("internal ");
						opll_mode = 1;
						opll_slot = k;
					}else{
						printf("external ");
						if(!opll_mode){
							opll_mode = 2;
							opll_slot = k;
						}
					}
					printf("OPLL expand slot %d - %d \n", i, j);
				}
			}
		}
	}
	if(!opll_mode){
		printf("OPLL not found.\n");
		mem[WORK_SLOT] = 0xff;
	}else if(opll_mode == 1){
		enable_opll(opll_slot);
		mem[WORK_SLOT] = opll_slot;
	}else if(opll_mode == 2){
		enable_opll(opll_slot);
		printf("OPLL ON");
		mem[WORK_SLOT] = opll_slot;
	}
__asm
	EI
__endasm;

__asm
	call #0xbb00
__endasm;
}

void stop_bgm(void)
{
__asm
	call #0xbb03
	EI
__endasm;
}

void fade_bgm(void)
{
__asm
	call #0xbb06
__endasm;
}



int	main(int argc,char **argv)
{
	int no = 0;
	int noise = 1; //0x38
	if (argc < 2){
		printf("FM+PSGMSX Loader.\n");
		return ERROR;
	}
	if (argc >= 3){ //argv[2] != NULL){
		no = atoi(argv[2]);
		if((no % 256) > 9)
			no = 0;
/*		else{
			printf("no: %d\n",no);
			getch();
		}
*/	}

	if (argc >= 4){ //argv[2] != NULL){
		noise = atoi(argv[3]);
		if(!noise)
			noise = 1;
//		if(noise > 255)
//			noise = 0xc8;
/*		else{
			printf("no: %d\n",no);
			getch();
		}
*/	}

	if(msxload("psgtone.dat", 0x2200*1) == ERROR)
		return ERROR;

	if(msxload("keymsx.msx", 0x2100*1) == ERROR)
		return ERROR;

	if(msxload("fmdmsx2.msx", 0) == ERROR)
		return ERROR;

	if(msxload(argv[1], 0x2100*1) == ERROR)
		return ERROR;

	EI();

	play_bgm(no, noise);
	if(getchar() != 0x1b)
		stop_bgm();
	else
		fade_bgm();

/*__asm
	xor	a
	ld	c,0
	call	0005h
__endasm;*/

	return NOERROR;
}

