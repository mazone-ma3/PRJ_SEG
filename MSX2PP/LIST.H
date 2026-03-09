/* リスト構造定義マクロ ヘッダファイル By ma3 2001. */

#ifndef LIST_H_INCLUDE
#define LIST_H_INCLUDE

/*int next_num[MAX + 1];*/	/* 次の要素の番号 */
/*int free_num[MAX];	*/	/* 未使用の要素の番号 */

#define END_LIST 255

#define INIT_LIST(MAX, INDEX, NEXT, FREE) {\
	{\
		for(INDEX = 0; INDEX <= MAX; INDEX++){\
			FREE[INDEX] = INDEX + 1;\
			NEXT[INDEX] = END_LIST;\
		}\
		FREE[MAX - 1] = END_LIST;\
		FREE[MAX] = 0;\
	}\
}

/* リストの先頭に要素を追加する */
/* (フリーリストから要素を削除する) */

/* フリーリストの先頭から要素を取り出す */
/* フリーリストの先頭から要素を削除する */
/*  */
/* リストの先頭に取り出した要素を追加する */

#define ADD_LIST(MAX, NEW_INDEX, NEXT, FREE) {\
	NEW_INDEX = FREE[MAX];\
	FREE[MAX] = FREE[NEW_INDEX];\
	NEXT[NEW_INDEX] = NEXT[MAX];\
	NEXT[MAX] = NEW_INDEX;\
}

/* リストから要素を削除する */
/* (フリーリストに要素を追加する) */
/*  */
/* フリーリストの先頭に要素を追加する */
/* リストから要素を削除する */
#define DEL_LIST(MAX, INDEX, PREV, NEXT, FREE) {\
	FREE[INDEX] = FREE[MAX];\
	FREE[MAX] = INDEX;\
	NEXT[PREV] = NEXT[INDEX];\
	INDEX = PREV;\
}

#define SEARCH_LIST(MAX, INDEX, NEXT) for(INDEX = NEXT[MAX]; INDEX != END_LIST; INDEX = NEXT[INDEX])

#define SEARCH_LIST2(MAX, INDEX, PREV, NEXT) for(INDEX = NEXT[MAX], PREV = MAX; INDEX != END_LIST; PREV = INDEX, INDEX = NEXT[INDEX])

#endif
