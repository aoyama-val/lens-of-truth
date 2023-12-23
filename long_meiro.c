/*
 *  long_meiro.c
 *  Originally by Tsukatomo
 * https://trap.jp/post/472/
 */

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "long_meiro.h"

/***********/
/* 自動生成 */
/***********/

coord_t the_start; // スタート地点
coord_t the_goal;  // ゴール地点

int meiro[MAXX][MAXY] = {{0}}; // 迷路盤
coord_t nodes[MAXX * MAXY];	   // 未開発の（壁が伸びていない）柱のリスト
int nodend = 0;				   // nodesリストの長さ
coord_t path[MAXX * MAXY];	   // 探索した柱のリスト（スタック）
int pathend = 0;			   // pathリストの長さ

// 構造体の値を設定する
coord_t newcoord(int x, int y)
{
	coord_t result;
	result.x = x;
	result.y = y;
	return result;
}

// リストに新たな要素を追加(nodes)
void insert_node(coord_t c)
{
	nodes[nodend] = c;
	nodend++;
}
// リストに新たな要素を追加(path)
void insert_path(coord_t c)
{
	path[pathend] = c;
	pathend++;
}

// nodesリストの任意の要素を取り出す
coord_t remove_node(int k)
{
	coord_t out;
	if (k > nodend)
	{
		printf("error: Not found");
		return newcoord(-1, -1);
	}
	else
	{
		out = nodes[k];						 // 取り出す
		for (int i = k; i < nodend - 1; i++) // 取り出したぶん詰める
			nodes[i] = nodes[i + 1];
		nodend--;
	}
	return out;
}
// pathリストの†末尾†の要素を取り出す
coord_t remove_path(void)
{
	coord_t out;
	out = path[pathend - 1]; // 取り出す
	pathend--;
	return out;
}

// リスト総当たり検索(nodes)
int search_node(coord_t c)
{
	for (int i = 0; i <= nodend - 1; i++)
	{
		if ((nodes[i].x == c.x) && (nodes[i].y == c.y))
			return i;
	}
	return -1;
}
// リスト総当たり検索(path)
int search_path(coord_t c)
{
	for (int i = 0; i <= pathend - 1; i++)
	{
		if ((path[i].x == c.x) && (path[i].y == c.y))
			return i;
	}
	return -1;
}

// 上下左右シャッフル
void shuffle(coord_t *direction)
{
	int j;
	coord_t tmp;
	for (int i = 0; i < 4; i++)
	{
		j = rand() % 4;
		tmp = direction[i];
		direction[i] = direction[j];
		direction[j] = tmp;
	}
}

// 柱を探索（再帰）
int choose_node(coord_t c)
{
	int s, r;
	coord_t p_1, p_2;
	coord_t d[4] = {newcoord(0, 2),
					newcoord(0, -2),
					newcoord(2, 0),
					newcoord(-2, 0)}; // 壁を伸ばす方向

	// printf("x:%d, y:%d\n", c.x, c.y);
	if (search_path(c) != -1)
	{			  // cが探索済の時
		return 1; // 生成失敗
	}
	else
	{
		insert_path(c); // pathに追加
		s = search_node(c);
		if (s != -1)
		{					// cが未探索の時
			remove_node(s); // nodeから消す
			// 上下左右のいずれかに壁を伸ばす
			shuffle(d);
			for (int i = 0; i < 4; i++)
			{
				r = choose_node(newcoord(c.x + d[i].x, c.y + d[i].y));
				if (r == 0)
					break;
			}
			if (r == 1)
				insert_node(remove_path());
			return r;
		}
		else
		{ // cがすでに壁の時
			// pathリストの末尾から一つずつ取り出し壁を作る
			p_2 = remove_path();
			do
			{
				p_1 = p_2;
				p_2 = remove_path();
				// printf("wall(%d, %d)\n",(p_1.x + p_2.x) / 2, (p_1.y + p_2.y) / 2);
				meiro[(p_1.x + p_2.x) / 2][(p_1.y + p_2.y) / 2] = 1;
			} while (pathend);
			return 0;
		}
	}
}

// 迷路の自動生成
void make_meiro(int w, int h)
{
	while (nodend)
	{
		choose_node(nodes[rand() % nodend]);
	}
	// 外壁
	for (int j = 0; j < h; j++)
	{
		meiro[0][j] = 1;
		meiro[w - 1][j] = 1;
	}
	for (int i = 0; i < w; i++)
	{
		meiro[i][0] = 1;
		meiro[i][h - 1] = 1;
	}
}

/*******/
/* 探索 */
/*******/

/** 迷路作成に用いたpathスタックの領域をキューとして再利用 **/
// pathリストの†先頭†の要素を取り出す
coord_t remove_path_h(void)
{
	coord_t out = path[0];			  // 取り出す
	for (int i = 0; i < pathend; i++) // 詰める
		path[i] = path[i + 1];
	pathend--;
	return out;
}

// 地点pから最も遠い点を検索(幅優先)
coord_t search_far(int w, int h, coord_t p)
{
	coord_t n;
	int sx, sy;
	coord_t d[4] = {newcoord(0, 1),
					newcoord(0, -1),
					newcoord(1, 0),
					newcoord(-1, 0)}; // 進む方向

	// 迷路をコピー
	int meiro_2[w][h];
	for (int j = 0; j < h; j++)
	{
		for (int i = 0; i < w; i++)
		{
			meiro_2[i][j] = meiro[i][j];
		}
	}

	// スタートを追加
	pathend = 0;
	insert_path(p);

	// 繰り返し探索
	while (pathend)
	{
		n = remove_path_h();
		// printf("search = (%d, %d)\n", n.x, n.y);
		// printf ("%d\n", pathend);
		meiro_2[n.x][n.y] = 2; // 対応する座標を探索済みにする
		shuffle(d);			   // ランダム性を持たせるためシャッフル
		// printf("%d,%d\n",d[0].x,d[0].y);
		for (int i = 0; i < 4; i++)
		{
			sx = n.x + d[i].x; // 隣接マスのx座標
			sy = n.y + d[i].y; // 隣接マスのy座標
			if (meiro_2[sx][sy] == 0)
			{ // 隣接マスが未探索の道のとき
				insert_path(newcoord(sx, sy));
			}
		}
	}
	return n;
}

// 幅優先探索
int search_meiro(int w, int h, coord_t start, coord_t goal)
{
	coord_t n;
	int sx, sy;
	coord_t direction[w][h]; // 辿ってきた方向を記憶する配列(要素はd[4]のどれか)
	coord_t d[4] = {newcoord(0, 1),
					newcoord(0, -1),
					newcoord(1, 0),
					newcoord(-1, 0)}; // 進む方向

	// スタートを追加
	pathend = 0;
	insert_path(start);

	// 繰り返し探索
	while (pathend)
	{
		n = remove_path_h();
		// printf("search = (%d, %d)\n", n.x, n.y);
		// printf ("%d\n", pathend);
		meiro[n.x][n.y] = 2; // 対応する座標を探索済みにする
		if (n.x == goal.x && n.y == goal.y)
		{ // ゴール処理
			while (n.x != start.x || n.y != start.y)
			{
				// 値を更新
				sx = n.x;
				sy = n.y;
				// printf("PATH = (%d, %d)\n", n.x, n.y);
				// マーキング
				meiro[n.x][n.y] = 3;
				// 来た道を戻る
				n.x -= direction[sx][sy].x;
				n.y -= direction[sx][sy].y;
			}
			return 0;
		}
		else
		{ // 探索処理
			for (int i = 0; i < 4; i++)
			{
				sx = n.x + d[i].x; // 隣接マスのx座標
				sy = n.y + d[i].y; // 隣接マスのy座標
				if (meiro[sx][sy] == 0)
				{ // 隣接マスが未探索の道のとき
					insert_path(newcoord(sx, sy));
					direction[sx][sy] = d[i];
				}
			}
		}
	}
	printf("I cannot find the goal...\n");
	return 1;
}

// 迷路を表示
void show_meiro(int w, int h, coord_t start, coord_t goal)
{
	for (int j = 0; j < h; j++)
	{
		for (int i = 0; i < w; i++)
		{
			if ((i == start.x) && (j == start.y))
			{
				printf("\x1b[41m");
				printf("\x1b[37m");
				printf("S "); // スタート
			}
			else if ((i == goal.x) && (j == goal.y))
			{
				printf("\x1b[41m");
				printf("\x1b[37m");
				printf("G "); // ゴール
			}
			else
			{
				switch (meiro[i][j])
				{
				case 0: // 探索されていない道
					printf("\x1b[0m");
					break;
				case 1: // 壁
					printf("\x1b[42m");
					break;
				case 2:				   // 探索された道
					printf("\x1b[0m"); // [47mでうっすら表示できるよ
					break;
				case 3: // 正解ルート
					printf("\x1b[45m");
					break;
				default:
					printf("\x1b[0m");
				}
				printf("  ");
			}
		}
		printf("\x1b[0m");
		printf("\n");
	}
	printf("\n\n");
}

void create_meiro(int width, int height, int *pw, int *ph)
{
	// 柱リストの作成
	for (int j = 1; j < height; j++)
	{
		for (int i = 1; i < width; i++)
		{
			insert_node(newcoord(2 * i, 2 * j));
			// printf("%d\n", nodend);
			meiro[2 * i][2 * j] = 1;
		}
	}

	// 迷路盤の使用領域を設定
	int w = 2 * width + 1;	// 迷路盤の幅
	int h = 2 * height + 1; // 迷路盤の高さ

	// 迷路の自動生成
	make_meiro(w, h);

	// スタート、ゴールを設定
	the_start = search_far(w, h, newcoord((rand() % width) * 2 + 1, (rand() % height) * 2 + 1));
	the_goal = search_far(w, h, the_start);

	if (pw != NULL)
	{
		*pw = w;
	}
	if (ph != NULL)
	{
		*ph = h;
	}
}

/***********/
/*  メイン  */
/***********/
#ifdef LONG_MEIRO_MAIN
int main(void)
{
	srand((unsigned)time(NULL));
	int width, height; // 壁の厚さを0、道の太さを1とした時の迷路の幅、高さ
	printf("width> ");
	scanf("%d", &width);
	printf("height> ");
	scanf("%d", &height);

	int w, h;
	create_meiro(width, height, &w, &h);

	// 迷路の表示
	show_meiro(w, h, the_start, the_goal);

	printf("[end of 'long_meiro.c']\n");
	return 0;
}
#endif