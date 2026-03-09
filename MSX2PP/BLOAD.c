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

void write_VDP(unsigned char regno, unsigned char data)
{
	outp(VDP_writeport(VDP_WRITECONTROL), data);
	outp(VDP_writeport(VDP_WRITECONTROL), 0x80 | regno);
}

void write_vram_adr(unsigned char highadr, int lowadr) __sdcccall(1)
{
	write_VDP(14, (((highadr  << 2) & 0x04) | (lowadr >> 14) & 0x03));
	outp(VDP_writeport(VDP_WRITECONTROL), (lowadr & 0xff));
	outp(VDP_writeport(VDP_WRITECONTROL), 0x40 | ((lowadr >> 8) & 0x3f));
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
	write_VDP(2, (page << 5) & 0x60 | 0x1f);
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

	set_screenmode(5);
	VDP_readadr = read_mainrom(0x0006);
	VDP_writeadr = read_mainrom(0x0007);

	fread(pattern, 1, 2, stream[0]);	/* MSXヘッダ 開始アドレス */
	vram_start_adr = pattern[0] + pattern[1] * 256;

	fread(pattern, 1, 2, stream[0]);	/* MSXヘッダ 終了アドレス */
	vram_end_adr = pattern[0] + pattern[1] * 256;

	fread(pattern, 1, 2, stream[0]);	/* MSXヘッダ 0 */

	switch(page){
		case 1:
			write_vram_adr(0x00, vram_start_adr + 0x8000);
			break;
		case 2:
			write_vram_adr(0x01, vram_start_adr);
			break;
		case 3:
			write_vram_adr(0x01, vram_start_adr + 0x8000);
			break;
		default:
			write_vram_adr(0x00, vram_start_adr);
			break;
	}
	if(page < 4)
		if(mode)
			set_displaypage(page);

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

	return NOERROR;
}

unsigned char *oldscr = 0xfcb0;

int	main(int argc,char **argv){
//	printf("%d\n", argc);

	if (argc < 2){ //argv[1] == NULL){
		printf("MSX .SC5 file Loader for MSX2.\n");
//		exit(1);
		return ERROR;
	}

	if (argc >= 3){ //argv[2] != NULL){
		page = atoi(argv[2]);
		if(page > 3)
			page = 0;
/*		else{
			printf("page: %d\n",page);
			getch();
		}
*/	}

	if(argc < 4)
//	if(argv[3] == NULL)
		mode = 1;

	if(conv(argv[1]))
//		end();
//		exit(1);
		return ERROR;

	if(mode)
		getch();

	set_screenmode(*oldscr);
	end();

	return NOERROR;
}
