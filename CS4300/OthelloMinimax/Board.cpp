#include "Board.h"

/******************************************************************************
* Name    : Board()
* Purpose : Default Constructor. For rare situations in which a Board is created
* 			and immediately modified.
* Inputs  : None.
* Outputs : None.
******************************************************************************/
Board::Board()
{
	dim = 0;
	boardState = 0;
}

/******************************************************************************
* Name    : Board(int x)
* Purpose : Calls the function to set instance's dimension and sets boardState.
* Inputs  : Integer 'x', the dimension of the board
* Outputs : None.
******************************************************************************/
Board::Board(int x)
{
	setDim(x);

	boardState = new char*[dim];

	for (int i = 0; i < dim ; i++)
	{
		boardState[i] = new char[dim];
		for (int j = 0; j < dim; j++)
		{
			boardState[i][j] = 'e';
		}
	}
};

/******************************************************************************
* Name    : ~Board()
* Purpose : Deallocates the memory occupied by boardState
* Inputs  : None.
* Outputs : None.
******************************************************************************/
Board::~Board()
{
	for (int i = 0; i < dim; i++)
	{
		delete [] boardState[i];
	}

	delete [] boardState; //Check if this works
};

/******************************************************************************
* Name    : void printBoard()
* Purpose : Prints the content of the Board instance.
* Inputs  : None.
* Outputs : Console output of the Board's 2D character array.
* Notes	  :
* 	- Does not properly format boards of 10x10 and greater, can be modified later
******************************************************************************/
void Board::printBoard()
{
	std::cout << "\nCurrent Board:\n";
	std::cout << "----" << std::setfill('-') << std::setw(dim * 4) << "\n";

	//Print Top Row
	std::cout << "  | ";
	for (int i = 0; i < dim; i++)
	{
		std::cout << i << " | ";
	}
	std::cout << "\n";
	std::cout << "----" << std::setfill('-') << std::setw(dim * 4) << "\n";

	//Print Rest of Rows
	for (int i = 0; i < dim; i++)
	{
		std::cout << i << " | ";

		for (int j = 0; j < dim; j++)
		{
			if (boardState[i][j] == 'e')
			{
				std::cout << "  | ";
			}
			else if (boardState[i][j] == 'b')
			{
				std::cout << "\u25CF" << " | ";
			}
			else
			{
				std::cout << "\u25CB" << " | ";
			}
		}

		std::cout << "\n----" << std::setfill('-') << std::setw(dim * 4) << "\n";
	}
	std::cout << "\n";
}

/******************************************************************************
* Name    : Overloaded =
* Purpose : Allows for deep copying between Board instances.
* Inputs  : Board to be copied.
* Outputs : None.
******************************************************************************/
void Board::operator = (const Board &obj)
{
	dim = obj.dim;
	boardState = new char*[dim];

	for (int i = 0; i < dim; i++)
	{
		boardState[i] = new char[dim];
		for (int j = 0; j < dim; j++)
		{
			boardState[i][j] = obj.boardState[i][j];
		}
	}
}

/******************************************************************************
* Name    : void setBoard(char** newState)
* Purpose : Deep copies the input into Board instance's boardState.
* Inputs  : A 2D character array.
* Outputs : None.
******************************************************************************/
void Board::setBoard(char** newState)
{
	for (int i = 0; i < dim; i++)
	{
		for (int j = 0; j < dim; j++)
		{
			boardState[i][j] = newState[i][j];
		}
	}
};

/******************************************************************************
* Name    : setSinglePiece(char color, int r, int c)
* Purpose : Sets a single piece at 'r', 'c' to the color input.
* Inputs  : color ('b' or 'w'), row and column integers.
* Outputs : None.
******************************************************************************/
void Board::setSinglePiece(char color, int r, int c)
{
	boardState[r][c] = color;
}

/******************************************************************************
* Name    : flipSinglePiece(int r, int c)
* Purpose : Flips a single piece located at 'r', 'c' to its opposite color.
* Inputs  : Row and column integers.
* Outputs : None.
* Notes	  :
* 	- This function is used in conjunction with flipScan(), never alone.
******************************************************************************/
void Board::flipSinglePiece(int r, int c)
{
	if (boardState[r][c] == 'b')
	{
		boardState[r][c] = 'w';
	}
	else if (boardState[r][c] == 'w')
	{
		boardState[r][c] = 'b';
	}
	else
	{
		return;
	}
}

/******************************************************************************
* Name    : getState()
* Purpose : Gets the Board's current boardState.
* Inputs  : None.
* Outputs : 2D array stored in instance's boardState variable.
******************************************************************************/
char** Board::getState()
{
	return boardState;
};

/******************************************************************************
* Name    : getDim()
* Purpose : Returns dimension of Board.
* Inputs  : None.
* Outputs : Integer stored in instance's dim variable.
******************************************************************************/
int Board::getDim()
{
	return this->dim;
};

/******************************************************************************
* Name    : setDim(int x)
* Purpose : Sets the dimension of the board to x by x. Used by Board(int x).
* Inputs  : Integer x for the max dimension.
* Outputs : None.
******************************************************************************/
void Board::setDim(int x)
{
	this->dim = x;
};

/******************************************************************************
* Name    : isOnBoard(int r, int c)
* Purpose : Returns true if the position 'r', 'c' is a valid position on the Board.
* Inputs  : Integers 'r' and 'c', denoting the position.
* Outputs : True or False.
******************************************************************************/
bool Board::isOnBoard(int r, int c)
{
	if (r < 0 || c < 0 || (r > dim - 1) || (c > dim - 1))
		{
			return false;
		}
		else
		{
			return true;
		}
}

/******************************************************************************
* Name    : isValidMove(int toR, int toC, char color)
* Purpose : Returns true if the position 'toR', 'toC' is a playable position by
* 			the	piece of color 'color' ('b' or 'w').
* Inputs  : Integers 'toR', 'toC' denoting location of piece to be played.
* 			'color' denoting the color of the piece to be played.
* Outputs : True or False.
******************************************************************************/
bool Board::isValidMove(int toR, int toC, char color)
{
	std::vector<std::vector<int> > output(0);
	output.clear();

	//Are the dimensions valid?
	if (!isOnBoard(toR, toC))
	{
		return false;
	}
	//Space not empty?
	if (boardState[toR][toC] != 'e')
	{
		return false;
	}

	//Create container of moves
	std::vector<int> temp(0);

	//Try all directions
	for (int i = 0; i < 8; i++)
	{
		temp = flipScan(toR, toC, color, i, false);
		output.push_back(temp);

		//std::cout << output[i].size() << std::endl;
		if (temp.size() > 0)
		{
			return true;
		}
	}

	return false;
}

/******************************************************************************
* Name    : flipScan(int toR, int toC, char color, int direction, bool flip)
* Purpose : Flips pieces in response to a player's move, or simply scans for whether
* 			or not pieces are flipped by a player's move (used for move validation).
* Inputs  : Integers 'toR', 'toC' denoting the location of the piece to be played.
* 			Char 'color' denoting the color of the piece to be played.
* 			Integer 'direction' (0 for N, 1 for NE, 2 for E...7 for NW).
* 			Boolean 'flip'. If True, all piece positions stored in the vector get
* 			flipped and the output vector is emptied. If False, simply returns the
* 			positions of pieces that would have been flipped.
* Outputs : Returns an even-sized vector of the grid positions of flipped pieces
******************************************************************************/
std::vector<int> Board::flipScan(int toR, int toC, char color, int direction, bool flip)
{
	std::vector<int> output(0); //Valid grid position contents: r1, c1, r2, c2,...rn,,cn. Always should return even size.
	output.clear();

	int i = toR;
	int j = toC;

	char oppColor;
	if (color == 'b')
	{
		oppColor = 'w';
	}
	else
	{
		oppColor = 'b';
	}

	switch(direction)
	{
	//N
	case 0:
		i--;
		if (!isOnBoard(i, j)) { return output; }

		while (boardState[i][j] == oppColor)
		{
			output.push_back(i);
			output.push_back(j);

			i--;
			if (!isOnBoard(i, j))
			{
				output.clear();
				return output;
			}
		}

		if (output.size() > 0 && boardState[i][j] == color) //At least one was flipped and was encapsulated
		{
			if (flip)
			{
				//Flip the pieces of temp board
				int y;
				int x;
				while (output.size() > 0)
				{
					y = output.back();
					output.pop_back();
					x = output.back();
					output.pop_back();
					flipSinglePiece(x, y);
				}
			}
			return output;
		}
		else
		{
			output.clear();
			return output;
		}
		break;
	//NE
	case 1:
		i--;
		j++;
		if (!isOnBoard(i, j)) { return output; }

		while (boardState[i][j] == oppColor)
		{
			output.push_back(i);
			output.push_back(j);

			i--;
			j++;
			if (!isOnBoard(i, j))
			{
				output.clear();
				return output;
			}
		}

		if (output.size() > 0 && boardState[i][j] == color) //At least one was flipped and was encapsulated
		{
			if (flip)
			{
				//Flip the pieces of temp board
				int y;
				int x;
				while (output.size() > 0)
				{
					y = output.back();
					output.pop_back();
					x = output.back();
					output.pop_back();
					flipSinglePiece(x, y);
				}
			}
			return output;
		}
		else
		{
			output.clear();
			return output;
		}
		break;
	//E
	case 2:
		j++;
		if (!isOnBoard(i, j)) { return output; }

		while (boardState[i][j] == oppColor)
		{
			output.push_back(i);
			output.push_back(j);

			j++;
			if (!isOnBoard(i, j))
			{
				output.clear();
				return output;
			}
		}

		if (output.size() > 0 && boardState[i][j] == color) //At least one was flipped and was encapsulated
		{
			if (flip)
			{
				//Flip the pieces of temp board
				int y;
				int x;
				while (output.size() > 0)
				{
					y = output.back();
					output.pop_back();
					x = output.back();
					output.pop_back();
					flipSinglePiece(x, y);
				}
			}
			return output;
		}
		else
		{
			output.clear();
			return output;
		}
		break;
	//SE
	case 3:
		i++;
		j++;
		if (!isOnBoard(i, j)) { return output; }

		while (boardState[i][j] == oppColor)
		{
			output.push_back(i);
			output.push_back(j);

			i++;
			j++;
			if (!isOnBoard(i, j))
			{
				output.clear();
				return output;
			}
		}

		if (output.size() > 0 && boardState[i][j] == color) //At least one was flipped and was encapsulated
		{
			if (flip)
			{
				//Flip the pieces of temp board
				int y;
				int x;
				while (output.size() > 0)
				{
					y = output.back();
					output.pop_back();
					x = output.back();
					output.pop_back();
					flipSinglePiece(x, y);
				}
			}
			return output;
		}
		else
		{
			output.clear();
			return output;
		}
		break;
	//S
	case 4:
		i++;
		if (!isOnBoard(i, j)) { return output; }

		while (boardState[i][j] == oppColor)
		{
			output.push_back(i);
			output.push_back(j);

			i++;
			if (!isOnBoard(i, j))
			{
				output.clear();
				return output;
			}
		}

		if (output.size() > 0 && boardState[i][j] == color) //At least one was flipped and was encapsulated
		{
			if (flip)
			{
				//Flip the pieces of temp board
				int y;
				int x;
				while (output.size() > 0)
				{
					y = output.back();
					output.pop_back();
					x = output.back();
					output.pop_back();
					flipSinglePiece(x, y);
				}
			}
			return output;
		}
		else
		{
			output.clear();
			return output;
		}
		break;
	//SW
	case 5:
		i++;
		j--;
		if (!isOnBoard(i, j)) { return output; }

		while (boardState[i][j] == oppColor)
		{
			output.push_back(i);
			output.push_back(j);

			i++;
			j--;
			if (!isOnBoard(i, j))
			{
				output.clear();
				return output;
			}
		}

		if (output.size() > 0 && boardState[i][j] == color) //At least one was flipped and was encapsulated
		{
			if (flip)
			{
				//Flip the pieces of temp board
				int y;
				int x;
				while (output.size() > 0)
				{
					y = output.back();
					output.pop_back();
					x = output.back();
					output.pop_back();
					flipSinglePiece(x, y);
				}
			}
			return output;
		}
		else
		{
			output.clear();
			return output;
		}
		break;
	//W
	case 6:
		j--;
		if (!isOnBoard(i, j)) { return output; }

		while (boardState[i][j] == oppColor)
		{
			output.push_back(i);
			output.push_back(j);

			j--;
			if (!isOnBoard(i, j))
			{
				output.clear();
				return output;
			}
		}

		if (output.size() > 0 && boardState[i][j] == color) //At least one was flipped and was encapsulated
		{
			if (flip)
			{
				//Flip the pieces of temp board
				int y;
				int x;
				while (output.size() > 0)
				{
					y = output.back();
					output.pop_back();
					x = output.back();
					output.pop_back();
					flipSinglePiece(x, y);
				}
			}
			return output;
		}
		else
		{
			output.clear();
			return output;
		}
		break;
	//NW
	case 7:
		i--;
		j--;
		if (!isOnBoard(i, j)) { return output; }

		while (boardState[i][j] == oppColor)
		{
			output.push_back(i);
			output.push_back(j);

			i--;
			j--;
			if (!isOnBoard(i, j))
			{
				output.clear();
				return output;
			}
		}

		if (output.size() > 0 && boardState[i][j] == color) //At least one was flipped and was encapsulated
		{
			if (flip)
			{
				//Flip the pieces of temp board
				int y;
				int x;
				while (output.size() > 0)
				{
					y = output.back();
					output.pop_back();
					x = output.back();
					output.pop_back();
					//std::cout << x << y << std::endl;
					flipSinglePiece(x, y);
				}
			}
			return output;
		}
		else
		{
			output.clear();
			return output;
		}
		break;
	}

	std::cout << "flipScan() bottomed out\n";
	return output;
}

/******************************************************************************
* Name    : getValidMoves(char color)
* Purpose :	Returns a vector containing all valid moves of a given boardState
* 			for a given player.
* Inputs  : Char 'color' denoting the color of the piece to be played.
* Outputs : A vector of size two integer vectors.
******************************************************************************/
std::vector<std::vector<int> > Board::getValidMoves(char color)
{
	std::vector<std::vector<int> > output; //Contains all valid moves for the given color @ a certain ply
	std::vector<int> validMove; 		   //Temporary vector holding a single move, gets pushed into output
	output.clear();
	validMove.clear();

	for (int i = 0; i < dim; i++)
	{
		for (int j = 0; j < dim; j++)
		{
			if (isValidMove(i, j, color))
			{
				validMove.push_back(i);
				validMove.push_back(j);
				output.push_back(validMove);
			}
			validMove.clear();
		}
	}

	return output;
}
