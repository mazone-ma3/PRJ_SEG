/* MSX-SC5 Loader for ZSDCC(MSX-DOS) By m@3 */

#include <stdio.h>
#include <stdlib.h>
#include <conio.h>

#define ERROR 1
#define NOERROR 0

#define WIDTH 32
#define LINE 212

enum {
	VDP_READDATA = 0,
	VDP_READSTATUS = 1
};

enum {
	VDP_WRITEDATA = 0,
	VDP_WRITECONTROL = 1,
	VDP_WRITEPAL = 2,
	VDP_WRITEINDEX = 3
};

#define VDP_readport(no) (VDP_readadr + no)
#define VDP_writeport(no) (VDP_writeadr + no)

unsigned char VDP_readadr;
unsigned char VDP_writeadr;

#define MAXCOLOR 16

FILE *stream[1];

long i, count, count2, n = 0;
short m = 0;
unsigned char read_pattern[WIDTH * LINE * 2 + 2];
unsigned char pattern[10];

#define oldscr ((volatile unsigned char *)0xfcb0)

/* screenのBIOS切り替え */
void set_screenmode(unsigned char mode) __sdcccall(1)
{
__asm
;	ld	 hl, 2
;	add	hl, sp
	ld	l,a
	push	ix

	ld	a,(#0xfcc1)	; exptbl
	ld	d,a
	ld	e,0
	push	de
	pop	iy
	ld ix,#0x005f	; CHGMOD(MAINROM)

;	ld	a, (hl)	; a = mode
	ld	a,l

	call	#0x001c	; CALSLT
	pop	ix
__endasm;
}

/* mainromの指定番地の値を得る */
unsigned char read_mainrom(unsigned short adr) __sdcccall(1)
{
__asm
;	ld	 hl, #2
;	add	hl, sp
;	ld	e, (hl)
;	inc	hl
;	ld	d, (hl)	; de=adr
;	ld	h,d
;	ld	l,e	; hl=adr

	ld	a,(#0xfcc1)	; exptbl
	call	#0x000c	; RDSLT

;	ld	l,a
;	ld	h,#0
	ld	e,a
__endasm;
}

void write_VDP(unsigned char regno, unsigned char data) __sdcccall(1)
{
//	outp(VDP_writeport(VDP_WRITECONTROL), data);
//	outp(VDP_writeport(VDP_WRITECONTROL), 0x80 | regno);
__asm
	ld	h,a
	ld	a,(_VDP_writeadr)
	inc	a
	ld	c,a
	ld	a,l
	out	(c),a
	ld	a,h
	set 7,a
	out	(c),a
__endasm;
}

void write_vram_adr(unsigned char highadr, int lowadr) __sdcccall(1)
{
__asm
	push	de
__endasm;
	write_VDP(14, (((highadr  << 2) & 0x0c) | (lowadr >> 14) & 0x03));	// V9968
__asm
	pop	de
	ld	a,(_VDP_writeadr)
	inc	a
	ld	c,a
	out	(c),e
	ld	a,d
	and	a,0x3f
	set	6,a
	out	(c),a
__endasm;
//	outp(VDP_writeport(VDP_WRITECONTROL), (lowadr & 0xff));
//	outp(VDP_writeport(VDP_WRITECONTROL), 0x40 | ((lowadr >> 8) & 0x3f));
}

void write_vram_data(unsigned char data) __sdcccall(1)
{
	outp(VDP_writeport(VDP_WRITEDATA), data);
}

void set_displaypage(int page) __sdcccall(1)
{
__asm
	DI
__endasm;
	write_VDP(2, (page << 5) & 0xe0 | 0x1f);
__asm
	EI
__endasm;
}

/*終了処理*/
void end(void)
{
/*__asm
	xor	a
	ld	c,0
	call	0005h
__endasm;*/
}

unsigned short vram_start_adr, vram_end_adr;
unsigned char page = 0;
unsigned char mode = 0;
unsigned char mode2 = 0;

int conv(char *loadfil)
{
	if ((stream[0] = fopen( loadfil, "rb")) == NULL) {
		printf("Can\'t open file %s.", loadfil);

//		fclose(stream[0]);
//		end();
//		exit(1);
		return ERROR;
	}

	fread(pattern, 1, 1, stream[0]);	/* MSX先頭ヘッダ */
	if(pattern[0] != 0xfe){
		printf("Not BSAVE,S file %s.", loadfil);
		fclose(stream[0]);
//		end();
//		exit(1);
		return ERROR;
	}

	if(page >= 8){
		VDP_readadr = 0x88;
		VDP_writeadr = 0x88;
	}else{
		VDP_readadr = read_mainrom(0x0006);
		VDP_writeadr = read_mainrom(0x0007);
	}

	if(page < 8)
		set_screenmode(5);

	if(page >= 8){
		write_VDP(0, 0x06); // Mode 0
//		write_VDP(1, 0x60); // Mode 1 IE=1
		write_VDP(1, 0x40); // Mode 1 IE=0
		write_VDP(8, 0x0a); // Mode 2
		write_VDP(9, 0x08); // Mode 3

		write_VDP(2, 0x1f); // Pattern name table base address register
		write_VDP(5, 0xef); // Sprite attibute table base adderss register
		write_VDP(11, 0x00); // Sprite attibute table base adderss register
		write_VDP(6, 0x0f); // Sprite pattern generatorable base adderss register
//		write_VDP(7, 0x02); // Back drop color register
	}

	write_VDP(20, 0xef);	/* V9968拡張 */


	fread(pattern, 1, 2, stream[0]);	/* MSXヘッダ 開始アドレス */
	vram_start_adr = pattern[0] + pattern[1] * 256;

	fread(pattern, 1, 2, stream[0]);	/* MSXヘッダ 終了アドレス */
	vram_end_adr = pattern[0] + pattern[1] * 256;

	fread(pattern, 1, 2, stream[0]);	/* MSXヘッダ 0 */

	switch(page % 8){
		case 0:
			write_vram_adr(0x00, vram_start_adr);
			break;
		case 1:
			write_vram_adr(0x00, vram_start_adr + 0x8000);
			break;
		case 2:
			write_vram_adr(0x01, vram_start_adr);
			break;
		case 3:
			write_vram_adr(0x01, vram_start_adr + 0x8000);
			break;
		case 4:
			write_vram_adr(0x02, vram_start_adr);
			break;
		case 5:
			write_vram_adr(0x02, vram_start_adr + 0x8000);
			break;
		case 6:
			write_vram_adr(0x03, vram_start_adr);
			break;
		case 7:
			write_vram_adr(0x03, vram_start_adr + 0x8000);
			break;
	}
//	if(page < 4)
	if(mode)
		set_displaypage(page % 8);
	else
		set_displaypage(0);

	n = vram_start_adr;
	for(count = 0; count < 4; ++count){
		i = fread(read_pattern, 1, WIDTH * LINE, stream[0]);
		if(i < 1)
			break;
		m = 0;
		for(count2 = 0; count2 < WIDTH * LINE; ++count2){
			write_vram_data(read_pattern[m]);
			if(n == vram_end_adr){
				fclose(stream[0]);
				return NOERROR;
			}
			++m;
			++n;
		}
	}
	fclose(stream[0]);

	write_VDP(20, 0x0);

	if(mode)
//		if(!mode2)
			getch();

	if(page < 8)
		set_screenmode(*oldscr);

	return NOERROR;
}

int	main(int argc,char **argv){
//	printf("%d\n", argc);

	if (argc < 2){ //argv[1] == NULL){
		printf("MSX .SC5 file Loader for MSX2.\n");
//		exit(1);
		return ERROR;
	}

	if (argc >= 3){ //argv[2] != NULL){
		page = atoi(argv[2]);
//		if(page >= 8){
//			mode2 = page / 8;
//			page %= 8;
//		}
/*		else{
			printf("page: %d\n",page);
			getch();
		}
*/	}

	if(argc <= 3){
//	if(argv[3] == NULL)
		mode = 1;
	}

	if(conv(argv[1]))
//		end();
//		exit(1);
		return ERROR;

	end();

	return NOERROR;
}
