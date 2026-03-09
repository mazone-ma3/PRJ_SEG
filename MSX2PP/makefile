# PRJ_SEG.COM makefile for MSX2++ ZSDCC (z88dk)

# --- 設定 ---
TARGET   = PRJ_SEG
CC       = zcc

# コンパイルオプション（SDCCを使用）
#CFLAGS   = +msx -compiler sdcc --opt-code-speed --sdcccall0
CFLAGS   = +cpm -compiler sdcc --opt-code-size --sdcccall0
# リンクオプション（subtypeで切り替え）
#LDFLAGS  = +msx -create-app -lmsxbios
LDFLAGS = +cpm -create-app

# 中間ファイル
OBJS     = $(TARGET).o

# --- ルール ---

all: $(TARGET).com

# 1. パターンルール（.c から .o を作成）
%.o: %.c
	$(CC) $(CFLAGS) -c $<

# 2. DOS用COM生成ルール
$(TARGET).com: $(OBJS)
#	$(CC) $(LDFLAGS) -subtype=msxdos -lndos -o $(TARGET).com $(OBJS)
	$(CC) $(LDFLAGS) -o $(TARGET).com $(OBJS)

# 3. (おまけ) ROMファイルやBINも作りたい場合
#rom: $(OBJS)
#	$(CC) $(LDFLAGS) -subtype=rom -o $(TARGET) $(OBJS) -m -v

#bin: $(OBJS)
#	$(CC) $(LDFLAGS) -subtype=disk -zorg=0x83c0 -o $(TARGET).bin $(OBJS)

# 依存関係
$(TARGET).o: $(TARGET).c  sg.h spr_def.h sg_com.h inkey.h list.h spr_col.h sg_ene.h SG_MAIN.h sg_sht.h

clean:
	rm -f *.o *.rom *.map *.com *.bin *.map *.ihx *.cas
