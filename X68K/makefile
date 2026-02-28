# PRJ_SEG.X makefile for X680x0 GCC (elf2x68k) (XSP選択式)

# --- 設定 ---
TARGET      = PRJ_SEG
CC          = m68k-xelf-gcc
AS          = m68k-xelf-as

# X68000(68000)ターゲット、最適化、文字コード変換（SJIS出力）
CFLAGS      = -m68000 -O2 -finput-charset=utf-8 -fexec-charset=cp932
# リンクオプション：0x6800番地開始、シンボル削除、数学ライブラリ
LDFLAGS     = -Ttext=0x6800 -s

# 中間ファイル
XSPLIBFILES = xsp2lib.o pcm8afnc.o
OBJS        = SP_INIT.o SUBFUNC.o SP_MAIN.o SP_LD.o PLAY.o FONT68.o FMD68.o RASTER.o

# --- ルール ---

all: $(TARGET).X

# 1. パターンルール
%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

%.o: %.s
	$(AS) -o $@ $<

# 2. リンク工程
$(TARGET).X: $(OBJS)
	$(CC) $(LDFLAGS) -o $@ $(OBJS) -lm

# 3. (おまけ) XSP対応にする場合
xsp:  $(OBJS)
	$(CC) $(LDFLAGS) -o $(TARGET)_XSP.X $(OBJS) $(XSPLIBFILES) -lm

# --- 依存関係 ---
#$(TARGET).o: $(TARGET).c inkey.h
SP_INIT.o : SP_INIT.c SP.h SP_LD.h PLAY.h common.h FMD68.h

SP_LD.o : SP_LD.c SP_LD.h sp.h common.h

PLAY.o : PLAY.c PLAY.h

SUBFUNC.o : SUBFUNC.c SP.h common.h

SP_MAIN.o : SP_MAIN.c SP.h SPR_DEF.h SP_COM.h FONT68.h common.h SP_ENE.h SP_SHT.h

FONT68.o : FONT68.c FONT68.h

FMD68,o : FMD68.c FMD68.h key.h keyx68.h tone.h

RASTER.o : RASTER.s

clean:
	rm -f $(TARGET).o $(TARGET).X
