#ifndef SG_INIT_H_INCLUDE
#define SG_INIT_H_INCLUDE

//void clear(unsigned short);
void sys_wait(unsigned char wait);
void pal_set(int, unsigned short,unsigned short,unsigned short,unsigned short);
void pal_all(int, unsigned char[16][3]);
void set_pal_reverse(int pal_no, unsigned char  pal[16][3]); //WORD far p[16])
void set_constrast(int value, unsigned char org_pal[16][3], int pal_no);
void fadeinblack(unsigned char org_pal[16][3], int pal_no, int wait);
void fadeoutblack(unsigned char org_pal[16][3], int pal_no, int wait);
void fadeoutwhite(unsigned char org_pal[16][3], int pal_no, int wait);
void pal_allblack(int pal_no);

extern unsigned char org_pal[16][3];

#define SND_BUFFSIZE 55000

extern char SNDBUFF[4][SND_BUFFSIZE];
extern char msv_se[4][256];

#endif
