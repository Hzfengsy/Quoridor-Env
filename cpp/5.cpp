#include "submit.h"
#include <cstring>
#include <string>
#include <iostream>
#include <bitset>
#include <queue>
#include <vector>

extern int ai_side;
std::string ai_name = "sakura";

int locx, locy;
int d = 0;

class Player
{
public:
	int x, y;
	int barriers;
	bool playerOne = true;

public:
	Player();
	Player(int px, int py);
	~Player();
	Player(Player& p);
};

Player::Player()
{
	x = 4;
	y = 0;
	barriers = 10;
}

Player::Player(int px, int py)
{
	x = px;
	y = py;
	barriers = 10;
}

Player::Player(Player& p) {
	x = p.x;
	y = p.y;
	playerOne = p.playerOne;
	barriers = p.barriers;
}

Player::~Player()
{
}

struct play {
	bool playerMove; //true: player Movement, ignore vert; wrong: Barrier placement
	int direction;
	int o2 = 0;
	int x, y;
	int x2, y2;
	bool vert;
	play(bool p, int d, int x, int y, bool v,int x2 = 0,int y2 = 0) : playerMove(p), direction(d), x(x), y(y), vert(v),x2(x2),y2(y2) {}
	play() {};
};

struct Barrier {
	Barrier(bool vert, int x, int y) : vert(vert), x(x), y(y) {}

	bool vert; int x, y;
};

class Board
{
private:
	bool playerMove = true; // true = Player 1 | false = Player 2
public:
	Board();
	~Board();

	Board(Board& b);
	bool possible(short x, short y, short direction);
	bool barrier_possible(int x, int y, bool vert);

	bool check_win(Player p);
	void switch_player();
	int destination(Player p);
	int evaluate();
	void make_play(play p);
	void delete_play(play p);

	void place_barrier(Barrier b);
	void delete_barrier(Barrier b);

	//std::queue<play> possibleMoves();
	std::queue<play> get_moves();//Not all moves, but the ones a human player will play almost always


	Player* now_player();
	Player* not_now_player();

	std::bitset<9> vBarriers[9] = {}; // 9*9 0 == you can move from there ; 1 == theres a barrier, can't pass //9th place unused for real data but helpful for ORing
	std::bitset<9> hBarriers[9] = {}; // 9*8 Numbers are counted from top left to bot right

	std::vector<Barrier> barriers;

	Player one, two;

	bool operator==(Board c);

};

Board::Board()
{
	one.playerOne = true;
	two.playerOne = false;
	one.x = 4; one.y = 0;
	two.x = 4; two.y = 8;
}

Board::~Board()
{
}

Board::Board(Board& b) {
	one = b.one;
	two = b.two;
	playerMove = b.playerMove;
}

bool Board::possible(short x, short y, short direction)
{
	if (direction == 2 && y == 8) { //bottom corner
		return false;
	}
	else if (direction == 0 && y == 0) {
		return false;
	}
	else if (direction == 1 && x == 8) {
		return false;
	}
	else if (direction == 3 && x == 0) {
		return false;
	}
	else if (direction == 0) {
		return !hBarriers[y - 1][x];
	}
	else if (direction == 2) { // go to bottom
		return !hBarriers[y][x];

	}
	else if (direction == 1) {
		return !vBarriers[y][x];

	}
	else if (direction == 3) {
		return !vBarriers[y][x - 1];

	}
	return false;
}

bool Board::barrier_possible(int x, int y, bool vert) {

	if (x < 0 || x > 7 || y < 0 || y > 7) {
		return false;
	}
	if (now_player()->barriers <= 0) {
		return false;
	}

	if (vert ? !possible(x, y, 1) || !possible(x, y + 1, 1) : !possible(x, y, 2) || !possible(x + 1, y, 2))
		return false;

	for (auto b = barriers.begin(); b != barriers.end(); b++) { //Barriers are blocking each other "in a cross"
		if (b->vert != vert && x == b->x && y == b->y)
			return false;
	}

	//Barrier is theoretical possible to place, now check if both players can win if it is placed
	play p(false, -1, x, y, vert);
	make_play(p);
	bool possible = true;
	if (destination(one) == -1 || destination(two) == -1) { //Play blocks one Player
		possible = false;
		//msg += "fF(one) " + to_string(floodFill(one)) + " fF(two) " + to_string(floodFill(two)) + "\n";
	}
	delete_play(p);
	return possible;
}

int Board::evaluate() { // positive is really good //evaluate light...
	if (check_win(*not_now_player())) {
		return -1000;
	}
	if (check_win(*now_player())) {
		return 1000;
	}
	int factor = now_player()->playerOne ? -1 : 1;

	int barrierDiff = -now_player()->barriers + not_now_player()->barriers;
	//msg += "Evaluated: " + to_string(moveDiff * 10 + barrierDiff * 1);

	return factor * (destination(one) - destination(two));
}
bool dd;
int Board::destination(Player p) {

	int target = 0;
	if (p.playerOne) {
		target = 8;
	}
	int steps = 0;
	std::bitset<9> f[9] = {}; //Array is y-Coordinate, solo bits are x
	std::bitset<9> new_f[9] = {};
	f[p.y][p.x] = 1; //Now having a zeroed bitset with only a 1 at the player position
	bool equal = false;
	if (dd) {
		for (int i = 0; i < 9; i++) {
			for (int j = 0; j < 9; j++) {
				std::cerr << vBarriers[i][j] << " ";
			}
			std::cerr << std::endl;
		}
		for (int i = 0; i < 9; i++) {
			for (int j = 0; j < 9; j++) {
				std::cerr << hBarriers[i][j] << " ";
			}
			std::cerr << std::endl;
		}

	}
	while (!equal) { //Flood everything
		equal = true;
		if (f[target] != 0) {
			return steps;
		}
		for (int i = 0; i<9; i++) {
			if (f[i] != 0) {
				//Right Movement:
				new_f[i] |= ((~vBarriers[i] & f[i]) << 1);
				//Left Movement:
				new_f[i] |= (~vBarriers[i] & (f[i] >> 1));
				//Top Movement:
				if (i != 0) {
					new_f[i - 1] |= (~hBarriers[i - 1]) & f[i];
				}
				//Bot Movement:
				if (i != 8) {
					new_f[i + 1] |= (~hBarriers[i] & f[i]);
				}
			}
		}
		steps++;


		for (int i = 0; i<9; i++) {
			if (f[i] != new_f[i]) {
				f[i] |= new_f[i];
				if (new_f[i] != 0)
					equal = false;
			}
		}
	}

	return -1; //Unable to reach the target

}

void Board::make_play(play p) {
	if (p.playerMove == true) {
		if (p.direction == 5) {
			now_player()->y = p.y;
			now_player()->x = p.x;
		}
	}
	else {
		place_barrier(Barrier(p.vert, p.x, p.y));
	}
	playerMove = !playerMove;
}

void Board::delete_play(play p) {
	playerMove = !playerMove;
	if (p.playerMove == true) {
		if (p.direction == 5) {
			now_player()->y = p.y2;
			now_player()->x = p.x2;
		}
	}
	else {
		delete_barrier(Barrier(p.vert, p.x, p.y));
	}

}

Player *Board::now_player()
{
	if (playerMove) {
		return &one;
	}
	else {
		return &two;
	}
}

Player *Board::not_now_player() {
	if (!playerMove) {
		return &one;
	}
	else {
		return &two;
	}
}

bool Board::check_win(Player p) {
	if (p.playerOne && p.y == 8) {
		return true;
	}
	else if (!p.playerOne && p.y == 0) {
		return true;
	}
	return false;
}

void Board::switch_player()
{
	playerMove = !playerMove;
}

bool Board::operator==(Board c) {
	if (playerMove != c.playerMove)
		return false;
	if (one.x != c.one.x || one.y != c.one.y)
		return false;
	if (two.x != c.two.x || two.y != c.two.y)
		return false;
	if (one.barriers != c.one.barriers || two.barriers != c.two.barriers)
		return false;

	for (int i = 0; i<8; i++) {
		if (hBarriers[i] != c.hBarriers[i])
			return false;
	}
	for (int i = 0; i<9; i++) {
		if (vBarriers[i] != c.vBarriers[i])
			return false;
	}

	//If the Bitsets are the same the barriers have to be the same, or at least they block the same things

	return true;
}

void Board::place_barrier(Barrier b) {

	barriers.push_back(b);
	now_player()->barriers--;
	if (b.vert) {
		vBarriers[b.y][b.x] = 1;
		vBarriers[b.y + 1][b.x] = 1;
	}
	else {
		hBarriers[b.y][b.x] = 1;
		hBarriers[b.y][b.x + 1] = 1;

	}

}

void Board::delete_barrier(Barrier b) {

	now_player()->barriers++;
	if (b.vert) {
		vBarriers[b.y][b.x] = 0;
		vBarriers[b.y + 1][b.x] = 0;
	}
	else {
		hBarriers[b.y][b.x] = 0;
		hBarriers[b.y][b.x + 1] = 0;

	}

	for (auto br = barriers.begin(); br != barriers.end(); br++) { //Remove Barrier from the list of barriers
		if (br->vert == b.vert && b.x == br->x && b.y == br->y) {
			barriers.erase(br);
			return;
		}

	}


}

std::queue<play> Board::get_moves() {

	std::queue <play> q;
	//queue <play> pq; //Priority queue, this moves are more likely to be good
	int x = now_player()->x; int y = now_player()->y;
	int x2 = not_now_player()->x; int y2 = not_now_player()->y;
	if (possible(x, y, 0)) {
		if (x == x2 && y - 1 == y2) {
			bool have_wall = hBarriers[y - 2][x];
			if (possible(x, y - 1, 0) && !have_wall) {
				q.push(play(true, 5, x, y - 2, false, x, y));
			}
			if (possible(x, y - 1, 1) && have_wall) {
				q.push(play(true, 5, x + 1, y - 1, false, x, y));
			}
			if (possible(x, y - 1, 3) && have_wall) {
				q.push(play(true, 5, x - 1, y - 1, false, x, y));
			}
		}
		else q.push(play(true, 5, x, y - 1, false, x, y));
	}
	if (possible(x, y, 1)) {
		if (x + 1 == x2 && y == y2) {
			bool have_wall = vBarriers[y][x+1];
			if (possible(x + 1, y, 0) && have_wall) {
				q.push(play(true, 5, x + 1, y - 1, false, x, y));
			}
			if (possible(x + 1, y, 1) && !have_wall) {
				q.push(play(true, 5, x + 2, y, false, x, y));
			}
			if (possible(x + 1, y, 2) && have_wall) {
				q.push(play(true, 5, x + 1, y + 1, false, x, y));
			}
		}
		else q.push(play(true, 5, x + 1, y, false, x, y));
	}
	if (possible(x, y, 2)) {
		if (x == x2 && y + 1 == y2) {
			bool have_wall = hBarriers[y + 1][x];
			if (possible(x, y + 1, 1)&&have_wall) {
				q.push(play(true, 5, x + 1, y + 1, false, x, y));
			}
			if (possible(x, y + 1, 2)&&!have_wall) {
				q.push(play(true, 5, x, y + 2, false, x, y));
			}
			if (possible(x, y + 1, 3)&&have_wall) {
				q.push(play(true, 5, x - 1, y + 1, false, x, y));
			}
		}
		else q.push(play(true, 5, x, y + 1, false, x, y));
	}
	if (possible(x, y, 3)) {
		if (x - 1 == x2 && y == y2) {
			bool have_wall = vBarriers[y][x-2];
			if (possible(x - 1, y, 0)&&have_wall) {
				q.push(play(true, 5, x - 1, y - 1, false, x, y));
			}
			if (possible(x - 1, y, 2)&&have_wall) {
				q.push(play(true, 5, x - 1, y + 1, false, x, y));
			}
			if (possible(x - 1, y, 3)&&!have_wall) {
				q.push(play(true, 5, x - 2, y, false, x, y));
			}
		}
		else q.push(play(true, 5, x - 1, y, false, x, y));
	}

	std::bitset<8> barVert[8] = {}; //Make Bitmap of likely barriers
	std::bitset<8> barHorz[8] = {};

	const int offsetP[4][2] = { -1, 0,  0,0,  -1,-1,  0,-1 };
	for (int i = 0; i<4; i++) {
		int px = x + offsetP[i][0], py = y + offsetP[i][1];
		if (px >= 0 && px < 9 && py >= 0 && py < 9) //Boundary Check
		{
			barVert[px][py] = 1;
			barHorz[px][py] = 1;
		}
	}
	x = not_now_player()->x; y = not_now_player()->y;
	for (int i = 0; i<4; i++) {
		int px = x + offsetP[i][0], py = y + offsetP[i][1];
		if (px >= 0 && px < 9 && py >= 0 && py < 9) //Boundary Check
		{
			barVert[px][py] = 1;
			barHorz[px][py] = 1;
		}
	}

	const int offsetB[8][2] = { -2,0,  -1,-1,  0,-1,  1,-1,  2,0,   1,1,   0,1,  -1,1 }; //Round around the existing barriers
	const bool vertical[8] = { false, true, true, true, false, true, true, true }; //Could be boolean expression
	for (auto br = barriers.begin(); br != barriers.end(); br++) { //Go through list of barriers
		for (int i = 0; i<8; i++) {
			int py = br->y + offsetB[i][1];
			int px = br->x + offsetB[i][0];
			if (px >= 0 && px < 8 && py >= 0 && py < 8) //Boundary Check
			{
				if (vertical[i])
					barVert[px][py] = 1;
				else
					barHorz[px][py] = 1;
			}

		}

	}
	//Add barriers to queue
	for (int i = 0; i<8; i++) {
		for (int j = 0; j<8; j++) {
			if (barVert[i][j] && barrier_possible(i, j, true))
				q.push(play(false, -1, i, j, true));
			if (barHorz[i][j] && barrier_possible(i, j, false))
				q.push(play(false, -1, i, j, false));
		}
	}



	return q;
}

class Game
{
private:
	Board game_board;
public:
	Game();
	~Game();

	int dfs(Board* b, int depth, int alpha, int beta);
	play computer_move();
	void make_play(play a);
	void debug();

}this_game;

Game::Game()
{
}

Game::~Game()
{
}

int Game::dfs(Board *b, int depth, int alpha, int beta)
{
	if (depth == 0 || b->check_win(*b->now_player()) || b->check_win(*b->not_now_player())) {
		//msg += to_string(b->evaluate());
		return b->evaluate();
	}

	std::queue<play> moves = b->get_moves();
	while (!moves.empty()) {
		b->make_play(moves.front()); //Try next Move
		int score = -dfs(b,depth - 1, -beta, -alpha);
		b->delete_play(moves.front());
        moves.pop();
		//msg += to_string(score) + "|";
		if (score > alpha) {
			alpha = score; // alpha acts like max in MiniMax
		}
		if (score >= beta) {
			return beta; //beta-cutoff
		}
	}
	return alpha;
}

play Game::computer_move() {
	play best_play;
	
	int alpha = -1001, beta = 1001;

    std::queue<play> q = game_board.get_moves();
    int bestScore=-1001;
    while(!q.empty()){
        game_board.make_play(q.front());
        int score = -dfs(&this->game_board,3,alpha,beta);
        if(score > bestScore) {
            bestScore = score;
            best_play = q.front();
        }
        game_board.delete_play(q.front());
        q.pop();
    }

	game_board.make_play(best_play);

	return best_play;

}

void Game::make_play(play a) {
	game_board.make_play(a);
}

void Game::debug() {
	std::cerr <<"1"<<" "<< game_board.one.x << " " << game_board.one.y 
		<< " " << this_game.game_board.destination(this_game.game_board.one) << std::endl;
	std::cerr <<"2"<<" "<< game_board.two.x << " " << game_board.two.y
		<<" "<< this_game.game_board.destination(this_game.game_board.two)<< std::endl;
}

void init() {
	/* Your code here */
	if (ai_side == 0) {

	}
	else {

	}
}

int ooo = 0;

int tot_step = 0;
void GetUpdate(std::pair<int, int> location) {
	tot_step++;
	std::cerr << tot_step << "=====================" << std::endl;
	int xx = location.first, yy = location.second;
	std::swap(xx, yy);
	play u;
	if ((!(xx & 1)) && (yy & 1)) {
		u = play(false, -1, (xx - 2)/2, (yy - 3)/2, false);
	}
	if ((!(xx & 1)) && (!(yy & 1))) {
		u = play(true, 5, (xx - 2) / 2, (yy - 2) / 2, false);
	}
	if ((xx & 1) && (!(yy & 1))) {
		u = play(false, -1, (xx - 3)/2, (yy - 2)/2, true);
	}
	if (!u.playerMove) {
		std::cerr << yy << "x" << xx << std::endl;
		std::cerr << u.y << " " << u.x << " " << u.vert << std::endl;
	}
	this_game.make_play(u);
	dd = 1;
	this_game.debug();
	dd = 0;
}

bool temp = true;

std::pair<int, int> Action() {
	tot_step++;
	std::cerr << tot_step << "=====================" << std::endl;
	play u = this_game.computer_move();
	dd = 1;
	this_game.debug();
	dd = 0;
	if (u.playerMove) {
		return std::make_pair(u.y * 2 + 2, u.x * 2 + 2);
	}
	else {
		if (u.vert)
			return std::make_pair(u.y * 2 + 2, u.x * 2 + 3);
		else
			return std::make_pair(u.y * 2 + 3, u.x * 2 + 2);
	}


}
//sometimes it go and back again and again