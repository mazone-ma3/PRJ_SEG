# PRJ_SEG.EXP makefile for FM TOWNS GCC (GCCクロスコンパイル開発環境)

# --- 設定 ---
TARGET       = PRJ_SEG
#GCCROOT      = C:\tgcc
GCCROOT      = c:\winap\djgpp12.2.0
GCCROOT2     = C:\tgcc

BIN          = $(GCCROOT)\bin
BIN2         = $(GCCROOT2)\bin
LIB_PATH     = $(GCCROOT2)\lib
INC_PATH     = $(GCCROOT2)\include

# ツール定義
#CC           = $(BIN2)\gcc
CC           = $(BIN)\i586-pc-msdosdjgpp-gcc
AS           = $(BIN2)\as
LD           = $(BIN2)\ld_towns
GENEXP       = $(BIN2)\genexp3

# フラグ定義
CCFLAGS      = -O1 -msoft-float -Ic:\tgcc\include  -S
LDFLAGS      = -N -S -L$(LIB_PATH) -lTN -lce -lme -lpc -lt

# 中間ファイル
OBJS          = SG_INIT.o SUBFUNC.o SG_MAIN.o SP68_LD.o FONTTW.o FMDTWG.o VSYNC.o

CRT0         = $(LIB_PATH)\crt0.o

# --- ルール ---

all: $(TARGET).EXP

# 1. パターンルール（.c -> .o）
# 中間ファイル .i や .s を自動で処理する流れを定義
%.o: %.c
	$(CC) $< $(CCFLAGS)
	sed -i 's/\t\.section\s\+//' $*.s
	sed -i 's/\.lcomm \([^,]*\),\([^,]*\),[0-9]*/\.lcomm \1, \2/' $*.s
	$(AS)  -o $*.o $*.s
	@rm -f $*.s

# 2. パターンルール（.s -> .o）
%.o: %.s
	$(AS) -o $@ $<

# 3. リンク（.bin生成）
$(TARGET).bin: $(OBJS)
	$(LD) -o $@  $(CRT0) $(OBJS) $(LDFLAGS)

# 4. EXPファイル変換
$(TARGET).EXP: $(TARGET).bin
	$(GENEXP) $< $@

# --- 依存関係 ---
#$(TARGET).o: $(TARGET).c

SG_INIT.o : SG_INIT.c SG.H

#SPMAKE.o : SPMAKE.c SPMAKE.H

#PLAY.o : PLAY.c PLAY.H

SUBFUNC.o : SUBFUNC.c SUBFUNC.H SG.H

SP68_LD.o : SP68_LD.c SP68_LD.H SG.H

SG_MAIN.o : SG_MAIN.c SG_MAIN.H SG.H SPR_DEF.H SG_COM.H SG_SHT.H FONTTW.H FMDTWG.h

FONTTW.o : FONTTW.c FONTTW.H

FMDTWG,o : FMDTWG.c FMDTWG.h key.h keyxtow.h tone.h

VSYNC.o : VSYNC.s

clean:
	rm -f *.o *.bin *.EXP
