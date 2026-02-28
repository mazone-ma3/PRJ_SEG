#ifndef PLAY_H_INCLUDE
#define PLAY_H_INCLUDE

#define MCD_RELEASE			0x00
#define MCD_TRANSMDC		0x01
#define MCD_PLAYMUSIC		0x02
#define MCD_TRANSPCM		0x03
#define MCD_PAUSEMUSIC		0x04
#define MCD_STOPMUSIC		0x05
#define MCD_GETWORKADR		0x06
#define MCD_GETVCTTBLADR	0x08
#define MCD_GETKEYONPTR		0x09
#define MCD_GETCURDATAPTR	0x0a
#define MCD_GETPLAYFLG		0x0b
#define MCD_SETTRANSPOSE	0x0c
#define MCD_GETLOOPCOUNT	0x0d
#define MCD_GETNOWCLOCK		0x0e
#define MCD_GETTITLE		0x0f
#define MCD_GETCOMMENT		0x10
#define MCD_INTEXEC			0x11
#define MCD_SETSUBEVENT		0x12
#define MCD_UNREMOVE		0x13
#define MCD_FADEOUT			0x14
#define MCD_SETPARAM		0x15
#define MCD_GETTEMPO		0x16
#define MCD_GETPASSTIME		0x17
#define MCD_SKIPPLAY		0x18
#define MCD_SETADPCMNAME	0x1c
#define MCD_GETADPCMNAME	0x1d
#define MCD_CMPADPCMNAME	0x1e
#define MCD_GETTRWORKSIZE	0x1f
#define MCD_PLAYSE			0x20
#define MCD_STOPSE			0x21
#define MCD_SETTEMPOSNS		0x22
#define MCD_SETMASTERVOL	0x23
#define MCD_SETFADELVL		0x24
#define MCD_SETMIDIIF		0x25
#define MCD_TRANSMIDIDATA	0x26
#define MCD_MCMLINK			0x27
#define MCD_MCMUNLK			0x28
#define MCD_PURGE			0x29
#define MCD_SRCHMEAS		0x2a
#define MCD_JUMPPLAY		0x2b
#define MCD_PLAYSE2			0x2c
#define MCD_STOPSE2			0x2d
#define MCD_PLAYADPCM		0x2e
#define MCD_PLAYADPCM2		0x2f
#define MCD_SEFADEOUT		0x30


long mcd_release(void);

void mcd_fadeout(void);

long mcd_play(void);
void mcd_getplayflg(long *part1, long *part2);
short mcd_setfadelvl(short fadelevel);

void mcd_stop(void);

long  check_mcd(void);

long mcd_load(char *fil, unsigned char *playbuffer, int max_mcd_size);
long pcm_load(char *fil, unsigned char *pcmbuffer, int max_pcm_size);

void pcm_play(unsigned char *pcmbuffer, int pcm_size);

#endif
