//This is a Quoridor-AI program employing naive alpha-beta prunning algorithm with improved heurisitc functions.
//Author : spectrometer
#include <iostream>
#include <map>
#include <vector>
#include <cstring>
#include "submit.h"
using namespace std;
//--------------------------------Constants Defining--------------------------------
extern int ai_side;								//which side
std::string ai_name = "Icarus_Chaos(New Rule)";	//name of AI
const int BOARDSIZE = 9;						//Size of the board
const int DEPTH[11] = {1,2,2,2,2,2,2,2,2,2,2};							//Depth of minimax search
const int MYAI = 0;								//Representation of my AI
const int OPPONENT = 1;							//Representation of my opponent
const int INF = 2147483647;					//Infinity
const pair<int, int> DEFAULTACTION = make_pair(1, 1); //Default action which is illegal
const int deltax[12] = {0, 0, -1, 1, -1, 1, -1, 1, 0, 0, -2, 2};
const int deltay[12] = {-1, 1, 0, 0, -1, 1, 1, -1, -2, 2, 0, 0}; // Delta of pawn moving
const int LEFT = 0;
const int RIGHT = 1;
const int UP = 2;
const int DOWN = 3;
const int UPLEFT = 4;
const int DOWNRIGHT = 5;
const int UPRIGHT = 6;
const int DOWNLEFT = 7;
const int LEFTLEFT = 8;
const int RIGHTRIGHT = 9;
const int UPUP = 10;
const int DOWNDOWN = 11;						//Representatives of moving directions
//--------------------------------Variables Defining--------------------------------
//type Action
//<int, <int, int>> : <evaluation, <moveposx, moveposy>>
//moveposx, moveposy:
//	(even, even) --> pawn moving
// 	(even, odd)  --> vertitcal wall placing
//	(odd, even)  --> horrizontal wall placing
//  (odd, odd)   --> illegal action
typedef pair<int, pair<int, int> > Move;
Move bestaction;
int NOWDEPTH;
bool debugflag = true;

//type QuoridorBoard
class QuoridorBoard{
public:
	pair<int, int> location[2];
	int wallcnt[2], destrow[2];
	int walls[25][25];
	int vis[25][25];
	pair<int, int> q[100]; 
	int head, tail;
 
	inline Move MakeAction(int evaluation, pair<int, int> move){
		return make_pair(evaluation, move);
	}
	inline bool IsMovingThePawn(pair<int, int> move){
		return (!(move.first & 1)) && (!(move.second & 1));
	}
	inline bool IsVertical(pair<int, int> move){
		return (!(move.first & 1)) && (move.second & 1);
	}
	inline bool IsInsideTheBoard(pair<int, int> move){
		int x = move.first, y = move.second;
		return (2 <= x) && (x <= 18) && (2 <= y) && (y <= 18);
	}
	inline pair<int, int> CalDeltaPawnPosition(pair<int, int> now, int direction){
		return make_pair(now.first + deltax[direction], now.second + deltay[direction]);
	}
	inline bool IsOccupiedByWall(pair<int, int> now, int direction){
		return walls[now.first + deltax[direction]][now.second + deltay[direction]];
	}
	inline bool IsLegalPawnMoving(pair<int, int> now, pair<int, int> destination, int direction, int player){
		if (location[player ^ 1] == destination) return false;
		if (!IsInsideTheBoard(destination)) return false;
		//up down left right
		if (direction < 4) return !IsOccupiedByWall(now, direction);
		else if (direction < 8){
			//upright
			if (direction == UPRIGHT){
				//opponent in up direction
				if (location[player ^ 1] == CalDeltaPawnPosition(now, UP)){
					if (!IsOccupiedByWall(location[player ^ 1], UP)) return false;
					if (IsOccupiedByWall(location[player ^ 1], DOWN)) return false;
					if (IsOccupiedByWall(location[player ^ 1], RIGHT)) return false;
					return true;
				}
				//opponent in right direction
				if (location[player ^ 1] == CalDeltaPawnPosition(now, RIGHT)){
					if (!IsOccupiedByWall(location[player ^ 1], RIGHT)) return false;
					if (IsOccupiedByWall(location[player ^ 1], LEFT)) return false;
					if (IsOccupiedByWall(location[player ^ 1], UP)) return false;
					return true;
				}
				return false;
			}
			//upleft
			if (direction == UPLEFT){
				//opponent in up direction
				if (location[player ^ 1] == CalDeltaPawnPosition(now, UP)){
					if (!IsOccupiedByWall(location[player ^ 1], UP)) return false;
					if (IsOccupiedByWall(location[player ^ 1], DOWN)) return false;
					if (IsOccupiedByWall(location[player ^ 1], LEFT)) return false;
					return true;
				}
				//opponent in left direction
				if (location[player ^ 1] == CalDeltaPawnPosition(now, LEFT)){
					if (!IsOccupiedByWall(location[player ^ 1], LEFT)) return false;
					if (IsOccupiedByWall(location[player ^ 1], UP)) return false;
					if (IsOccupiedByWall(location[player ^ 1], RIGHT)) return false;
					return true;
				}
				return false;
			}
			//downright
			if (direction == DOWNRIGHT){
				//opponent in down direction
				if (location[player ^ 1] == CalDeltaPawnPosition(now, DOWN)){
					if (!IsOccupiedByWall(location[player ^ 1], DOWN)) return false;
					if (IsOccupiedByWall(location[player ^ 1], UP)) return false;
					if (IsOccupiedByWall(location[player ^ 1], RIGHT)) return false;
					return true;
				}
				//opponent in right direction
				if (location[player ^ 1] == CalDeltaPawnPosition(now, RIGHT)){
					if (!IsOccupiedByWall(location[player ^ 1], RIGHT)) return false;
					if (IsOccupiedByWall(location[player ^ 1], DOWN)) return false;
					if (IsOccupiedByWall(location[player ^ 1], LEFT)) return false;
					return true;					
				}
				return false;
			}
			//downleft
			if (direction == DOWNLEFT){
				//opponent in down direction
				if (location[player ^ 1] == CalDeltaPawnPosition(now, DOWN)){
					if (!IsOccupiedByWall(location[player ^ 1], DOWN)) return false;
					if (IsOccupiedByWall(location[player ^ 1], LEFT)) return false;
					if (IsOccupiedByWall(location[player ^ 1], UP)) return false;
					return true;
				}
				//opponent in left direction
				if (location[player ^ 1] == CalDeltaPawnPosition(now, LEFT)){
					if (!IsOccupiedByWall(location[player ^ 1], LEFT)) return false;
					if (IsOccupiedByWall(location[player ^ 1], DOWN)) return false;
					if (IsOccupiedByWall(location[player ^ 1], RIGHT)) return false;
					return true;					
				}
				return false;
			}
		}else{
			//upup, downdown, leftleft, rightright
			return  (location[player ^ 1] == CalDeltaPawnPosition(now, direction))
		 	 	  &&(!IsOccupiedByWall(now, direction - 8))
		 	 	  &&(!IsOccupiedByWall(location[player ^ 1], direction - 8));
		}
	}
	inline int BreadthFirstSearch(int player){
		pair<int, int> tmplocation = location[player ^ 1];
		location[player ^ 1] = make_pair(1, 1);
		memset(vis, -1, sizeof(vis)); 
		q[head = tail = 1] = location[player];
		vis[location[player].first][location[player].second] = 0;
		for (; head <= tail; head++){
			pair<int, int> now = q[head];
			for (int index = 0; index < 12; index++){
				int newx = now.first + (deltax[index] << 1), newy = now.second + (deltay[index] << 1);
				if (vis[newx][newy] == -1 && IsLegalPawnMoving(now, make_pair(newx, newy), index, player)){
					q[++tail] = make_pair(newx, newy);
					vis[newx][newy] = vis[now.first][now.second] + 1;
				}
			}
		}
		location[player ^ 1] = tmplocation;
		int minlength = INF;
		for (int col = 1; col <= 18; col++) 
			if (vis[destrow[player]][col] != -1) 
				minlength = min(minlength, vis[destrow[player]][col]);
		return minlength;
	}
	inline bool IsLegalWallPlacing(pair<int, int> move, int player){
		int x = move.first, y = move.second;
		if (IsVertical(move)){
			if (walls[x][y] || walls[x + 1][y] || walls[x + 2][y]) return false;
			walls[x][y] = walls[x + 1][y] = walls[x + 2][y] = 1;
			int minlength = max(BreadthFirstSearch(player), BreadthFirstSearch(player ^ 1));
			walls[x][y] = walls[x + 1][y] = walls[x + 2][y] = 0;
			return minlength != INF;
		}else{
			if (walls[x][y] || walls[x][y + 1] || walls[x][y + 2]) return false;
			walls[x][y] = walls[x][y + 1] = walls[x][y + 2] = 1;
			int minlength = max(BreadthFirstSearch(player), BreadthFirstSearch(player ^ 1));
			walls[x][y] = walls[x][y + 1] = walls[x][y + 2] = 0;
			return minlength != INF;
		}
	}
	inline void GeneratePawnMoving(vector<Move> &listofactions, int player){
		for (int index = 0; index < 12; index++){
			int x = location[player].first + (deltax[index] << 1);
			int y = location[player].second + (deltay[index] << 1);
			if (IsLegalPawnMoving(location[player], make_pair(x, y), index, player))
				listofactions.push_back(MakeAction(-1, make_pair(x, y)));
		}
	}
	inline void GenerateWallPlacing(vector<Move> &listofactions, int player){
		for (int row = 2; row <= 17; row++)
			for (int col = 3 - (row & 1); col <= 17; col += 2){
				if (IsLegalWallPlacing(make_pair(row, col), player)) 
					listofactions.push_back(MakeAction(-1, make_pair(row, col)));
			}
	}
	//Initialize the Board
	inline void Initialize(){
		//Set the coordinate of the pawn and its destination
		location[ai_side] = make_pair(2, 10);
		location[ai_side ^ 1] = make_pair(18, 10);
		destrow[ai_side] = 18;
		destrow[ai_side ^ 1] = 2;
		//Set the wallcounts
		wallcnt[0] = wallcnt[1] = 10;
		//Set the wallplacements
		memset(walls, 0, sizeof(walls));
		for (int i = 1; i <= 19; i++) walls[1][i] = walls[19][i] = walls[i][1] = walls[i][19] = 1;
	}
	//Generate all possible actions for the board
	inline void GenerateAction(vector<Move> &listofactions, int player){
		//Moving the pawn
		GeneratePawnMoving(listofactions, player);
		//Placing walls
		if (wallcnt[player] != 0) GenerateWallPlacing(listofactions, player);
	}
	//Apply the action to the board
	inline void MakeAction(pair<int, int> move, int player){
		if (IsMovingThePawn(move)) location[player] = move;
		else{
			wallcnt[player]--;
			int x = move.first, y = move.second;
			if (IsVertical(move)) walls[x][y] = walls[x + 1][y] = walls[x + 2][y] = 1;
			else walls[x][y] = walls[x][y + 1] = walls[x][y + 2] = 1;
		}
	}
	//Show the board state for debug
	inline void Show(){
		cerr << "myloc : " << location[MYAI].first << " " << location[MYAI].second << endl;
		cerr << "oploc : " << location[OPPONENT].first << " " << location[OPPONENT].second << endl;
		cerr << "mydestrow : " << destrow[MYAI] << endl;
		cerr << "opdestrow : " << destrow[OPPONENT] << endl;
		cerr << "mywallcnt : " << wallcnt[MYAI] << endl;
		cerr << "opwallcnt : " << wallcnt[OPPONENT] << endl;
		//for (int i = 1; i <= 19; i++){
		//	for (int j = 1; j <= 19; j++) cerr << walls[i][j] << " ";
		//	cerr << endl;
		//}
	}
	//Calculate the heuristic evaluation for the board
	inline double Evaluation(){
		int opdis = BreadthFirstSearch(OPPONENT), mydis = BreadthFirstSearch(MYAI);
		//Win/Lose Game Check
		if (opdis == 0) return -10000.0;
		if (mydis == 0) return 10000.0;
		//Show();
		//if (debugflag) cerr << "dis : " << opdis << " " << mydis; 
		double evalofpathdifference = opdis - mydis;
		double evalofbarrier = wallcnt[MYAI] - wallcnt[OPPONENT];
		return evalofpathdifference + evalofbarrier * 0.8;
	}
};
QuoridorBoard board;
//--------------------------------Functions for Debug---------------------------------------
inline void ShowList(vector<Move> list){
	for (auto action : list) 
		cerr << action.second.first << " " << action.second.second << " | ";
	cerr << endl; 
}
//--------------------------------Initialize the Board State--------------------------------
void init() {
	board.Initialize();
}
//--------------------------------Maintain the board--------------------------------
void GetUpdate(std::pair<int, int> location) {
	board.MakeAction(location, OPPONENT);
}
//--------------------------------Calculate the next step--------------------------------
//Minimax search with alpha-beta prunning
inline double AlphaBeta(QuoridorBoard state, int player, int depth, double alpha, double beta){
	if (depth == 0) return state.Evaluation();
	if (state.location[MYAI].first == state.destrow[MYAI]) return 10000.0 + depth;
	if (state.location[OPPONENT].first == state.destrow[OPPONENT]) return -10000.0 - depth;

	vector<Move> listofactions;
	state.GenerateAction(listofactions, player);
		
	//if (debugflag) state.Show();
	//if (debugflag) cerr << "List of actions :\n";
	//if (debugflag) ShowList(listofactions);

	if (player == OPPONENT){
		for (auto action : listofactions){
			QuoridorBoard newstate = state; 
			newstate.MakeAction(action.second, player);
			//if (debugflag) cerr << "---------------------player = " << player << " depth = " << depth << "--------------------" << endl;
			//if (debugflag) state.Show();
			//if (debugflag) cerr << "Action :" << action.second.first << " " << action.second.second << endl;
			double score = AlphaBeta(newstate, player ^ 1, depth - 1, alpha, beta);
			//if (debugflag) cerr << "Move : " << action.second.first << " " << action.second.second << " score : " << score << endl;
			if (score < beta){
				beta = score;
				if (depth == NOWDEPTH) bestaction = action;
				if (alpha >= beta) return alpha; 
			}
		}
		return beta;
	}else{
		for (auto action : listofactions){
			QuoridorBoard newstate = state; 
			newstate.MakeAction(action.second, player);
			//if (debugflag) cerr << "---------------------player = " << player << " depth = " << depth << "--------------------" << endl;
			//if (debugflag) state.Show();
			//if (debugflag) cerr << "Action :" << action.second.first << " " << action.second.second << endl;
			double score = AlphaBeta(newstate, player ^ 1, depth - 1, alpha, beta);
			//if (debugflag) cerr << "Move : " << action.second.first << " " << action.second.second << " score : " << score << endl;
			if (score > alpha){
				alpha = score;
				if (depth == NOWDEPTH) bestaction = action;
				if (alpha >= beta) return beta;
			}
		}
		return alpha;
	}
}
//Get Action
std::pair<int, int> Action() {
	//cerr << "Next Round-------------------------------" << endl;
	//if (board.location[MYAI] == make_pair(16, 2)) debugflag = true;
	//else debugflag = false;
	if (ai_side == 0){
		NOWDEPTH = DEPTH[min(board.wallcnt[0], board.wallcnt[1])];
	}else{
		NOWDEPTH = DEPTH[max(board.wallcnt[0], board.wallcnt[1])];
	}
	double bestscore = AlphaBeta(board, MYAI, NOWDEPTH, -INF, INF);
	board.MakeAction(bestaction.second, MYAI);
	return bestaction.second;
}