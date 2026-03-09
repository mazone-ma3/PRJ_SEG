#ifndef FONTTW_H_INCLUDE
#define FONTTW_H_INCLUDE

#define MSXWIDTH 256
#define MSXLINE 212
#define SPRSIZEX 8
#define SPRSIZEY 16
//#define SPRPARTS 256

short font_load(char *loadfil, unsigned short sprparts);
short title_load(char *loadfil, unsigned short sprparts, long max_xx);

#endif
