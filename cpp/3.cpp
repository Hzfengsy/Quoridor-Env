#include "submit.h"
#include <iostream>
#include <cstring>
#include <cstdio>
#include <cmath>
#include <cstdlib>
#include <algorithm>
#include <cmath>
#include <queue>
#include <set>
#include <bits/stdc++.h>
using namespace std;

const int MAXN = 21;
const double KEYV = 1;
const int HIDUSE = 50;
const int HIDWAL = 2;
const int STWAL = 5;
const double WALVAL = 1.99;

extern int ai_side;
std::string ai_name="Visa's artificial idiot";

bool hs[MAXN][MAXN];

int locx[2], locy[2]; 
int d = 0;
int cntmov = 0;
int cntwalmy, cntwalopo;

void init() {
	locy[0] = locy[1] = 10;
	locx[0] = 2;
	locx[1] = 18;
	for (int i = 1; i <= 19; ++i)
		hs[1][i] = hs[i][1] = hs[19][i] = hs[i][19] = 1;
	cntwalmy = 10;
	/*
	if (ai_side == 0) {
	locx = 2;
	locy = 10;
	d = 2;
	} else {
	locx = 18;
	locy = 10;
	d = -2;
	}
	*/
}

bool temp = true;

int bstx, bsty;
double bstv, bstv2;

int dis[2][MAXN][MAXN];

int dx[] = { 0, 1, 0, -1 };
int dy[] = { 1, 0, -1, 0 };

inline bool InsideBoard(int x, int y) {
	if (x < 2 || y < 2 || x > 18 || y > 18)
		return 0;
	return 1;
}

bool AlreadyHas(int x, int y) {
	if (!InsideBoard(x, y))
		return 0;
	return hs[x][y];
}

bool CheckWall(int tox, int toy, int x, int y) {
	if (!InsideBoard(tox, toy) || !InsideBoard(x, y))
		return 0;
	if (tox == x) {
		int mid = y + toy;
		mid >>= 1;
		if (AlreadyHas(x, mid))
			return 0;
		return 1;
	}
	else {
		int mid = x + tox;
		mid >>= 1;
		if (AlreadyHas(mid, y))
			return 0;
		else
			return 1;
	}
}// check if (x, y) to (tox, toy) has a wall and valid

void updatedis(int side) {
	queue<pair<int, int> > q;
	int zerx = side ? 2 : 18;
	memset(dis[side], 31, sizeof(dis[side]));
	for (int i = 2; i <= 18; i += 2) {
		dis[side][zerx][i] = 0;
		q.push(make_pair(zerx, i));
	}

	while (!q.empty()) {
		int curx = q.front().first, cury = q.front().second;
		q.pop();
		for (int v = 0; v < 4; ++v) {
			int tox = curx + dx[v] * 2;
			int toy = cury + dy[v] * 2;
			if (CheckWall(tox, toy, curx, cury)) {
				if (dis[side][tox][toy] > dis[side][curx][cury] + 1) {
					dis[side][tox][toy] = dis[side][curx][cury] + 1;
					q.push(make_pair(tox, toy));
				}
			}
		}
	}
}

int getdis(int side) {
	updatedis(side);
	return dis[side][locx[side]][locy[side]] >= 1e8 ? -1e9 : dis[side][locx[side]][locy[side]];
}

int stp1x, stp1y;

void update() {
	int key0 = getdis(ai_side);
	int key1 = getdis(ai_side ^ 1);
	if(key0 == -1e9 || key1 == -1e9)
		return;
	double eval = key0 - KEYV * key1 - (cntwalmy + cntwalopo) * WALVAL;
	if (eval >	bstv2) {
		//bstx = stp1x;
		//bsty = stp1y;
		bstv2 = eval;
	}
}

void OpoWall() {
	//for (auto it = AllValidWall.begin(); it != AllValidWall.end(); it++) {
	//	stp1x = it->first;
	//	stp1y = it->second;
	++cntwalopo;
	bool flag = stp1x == 17 && stp1y == 4;
	for(int stp1x = 2; stp1x <= 17; ++stp1x)
	for(int stp1y = (stp1x & 1 ? 2 : 3); stp1y <= 17; stp1y += 2) {
		if (stp1x & 1) {
			if(hs[stp1x][stp1y] || hs[stp1x][stp1y + 1] || hs[stp1x][stp1y + 2])
				continue;
			hs[stp1x][stp1y] = hs[stp1x][stp1y + 1] = hs[stp1x][stp1y + 2] = 1;
		}
		else {
			if(hs[stp1x][stp1y] || hs[stp1x + 1][stp1y] || hs[stp1x + 2][stp1y])
				continue;
			hs[stp1x][stp1y] = hs[stp1x + 1][stp1y] = hs[stp1x + 2][stp1y] = 1;
		}
		//cerr<<stp1x<<" "<<stp1y<<endl;
		//if(g)
		update();
		//if(stp1x == 11 && stp1y == 16 && flag)
			//cerr<<getdis(ai_side)<<" vs "<<getdis(ai_side ^ 1)<<" "<<bstx<<" "<<bsty<<" "<<bstv<<endl;
		if (stp1x & 1) {
			hs[stp1x][stp1y] = hs[stp1x][stp1y + 1] = hs[stp1x][stp1y + 2] = 0;
		}
		else {
			hs[stp1x][stp1y] = hs[stp1x + 1][stp1y] = hs[stp1x + 2][stp1y] = 0;
		}
	}
	--cntwalopo;
}

void TryMov(int aiside);

void OpoMov() {
	TryMov(ai_side ^ 1);
}

void dfs_opo() {
	//cerr<<cntmov<<endl;
	bstv2 = -1e9;
	//if(stp1x & 1 || stp1y & 1)
		//cerr<<"wall1 "<<stp1x<<" "<<stp1y<<endl;
	if(cntwalopo < 10)
		OpoWall();
	OpoMov();
	if(bstv2 < bstv) {
		bstv = bstv2;
		bstx = stp1x;
		bsty = stp1y;
	}
}

void mov1(int side, int x, int y) {
	//cerr<<x<<" "<<y<<" s "<<side<<endl;
	int xx = locx[side];
	int yy = locy[side];
	locx[side] = x;
	locy[side] = y;
	stp1x = x;
	stp1y = y;
//	cerr<<"wall1 "<<stp1x<<" "<<stp1y<<endl;
	dfs_opo();
	locx[side] = xx;
	locy[side] = yy;
}

void mov2(int side, int x, int y) {
	int xx = locx[side];
	int yy = locy[side];
	locx[side] = x;
	locy[side] = y;
	update();
	locx[side] = xx;
	locy[side] = yy;
}

void mov(int side, int x, int y) {
	if (side == ai_side)
		mov1(ai_side, x, y);
	else
		mov2(side, x, y);
}

void TryWall() {
	//	stp1x = it->first;
	//	stp1y = it->second;
	--cntwalmy;
	for(stp1x = 2; stp1x <= 17; ++stp1x)
	for(stp1y = (stp1x & 1 ? 2 : 3); stp1y <= 17; stp1y += 2) {
		if (stp1x & 1) {
			if(hs[stp1x][stp1y] || hs[stp1x][stp1y + 1] || hs[stp1x][stp1y + 2])
				continue;
			hs[stp1x][stp1y] = hs[stp1x][stp1y + 1] = hs[stp1x][stp1y + 2] = 1;
		}
		else {
			if(hs[stp1x][stp1y] || hs[stp1x + 1][stp1y] || hs[stp1x + 2][stp1y])
				continue;
			hs[stp1x][stp1y] = hs[stp1x + 1][stp1y] = hs[stp1x + 2][stp1y] = 1;
		}
		if(getdis(ai_side ^ 1) != -1e9 && getdis(ai_side) != -1e9)
		//cerr<<stp1x<<" "<<stp1y<<endl;
			dfs_opo();
		if (stp1x & 1) {
			hs[stp1x][stp1y] = hs[stp1x][stp1y + 1] = hs[stp1x][stp1y + 2] = 0;
		}
		else {
			hs[stp1x][stp1y] = hs[stp1x + 1][stp1y] = hs[stp1x + 2][stp1y] = 0;
		}
	}
	++cntwalmy;
}

void prtdis(int side) {
	for(int i = 2; i <= 18; i += 2, cerr<<endl)
		for(int j = 2; j <= 18; j += 2)
			cerr<<dis[side][i][j]<<" ";
	cerr<<endl;
}

void TryMov(int aiside) {
	//if(aiside == ai_side)
	//	prtdis(aiside);
	int ST = aiside ? 3 : 1;
	int tow = ST;
	do {
		updatedis(aiside);
		int tox = locx[aiside] + dx[tow] * 2;
		int toy = locy[aiside] + dy[tow] * 2;
		if (CheckWall(locx[aiside], locy[aiside], tox, toy)) {
			if (locx[aiside ^ 1] == tox && locy[aiside ^ 1] == toy) {
				if (CheckWall(tox, toy, tox + dx[tow] * 2, toy + dy[tow] * 2)) {
					tox += dx[tow] * 2;
					toy += dy[tow] * 2;
					if (dis[aiside][tox][toy] < dis[aiside][locx[aiside]][locy[aiside]]) {
						mov(aiside, tox, toy);
					}
				}
				else {
					int key;
					key = (tow + 1) % 4;
					int tx, ty;
					tx = tox + dx[key] * 2;
					ty = toy + dy[key] * 2;
					if (CheckWall(tox, toy, tx, ty) && dis[aiside][tx][ty] < dis[aiside][locx[aiside]][locy[aiside]])
						mov(aiside, tx, ty);
					updatedis(aiside);
					key = (tow + 3) % 4;
					tx = tox + dx[key] * 2;
					ty = toy + dy[key] * 2;
					if (CheckWall(tox, toy, tx, ty) && dis[aiside][tx][ty] < dis[aiside][locx[aiside]][locy[aiside]])
						mov(aiside, tx, ty);
				}
			}
			else {
				if (dis[aiside][tox][toy] < dis[aiside][locx[aiside]][locy[aiside]]) {
					mov(aiside, tox, toy);
				}
			}
		}
		//cerr<<tow<<endl;
		++tow;
		if (tow >= 4)
			tow -= 4;
	} while (tow != ST);
}

void dfs_my() {
	if (cntwalmy && cntmov >= STWAL) {
		if(cntwalmy > HIDWAL || cntmov >= HIDUSE)
			TryWall();
		cerr<<bstv<<" w "<<bstx<<" "<<bsty<<endl;
	}
	if((!cntwalmy) || getdis(ai_side ^ 1) > 1 || bstv == 1e9) {
		TryMov(ai_side);
		cerr<<cntmov<<" mv "<<bstv<<endl;
	}
}

void GetUpdate(std::pair<int, int> location) {
	++cntmov;
	int x = location.first;
	int y = location.second;
	if (x & 1) {
		hs[x][y] = hs[x][y + 1] = hs[x][y + 2] = 1;
		++cntwalopo;
		//WallRemove(x, y);
	}
	else
		if (y & 1) {
			hs[x][y] = hs[x + 1][y] = hs[x + 2][y] = 1;
			++cntwalopo;
			//WallRemove(x, y);
		}
		else {
			hs[locx[ai_side ^ 1]][locy[ai_side ^ 1]] = 0;
			locx[ai_side ^ 1] = x;
			locy[ai_side ^ 1] = y;
			hs[locx[ai_side ^ 1]][locy[ai_side ^ 1]] = 1;
		}
}

std::pair<int, int> Action() {
	bstv = 1e9;
	++cntmov;
	dfs_my();
	if ((bstx & 1) || (bsty & 1)) {
		--cntwalmy;
		if(bstx & 1) {
			hs[bstx][bsty] = hs[bstx][bsty + 1] = hs[bstx][bsty + 2] = 1;
		}
		else {
			hs[bstx][bsty] = hs[bstx + 1][bsty] = hs[bstx + 2][bsty] = 1;
		}
		//WallRemove(bstx, bsty);
	}
	else {
		locx[ai_side] = bstx;
		locy[ai_side] = bsty;
	}
	cerr<<getdis(ai_side)<<" "<<getdis(ai_side ^ 1)<<endl;
	cerr<<bstx<<" "<<bsty<<" v "<<bstv<<endl<<endl;
	return make_pair(bstx, bsty);
}