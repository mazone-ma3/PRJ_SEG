#ifndef SUBFUNC_H_INCLUDE
#define SUBFUNC_H_INCLUDE

extern char egb_work[1536];

void grp_set(void);
void spr_set(int);
void spr_fill(int);
void spr_fill2(int);
void spr_clear(void);
void spr_make(void);

void pad_read(int , int *, int *, int *);

void spr_on(int);
void spr_off(void);

void grp_term(void);

#endif
