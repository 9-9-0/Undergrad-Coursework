#include "GameV2.h"

/******************************************************************************
* Name    : setDim()
* Purpose : Prompts user to enter in the dimension of the board
* Inputs  : Console input of an even integer greater than 2.
* Outputs : None.
* Notes	  :
* 	- Currently does not correctly format board sizes of double digit dimensions.
******************************************************************************/
void Game::setDim()
{
	//Get board dimensions
	int userDim;

	//Get dimensions of the board
	std::cout << "Enter number of rows (must be even): ";
	std::cin >> userDim;

	while (!std::cin || (userDim < 2 || userDim % 2 != 0))
	{
		std::cout << "Input must be an even integer of at least 2, try again: ";
		std::cin.clear();
		std::cin.ignore();
		std::cin >> userDim;
	}

	boardDim = userDim;
}

/******************************************************************************
* Name    : dupBoard()
* Purpose : Deletes tempBoard if it exists and recreates it from currentBoard.
* Inputs  : None.
* Outputs : None.
******************************************************************************/
void Game::dupBoard()
{
	if (tempBoard != NULL)
	{
		delete tempBoard;
	}

	tempBoard = new Board(boardDim);
	*tempBoard = *currentBoard;

	return;
}

/******************************************************************************
* Name    : iniBoard()
* Purpose : Allocates memory for currentBoard, sets its initial boardState. Sets
* 			initial player turn and total moves made on the board.
* Inputs  : None.
* Outputs : None.
******************************************************************************/
void Game::iniBoard()
{
	//Create Board
	currentBoard = new Board(boardDim);

	char** temp = new char*[boardDim];
	for (int i = 0; i < boardDim; i++)
	{
		temp[i] = new char[boardDim];
	}

	for (int i = 0; i < boardDim; i++)
	{
		for (int j = 0; j < boardDim; j++)
		{
			temp[i][j] = currentBoard->getState()[i][j];
			//std::cout << temp[i][j];
		}
		//std::cout << "\n";
	}

	//Set initial 4 pieces
	temp[boardDim/2 - 1][boardDim/2 - 1] = 'w';
	temp[boardDim/2 - 1][boardDim/2] = 'b';
	temp[boardDim/2][boardDim/2 - 1] = 'b';
	temp[boardDim/2][boardDim/2] = 'w';

	currentBoard->setBoard(temp);


	//Delete Temp 2-D Array
	for (int i = 0; i < boardDim; i++)
	{
		delete [] temp[i];
	}
	delete [] temp;

	//Set Black as initial ply && Set movesCount
	playerTurn = 1;
	movesCount = 0;
}

/******************************************************************************
* Name    : setPlayer()
* Purpose : Prompts the user for his choice of playing the first or second move.
* Inputs  : None.
* Outputs : None.
******************************************************************************/
void Game::setPlayer()
{
	int playerChoice;
	std::cout << "\nChoose your color. Enter 1 for Black (first move of the game), 2 for White (second move of the game): ";
	std::cin >> playerChoice;

	while (!std::cin || (playerChoice != 1 && playerChoice != 2))
	{
		std::cout << "Input must be either 1 or 2 , try again: ";
		std::cin.clear();
		std::cin.ignore();
		std::cin >> playerChoice;
	}

	//Set Human Player #
	humanPlayer = playerChoice;

	//Set Computer Player #
	if (humanPlayer == 1)
	{
		computerPlayer = 2;
	}
	else
	{
		computerPlayer = 1;
	}
}

/******************************************************************************
* Name    : Game()
* Purpose : Creates tempBoard, calls setDim(), setPlayer(), iniBoard(), dupBoard(),
* 			and sets the moves-left flags for player 1 and 2.
* Inputs  : None.
* Outputs : None.
******************************************************************************/
Game::Game()
{
	//Set Temp Board to Null
	tempBoard = NULL;

	setDim();
	setPlayer();
	iniBoard();
	dupBoard();

	//Set initial moves left
	movesLeftP1 = hasMovesLeft(1);
	movesLeftP2 = hasMovesLeft(2);
}

/******************************************************************************
* Name    : ~Game()
* Purpose : Deletes currentBoard and tempBoard.
* Inputs  : None.
* Outputs : None.
******************************************************************************/
Game::~Game()
{
	delete this->currentBoard;
	delete this->tempBoard;
}

/******************************************************************************
* Name    : getMoveR(int player) and getMoveC(int player)
* Purpose : Prompts the player (either 1 or 2) for the row and column numbers of
* 			the grid position they wish to play.
* Inputs  : Integer 'player' of value 1 or 2.
* Outputs : Integer value of the row or column position entered.
* Notes	  :
* 	- This function contains old code from when the game was human versus human.
* 	  The arguments are unnecessary as the two functions are only run during a
* 	  human's turn.
******************************************************************************/
int Game::getMoveR(int player)
{
	int output;
	std::cout << "Enter row: ";
	std::cin >> output;
	while (!std::cin)
	{
		std::cout << "Input must be an integer, try again: ";
		std::cin.clear();
		std::cin.ignore();
		std::cin >> output;
	}
	while ((output > boardDim - 1) || (output < 0))
	{
		std::cout << "Input must be between 0 and " << boardDim - 1 << ", try again: ";
		std::cin.clear();
		std::cin.ignore();
		std::cin >> output;
	}
	return output;
}

int Game::getMoveC(int player)
{
	int output;
	std::cout << "Enter column: ";
	std::cin >> output;
	while (!std::cin)
	{
		std::cout << "Input must be an integer, try again: ";
		std::cin.clear();
		std::cin.ignore();
		std::cin >> output;
	}
	while ((output > boardDim - 1) || (output < 0))
	{
		std::cout << "Input must be between 0 and " << boardDim - 1 << ", try again: ";
		std::cin.clear();
		std::cin.ignore();
		std::cin >> output;
	}
	return output;
}

/******************************************************************************
* Name    : changePlayer()
* Purpose : Called after plies are successfully played. Changes the value of
* 			playerTurn to the player who is to play the next turn.
* Inputs  : None.
* Outputs : None.
******************************************************************************/
void Game::changePlayer()
{
	if (playerTurn == 1)
	{
		playerTurn = 2;
	}
	else
	{
		playerTurn = 1;
	}
}

/******************************************************************************
* Name    : runPly()
* Purpose : Increments movesCount, prompts the human player for a move until a
* 			valid one is entered. Plays the move, updates currentBoard to
* 			tempBoard, and finally prints currentBoard.
* Inputs  : None.
* Outputs : None.
******************************************************************************/
void Game::runPly()
{
	movesCount++;
	char color;
	if (playerTurn == 1)
	{
		color = 'b';
	}
	else
	{
		color = 'w';
	}

	std::cout << "Player " << playerTurn << "'s Turn: \n";
	std::cout << "---------------\n"; //15 Dashes

	int r = getMoveR(playerTurn);
	int c = getMoveC(playerTurn);

	while (!isValidMove(r, c, playerTurn))
	{
		std::cout << "\nInvalid move entered. Try again: \n";
		r = getMoveR(playerTurn);
		c = getMoveC(playerTurn);
	}
	playPiece(color, r, c);
	currentBoard->setBoard(tempBoard->getState());
	dupBoard();

	printScore();
	currentBoard->printBoard();
}

/******************************************************************************
* Name    : runPly(int r, int c)
* Purpose : Performs the same actions as runPly() for the human does, except
* 			instead of prompting for the position location, the location is
* 			entered as arguments as determined by the Minimax algorithm.
* Inputs  : Integers 'r' and 'c' denoting location of grid to be played.
* Outputs : None.
******************************************************************************/
void Game::runPly(int r, int c)
{
	movesCount++;
	char color;
	if (playerTurn == 1)
	{
		color = 'b';
	}
	else
	{
		color = 'w';
	}

	std::cout << "Move made: " << r << " " << c << std::endl;
	playPiece(color, r, c);
	currentBoard->setBoard(tempBoard->getState());
	dupBoard();

	printScore();
	currentBoard->printBoard();
}

/******************************************************************************
* Name    : play()
* Purpose : Main game loop function. Performs game termination checks, player
* 			turn skips, and executes players' turns.
* Inputs  : None.
* Outputs : None.
******************************************************************************/
void Game::play()
{
	//Print board + stats
	printScore();
	currentBoard->printBoard();

	//Should only execute when board is 2x2
	if (!(movesLeftP1 || movesLeftP2))
	{
		//Execute Final Scoring
		scoreGame();
		return;
	}

	while (movesLeftP1 || movesLeftP2)
	{
		//Account for skipped turns
		if (!movesLeftP1 && playerTurn == 1)
		{
			changePlayer();
			continue;
		}
		if (!movesLeftP2 && playerTurn == 2)
		{
			changePlayer();
			continue;
		}

		if (playerTurn == humanPlayer)
		{
			runPly();
			changePlayer();
		}
		else
		{
			std::cout << "Thinking..." << std::endl;

			int computerR;
			int computerC;

			//Run Minimax
			computer = new Minimax(currentBoard, playerTurn, computerPlayer);

			std::vector<int> move = computer->minimaxIDS(computer->root);

			/*
			std::cout << "Move Returned: " << move[0] << " " << move[1] << std::endl;
			std::cout << "Move's Fitness: " << move[2] << std::endl;
			std::cout << "Depth at which discovered: " << move[3] << std::endl;
			*/

			computerR = move[0];
			computerC = move[1];

			//If root ends up returning its value, just choose the first available move
			//This might be screwing up the playing efficiency...
			if (computerR == -1 && computerC == -1)
			{
				char color;
				if (computerPlayer == 1)
				{
					color = 'b';
				}
				else
				{
					color = 'w';
				}
				std::vector<std::vector<int> > temp = currentBoard->getValidMoves(color);
				computerR = temp[0][0];
				computerC = temp[0][1];
			}

			runPly(computerR, computerC);
			changePlayer();

			delete computer;
		}

		//Check for moves left for either player
		movesLeftP1 = hasMovesLeft(1);
		movesLeftP2 = hasMovesLeft(2);

	}

	//Execute Final Scoring
	scoreGame();
	return;
}

/******************************************************************************
* Name    : playPiece(char color, int r, int c)
* Purpose : Calls flipScan() and setSinglePiece() on the tempBoad, passing to
* 			them the arguments it was passed.
* Inputs  : Integers 'r' and 'c' denoting location of piece played.
* 			Char 'color' denoting color of piece played.
* Outputs : None.
******************************************************************************/
void Game::playPiece(char color, int r, int c)
{
	tempBoard->setSinglePiece(color, r, c);

	for (int i = 0; i < 8; i++)
	{
		tempBoard->flipScan(r, c, color, i, true);
	}
}

/******************************************************************************
* Name    : scoreBoard()
* Purpose : Scores the board and sets bScore and wScore accordingly.
* Inputs  : None.
* Outputs : None.
******************************************************************************/
void Game::scoreBoard()
{
	int bScoreCount = 0;
	int wScoreCount = 0;
	for (int i = 0; i < boardDim; i++)
	{
		for (int j = 0; j < boardDim; j++)
		{
			if (currentBoard->getState()[i][j] == 'b')
			{
				bScoreCount++;
			}
			else if (currentBoard->getState()[i][j] == 'w')
			{
				wScoreCount++;
			}
			else
			{
				continue;
			}
		}
	}
	bScore = bScoreCount;
	wScore = wScoreCount;
}

/******************************************************************************
* Name    : printScore()
* Purpose : Prints the values held by bScore and wScore.
* Inputs  : None.
* Outputs : Console output.
******************************************************************************/
void Game::printScore()
{
	scoreBoard();

	//Print Number of Plies so far
	std::cout << "\nTotal Moves Made by Both Players: " << movesCount << "\n";

	std::cout << "Black (Player 1) Score: " << bScore << "\n";
	std::cout << "White (Player 2) Score: " << wScore << std::endl;
}

/******************************************************************************
* Name    : scoreGame()
* Purpose : Scores the terminal Board state and determines the winner.
* Inputs  : None.
* Outputs : None.
******************************************************************************/
void Game::scoreGame()
{
	std::cout << "Final Score: Black - " << bScore << " White - " << wScore << "\n";
	if (bScore > wScore)
	{
		std::cout << "Black (Player 1) Wins!";
	}
	else if (bScore < wScore)
	{
		std::cout << "White (Player 2) Wins!";
	}
	else
	{
		std::cout << "Black and White Tie!";
	}
}

/******************************************************************************
* Name    : hasMovesLeft(int player)
* Purpose : Determines whether or not player 1 or 2 has any moves left.
* Inputs  : Integer 'player' (1 or 2).
* Outputs : True or False
******************************************************************************/
bool Game::hasMovesLeft(int player)
{
	char color;
	if (player == 1)
	{
		color = 'b';
	}
	else
	{
		color = 'w';
	}

	std::vector<std::vector<int> > temp(0);
	temp = currentBoard->getValidMoves(color);

	//Return true if moves are left
	if (temp.size())
	{
		if (player == 1)
		{
			p1Moves = temp;
			return true;
		}
		else
		{
			p2Moves = temp;
			return true;
		}
	}

	return false;
}

/******************************************************************************
* Name    : isValidMove(int r, int c, int player)
* Purpose : Determines if the move played by 'player' is valid.
* Inputs  : Integers 'r' and 'c' denoting location of grid to be played.
* 			Integer 'player' (1 or 2) denoting the player whom this function
* 			is validating for.
* Outputs : True or False
* Notes	  :
* 	- Old code leftover from the human versus human version. Still being run
* 	  during runPly()'s call for the human player.
******************************************************************************/
bool Game::isValidMove(int r, int c, int player)
{
	if (player == 1)
	{
		if (p1Moves.size() == 0)
		{
			return false;
		}
		for (int i = 0; i < p1Moves.size(); i++)
		{
			if (p1Moves[i][0] == r && p1Moves[i][1] == c)
			{
				return true;
			}
		}
		return false;
	}
	if (player == 2)
	{
		if (p2Moves.size() == 0)
		{
			return false;
		}
		for (int i = 0; i < p2Moves.size(); i++)
		{
			if (p2Moves[i][0] == r && p2Moves[i][1] == c)
			{
				return true;
			}
		}
		return false;
	}

	std::cout << "isValidMove bottomed out";
	return false;
}
