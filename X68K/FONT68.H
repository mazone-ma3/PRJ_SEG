#ifndef FONTTW_H_INCLUDE
#define FONTTW_H_INCLUDE

short font_load(char *loadfil, short offset, short pcgparts);
short title_load(char *loadfil, short x, short y, short msxline);
short title_load2(char *loadfil, unsigned short *vram_offset, short msxline);

#endif
