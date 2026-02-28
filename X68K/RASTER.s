	.section .text
	.global r_start
	.global HSYNC_handler
	.global RASTER_handler
	.global VSYNC_handler
	.extern sin_table


	/* HSYNC割り込みハンドラ */
HSYNC_handler:
#	move.w	raster_line,0xe80018 /* 水平オフセット書き込み */
	rte

RASTER_handler:

#	movem.l %d0-%d1/%a0, -(%sp)	/* レジスタ退避 */
	movem.l %d1/%a0,-(%sp)		/* レジスタ退避 */

	move.w line_counter,%d1


	move.l	(psintable),%a0
#	move.w  (%a0, %d1.w), 0xe80018	/* sinテーブルからオフセット取得(GRP0) */
	move.w  (%a0, %d1.w), 0xe8001c	/* sinテーブルからオフセット取得(GRP1) */

	move.w	%d1,0xe80012		/* 次のラスタ */
	addq.w	#4,%d1				/* ラインカウンタ進める */
	and.w	#0x1ff,%d1			/* 512まで */
	move.w  %d1,line_counter

	movem.l (%sp)+,%d1/%a0		/* レジスタ復帰 */
#	movem.l (%sp)+, %d0-%d1/%a0	/* レジスタ復帰 */
	rte

	/* VSYNC割り込みハンドラ */
VSYNC_handler:
#	rts

	/* 初期化 */
r_start:
	movem.l %d0/%a0, -(%sp)
	lea	 sin_table, %a0
	move.l  table_offset, %d0
	addq.l  #2, %d0			/* テーブルを2バイト進める（アニメーション） */
	and.l	#0x1ff,%d0		/* 512まで */
	move.l  %d0, table_offset

	add.l	%d0,%a0
	move.l	%a0,psintable
	movem.l (%sp)+, %d0/%a0
	rts
#	rte

	/* データセクション */
	.section .data
line_counter:
	.word 0					/* 現在のライン */
table_offset:
	.long 0					/* sinテーブルのオフセット */
psintable:
	.long 0					/* sinテーブルへのポインタ */
#raster_line:
#	.word 0
