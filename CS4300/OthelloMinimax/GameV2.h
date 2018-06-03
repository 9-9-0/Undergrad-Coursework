#ifndef GAMEV2_H_
#define GAMEV2_H_

#include "Board.h"
#include "Minimax.h"

class Minimax;

class Game
{
private:
	//Board + MiniMax Related Variables
	Board* currentBoard;
	Board* tempBoard;
	int boardDim;
	int humanPlayer; //1 if human player is Black (X), 2 if human player is White (0);
	int computerPlayer;

	//Game Variables, update with each ply
	int movesCount;
	int wScore;
	int bScore;
	int playerTurn; //1 for p1 (X), 2 for p2(O)

	//Along with the containers, boolean values updated in hasMovesLeft()
	bool movesLeftP1;
	bool movesLeftP2;
	//Each of these containers get updated with each passing ply in hasMovesLeft()
	std::vector<std::vector<int> > p1Moves;
	std::vector<std::vector<int> > p2Moves;

	//Game Initialization
	void setDim();
	void iniBoard();
	void setPlayer();

	//Update Functions
	void scoreBoard();
	void scoreGame();
	void printScore();
	void dupBoard();

	//Gameplay Logic Functions
	void changePlayer();
	bool hasMovesLeft(int player);
	int getMoveR(int player);
	int getMoveC(int player);
	bool isValidMove(int r, int c, int player);	//Repetitive, contents can be replaced with a call to Board.getValidMoves() and comparing contents, but leaving for now
	void runPly();
	void runPly(int r, int c);
	void playPiece(char color, int r, int c);

public:
	Game();
	~Game();

	Minimax* computer;
	void play();
};



#endif /* GAMEV2_H_ */
