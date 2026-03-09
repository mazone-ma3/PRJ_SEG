#ifndef _FMDTWG_H_INCLUDE_
#define _FMDTWG_H_INCLUDE_

#define BIOS

int load_fmdbgm(char *filename);
void play_fmdbgm(void);
void stop_fmdbgm(void);

#ifdef BIOS
#define BIOSVOL
#include "snd.h"
extern char sndwork[SndWorkSize];
#endif

#endif
