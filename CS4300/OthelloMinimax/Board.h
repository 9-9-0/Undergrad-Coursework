#ifndef BOARD_H_
#define BOARD_H_

#include <vector>
#include <iostream>
#include <iomanip>

class Board {
private:
	char** boardState;
	int dim;

	bool isValidMove(int toR, int toC, char color);
	bool isOnBoard(int r, int c);
	void flipSinglePiece(int r, int c);

public:
	Board();
	Board(int x);
	~Board();

	//Overloaded Operator
	void operator= (const Board &obj);

	//Set
	void setDim(int x);
	void setBoard(char** newState);
	void setSinglePiece(char color, int r, int c);

	//Get
	int getDim();
	char** getState();
	void printBoard();
	std::vector<std::vector<int> > getValidMoves(char color);

	std::vector<int> flipScan(int toR, int toC, char color, int direction, bool flip);

};

#endif /* BOARD_H_ */
