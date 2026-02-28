#ifndef SP_MAIN_H_INCLUDE
#define SP_MAIN_H_INCLUDE

#define CHRPAL_NO 0
#define REVPAL_NO 1
#define BGPAL_NO 2

void bg_roll(void);

short game_run(short);

void wait_vsync(void);
int opening_demo(void);
int title_demo(void);
void init_star(void);
void set_sprite(void);
void put_titlelogo(short x, short y);
void score_displayall(void);
void set_sprite(void);

void put_strings(int scr, int x, int y,  char *str, char pal);
void hiscore_display(void);
unsigned char keyscan(void);

extern unsigned short title_index;

extern int spr_count;

#endif
