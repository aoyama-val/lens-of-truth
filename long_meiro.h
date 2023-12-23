#ifndef LONG_MEIRO_H
#define LONG_MEIRO_H

// 迷路盤面の座標は構造体で管理
typedef struct
{
	int x;
	int y;
} coord_t;

#define MAXX 256
#define MAXY 256

#ifndef LONG_MEIRO_C
extern coord_t the_start; // スタート地点
extern coord_t the_goal;  // ゴール地点
extern int meiro[MAXX][MAXY]; // 迷路盤

void create_meiro(int width, int height, int *pw, int *ph);
void show_meiro(int w, int h, coord_t start, coord_t goal);
#endif

#endif