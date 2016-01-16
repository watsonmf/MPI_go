/*	CSCI 5576: Final Project: Parallel Go: rules.c
*	
*	Authors: Andrea Miller, Michael Watson
*	
*	Algorithms for checking the legality of a specific move and handling capture events
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "go_definitions.h"
#include "score.h"
#include "rules.h"
#include "influence.h"


/* Tests whether a move is legal under the Japanese rules of Go */
int is_legal(int move, char* board, int turnNumber)
{
	int color = (turnNumber % 2) + 1;
	
	if (color == 2)
	{
		color = WHITE;
	}
	
	if (board[move] != 0)
	{
		return OCCUPIED;
	}
	if (is_jisatsu(move, board, color))
	{
		return JISATSU;
	}
	/*if (is_ko(move, goBoard, color))
	{
		return KO;
	}*/
	
	return 0;
}

/* Ensures that the computer will not move into opposing territory */
int is_stupid(char* board, int move, int turnNumber)
{
	int color = (turnNumber % 2) + 1;
	
	if (color == 2)
	{
		color = WHITE;
	}
	
	int otherColor = color ^ 2;
	
	if ( check_territory(board, move, color, otherColor) )
	{
		return 1;
	}
	
	return 0;
}

/* Tests whether a move is suicide */
int is_jisatsu(int coordinates, char* board, int color)
{
	int liberties = get_group_liberties(coordinates, board, color);
	
	unmark_board(board);
	
	if ( liberties > 0 )
	{
		return 0;
	}
	
	if (coordinates >= BOARD_SIZE) // not bottom
	{
		if (board[coordinates - BOARD_SIZE] == EMPTY)
		{
			liberties++;
		} else if ( board[coordinates - BOARD_SIZE] == color )
		{
			liberties += get_group_liberties( coordinates - BOARD_SIZE, board, color );
		}
	}
	if (coordinates % BOARD_SIZE != 0) // not left edge
	{
		if (board[coordinates - 1] == EMPTY)
		{
			liberties++;
		} else if ( board[coordinates - 1] == color )
		{
			liberties += get_group_liberties( coordinates - 1, board, color );
		}
	}
	if (coordinates < BOARD_SIZE * (BOARD_SIZE - 1)) // not top edge
	{
		if (board[coordinates + BOARD_SIZE] == EMPTY)
		{
			liberties++;
		} else if ( board[coordinates + BOARD_SIZE] == color )
		{
			liberties += get_group_liberties( coordinates + BOARD_SIZE, board, color );
		}
	}
	if (coordinates % BOARD_SIZE != BOARD_SIZE - 1) // not right edge
	{
		if (board[coordinates + 1] == EMPTY)
		{
			liberties++;
		} else if ( board[coordinates + 1] == color )
		{
			liberties += get_group_liberties( coordinates + 1, board, color );
		}
	}
	
	return 1;
}

/* Test a possible move to see if it duplicates an earlier board position */
int is_ko(int move, char* board, int color)
{
	return 0;
}

/* Recursively search through a group and return the number of liberties */
int get_group_liberties(int coordinates, char* board, int color)
{
	board[coordinates] ^= MARKER; // mark piece to avoid infinite recursion
	int liberties = 0;
	
	if (coordinates >= BOARD_SIZE) // not bottom
	{
		if (board[coordinates - BOARD_SIZE] == EMPTY || board[coordinates - BOARD_SIZE] == -color)
		{
			liberties++;
		} else if ( board[coordinates - BOARD_SIZE] == color )
		{
			liberties += get_group_liberties( coordinates - BOARD_SIZE, board, color );
		}
	}
	if (coordinates % BOARD_SIZE != 0) // not left edge
	{
		if (board[coordinates - 1] == EMPTY || board[coordinates - 1] == -color )
		{
			liberties++;
		} else if ( board[coordinates - 1] == color )
		{
			liberties += get_group_liberties( coordinates - 1, board, color );
		}
	}
	if (coordinates < BOARD_SIZE * (BOARD_SIZE - 1)) // not top edge
	{
		if (board[coordinates + BOARD_SIZE] == EMPTY || board[coordinates + BOARD_SIZE] == -color )
		{
			liberties++;
		} else if ( board[coordinates + BOARD_SIZE] == color )
		{
			liberties += get_group_liberties( coordinates + BOARD_SIZE, board, color );
		}
	}
	if (coordinates % BOARD_SIZE != BOARD_SIZE - 1) // not right edge
	{
		if (board[coordinates + 1] == EMPTY || board[coordinates + 1] == -color)
		{
			liberties++;
		} else if ( board[coordinates + 1] == color )
		{
			liberties += get_group_liberties( coordinates + 1, board, color );
		}
	}
	
	return liberties;
}

/* Unmark all marked stones and territories */
void unmark_board(char* board)
{
	for (int i = 0; i < BOARD_SIZE * BOARD_SIZE; i++)
	{
		if (board[i] >= MARKER)
		{
			board[i] ^= MARKER;
		}
	}
}

/* Capture a group with no liberties, return the number of pieces in the group */
int capture_group(int coordinates, char* board, int color)
{
	int captured = 1;
	board[coordinates] = EMPTY;
	
	if (coordinates >= BOARD_SIZE) // not bottom
	{
		if ( board[coordinates - BOARD_SIZE] == color )
		{
			captured += capture_group(coordinates - BOARD_SIZE, board, color);
		} 
	}
	if (coordinates % BOARD_SIZE != 0) // not left edge
	{
		if ( board[coordinates - 1] == color )
		{
			captured += capture_group(coordinates - 1, board, color);
		} 
	}
	if (coordinates < BOARD_SIZE * (BOARD_SIZE - 1)) // not top edge
	{
		if ( board[coordinates + BOARD_SIZE] == color )
		{
			captured += capture_group( coordinates + BOARD_SIZE, board, color );
		} 
	}
	if (coordinates % BOARD_SIZE != BOARD_SIZE - 1) // not right edge
	{
		if ( board[coordinates + 1] == color )
		{
			captured += capture_group( coordinates + 1, board, color );
		} 
	}
	
	return captured;
}

/* Test a move location and see if any adjacent groups can be captured */
void test_capture (char* board, int* blackPrisoners, int* whitePrisoners, int coordinates, int color)
{
	int opposingColor;
	int totalCaptured = 0;
	
	if (color == BLACK)
	{
		opposingColor = WHITE;
	} else
	{
		opposingColor = BLACK;
	}
	
	if (coordinates >= BOARD_SIZE) // not bottom
	{
		if ( board[coordinates - BOARD_SIZE] == opposingColor )
		{
			if (get_group_liberties(coordinates - BOARD_SIZE, board, opposingColor) == 0)
			{
				totalCaptured += capture_group(coordinates - BOARD_SIZE, board, opposingColor ^ MARKER);
			}
		}
	}
	if (coordinates % BOARD_SIZE != 0) // not left edge
	{
		if ( board[coordinates - 1] == opposingColor )
		{
			if (get_group_liberties(coordinates - 1, board, opposingColor) == 0)
			{
				totalCaptured += capture_group(coordinates - 1, board, opposingColor ^ MARKER);
			}
		} 
	}
	if (coordinates < BOARD_SIZE * (BOARD_SIZE - 1)) // not top edge
	{
		if ( board[coordinates + BOARD_SIZE] == opposingColor )
		{
			if ( get_group_liberties( coordinates + BOARD_SIZE, board, opposingColor) == 0 )
			{
				totalCaptured += capture_group( coordinates + BOARD_SIZE, board, opposingColor ^ MARKER );
			}
		}
	}
	if (coordinates % BOARD_SIZE != BOARD_SIZE - 1) // not right edge
	{
		if ( board[coordinates + 1] == opposingColor )
		{
			if (get_group_liberties(coordinates + 1, board, opposingColor) == 0)
			{
				totalCaptured += capture_group(coordinates + 1, board, opposingColor ^ MARKER );
			}
		}
	}
	
	unmark_board(board);
	
	if (color == BLACK)
	{
		*whitePrisoners += totalCaptured;
	} else
	{
		*blackPrisoners += totalCaptured;
	}
}
















