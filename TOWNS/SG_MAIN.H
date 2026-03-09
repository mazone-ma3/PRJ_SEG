#ifndef SG_MAIN_H_INCLUDE
#define SG_MAIN_H_INCLUDE

#define CHRPAL_NO 0
#define REVPAL_NO 1
#define BGPAL_NO 2

void bg_roll(void);

short game_run(short);

void wait_vsync(void);
void wait_vsync2(void);
void wait_sprite(void);
int opening_demo(void);
int title_demo(void);
void init_star(void);
void set_sprite(void);
void put_titlelogo(short x, short y);
void score_displayall(void);
void set_sprite(void);
void init_chr_data(void);
int init_vsync(void);
void reset_vsync(void);

unsigned char keyscan(void);

extern unsigned short title_index;
extern unsigned int encode;

extern int spr_count, old_count;

#endif