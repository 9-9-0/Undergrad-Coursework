Notes:
------
**To be improved...**
- Based on the current utility scoring function of the implemented minimax algorithm, the computer is easily defeated on boards of dimensions greater than 6x6 so long as then human players play optimally during the early and mid-game stages. This is due to the computer prioritizing moves that yield the greatest number of pieces flipped, which is not an ideal strategy. 
- To improve this, early and mid-game stages should utilize heuristics aside from the current region risk-rating system or increase the precision of the region rating.
**On board state scoring:**
- In this current version, all multipliers used fitness scoring are hard-coded. That being said, 6x6 and 8x8 boards are the optimal dimensions to play against (in regards to playing against a challenging computer player).
- Utility Scoring (Minimax.cpp) : 
	- Scores a board state according to grid positions in possession by MAX (always the computer) and grid positions in possession by MIN (always the player). 
	- In short, the utility of the board is the sum of the grid positions multiplied by their biases. Corners possessed by MAX are worth 10, -10 if possessed by MIN. High risk regions (grids directly adjacent to the corners) are worth -3 for MAX, 3 for min. Edge positions: 2 MAX, -2 MIN. Every other position: 1 MAX, -1 MIN.
- End Game Scoring (Minimax.cpp) :
	- Adds 1 or subtracts 1 from the board's fitness depending on color. Multiplies final score by 10 to assure that incomplete board states' fitness are not prioritized over end game fitnesses.

**Known Issues:**
- Game.isValidMove() is old code, can be replaced with a call to Board.getValidMoves() for move validity checking for human player's runPly() call.
- Minimax's DLS function currently handles skipped turns by merely scoring the board state preceding the skipped turn and returns that score (Minimax.cpp lines 253-256, 291-294)
- In the case that Minimax's IDS function returns a move found at depth 0, the action of the root node of the tree (which by default has its preceding action set to -1, -1) is returned, which is an illegal move. GameV2.cpp handles this beginning at line 375 by getting a list of valid moves from the current board state and choosing the first one. 

**Console Debugging Lines (uncomment these and step through with gdb to verify correctness of algorithm):**

Minimax.cpp:
- 227     <--- Display all of a node's information
- 277,314 <--- Display each node's return value
- 496-499 <--- Display bottom level node's fitness as scored by the utility function

GameV2.cpp:
- 364-368 <--- Display the move, fitness, and depth discovered of minimax's return value


