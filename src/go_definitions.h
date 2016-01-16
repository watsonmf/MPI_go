#ifndef GO_DEFINITIONS_H
#define GO_DEFINITIONS_H

#define EMPTY 0
#define BLACK 1
#define WHITE 3
#define MARKER 8

// Define values for building board influence map
#define TERRITORY 256
#define INFLUENCE 16
#define MAX_INFLUENCE_DISTANCE 3
#define INFLUENCE_DIVISOR 3

#define MAX_RANDOM_MOVES 25
#define MAX_PARALLEL_DEPTH 4
#define MAX_SERIAL_DEPTH 4

#define EARLY_TURNS 8


/*	
*	Macro for printing each game board piece
*	boardChar (defaultString, pointValue), returns defaultString if point is empty or
*	the appropriate unicode character for each color.
*/
#define boardChar(S, N) ((N) == WHITE ? "\u25CF" : ((N) == BLACK ? "\u25CB" : (S) ))

enum { OCCUPIED = 1, JISATSU = 2, ATARI = 3, KO = 4};

int BOARD_SIZE;
int debug;

int np;
int my_rank;

typedef struct _gameboard
{
	char* board; // Game board: 0 = empty, 1 = black piece, 2 = white piece
	//int* influenceMap; //positive influence for black, negative for white
	
	int recentCapturedBlack;
	int recentCapturedBlackTurn;
	int recentCapturedWhite;
	int recentCapturedWhiteTurn;
	
	int blackPrisoners;
	int whitePrisoners;
	int turnNumber;
} GameBoard;

typedef struct _territory
{
	int numberOfTerritories;
	int whiteNeighbors;
	int blackNeighbors;
} Territory;

#endif