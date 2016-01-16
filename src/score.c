/*	CSCI 5576: Final Project: Parallel Go: score.c
*	
*	Authors: Andrea Miller, Michael Watson
*	
*	Algorithms for calculating total board score, black score, white score, and searching stone groups.
*/

#define GROUP_MARKER 64

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "popt.h"
#include "go_definitions.h"
#include "rules.h"
#include "influence.h"
#include "score.h"
#include "board_tools.h"


int get_black_score(char* board, int turnNumber)
{
	if (turnNumber < 2) 
	{
		//printf("nothing\n");
		return 0;
	}
	
	int blackScore = 0;
	
	for (int i = 0; i < BOARD_SIZE * BOARD_SIZE; i++)
	{
		if (board[i] == EMPTY)
		{
			if ( check_territory(board, i, BLACK, WHITE) )
			{
				mark_territory(board, i, BLACK);
			}
		}
	}
	
	for (int i = 0; i < BOARD_SIZE * BOARD_SIZE; i++)
	{
		if (board[i] == GROUP_MARKER || board[i] == GROUP_MARKER + 1)
		{
			board[i] = EMPTY;
		} else if (board[i] == -BLACK)
		{
			blackScore++;
			//board[i] = EMPTY;
		}
	}

	return blackScore;
}

int get_white_score(char* board, int turnNumber)
{
	if (turnNumber < 2) 
	{
		return 0;
	}
	int whiteScore = 0;
	
	for (int i = 0; i < BOARD_SIZE * BOARD_SIZE; i++)
	{
		if (board[i] == EMPTY)
		{
			if (check_territory(board, i, WHITE, BLACK))
			{
				mark_territory(board, i, WHITE);
			} 
		}
	}

	for (int i = 0; i < BOARD_SIZE * BOARD_SIZE; i++)
	{
		if (board[i] == GROUP_MARKER || board[i] == GROUP_MARKER + 1)
		{
			board[i] = EMPTY;
		} else if (board[i] == -WHITE)
		{
			whiteScore++;
			//board[i] = EMPTY;
		}
	}
	
	return whiteScore;
}

int get_board_score(char* board, int* blackScore, int* whiteScore)
{
	*blackScore = 0;
	*whiteScore = 0;
	
	for (int i = 0; i < BOARD_SIZE * BOARD_SIZE; i++)
	{
		if (board[i] == EMPTY)
		{
			if (check_territory(board, i, BLACK, WHITE))
			{
				mark_territory(board, i, BLACK);
			} 
		}
	}
	
	for (int i = 0; i < BOARD_SIZE * BOARD_SIZE; i++)
	{
		if (board[i] == GROUP_MARKER)
		{
			board[i] = EMPTY;
		}
	}
	
	for (int i = 0; i < BOARD_SIZE * BOARD_SIZE; i++)
	{
		if (board[i] == EMPTY)
		{
			if (check_territory(board, i, WHITE, BLACK))
			{
				mark_territory(board, i, WHITE);
			} 
		}
	}
	
	for (int i = 0; i < BOARD_SIZE * BOARD_SIZE; i++)
	{
		if (board[i] == GROUP_MARKER)
		{
			board[i] = EMPTY;
		}
	}
	
	for (int i = 0; i < BOARD_SIZE * BOARD_SIZE; i++)
	{
		if (board[i] == -BLACK)
		{
			*blackScore++;
			board[i] = EMPTY;
		} else if (board[i] == -WHITE)
		{
			*whiteScore++;
			board[i] = EMPTY;
		}
	}
	
	char blackBoard[BOARD_SIZE * BOARD_SIZE];
	char whiteBoard[BOARD_SIZE * BOARD_SIZE];
	char blackTerritoryMap[BOARD_SIZE * BOARD_SIZE];
	char whiteTerritoryMap[BOARD_SIZE * BOARD_SIZE];
	int blackGroupNumber = 1;
	int whiteGroupNumber = 1;

	for (int i = 0; i < BOARD_SIZE * BOARD_SIZE; i++ )
	{
		if (board[i] == BLACK)
		{
			blackBoard[i] = -1;
			whiteBoard[i] = EMPTY;
		} else if (board[i] == WHITE)
		{
			whiteBoard[i] = -1;
			blackBoard[i] = EMPTY;
		} else
		{
			whiteBoard[i] = EMPTY;
			blackBoard[i] = EMPTY;
		}
	}
	
	for (int i = 0; i < BOARD_SIZE * BOARD_SIZE; i++)
	{
		if (blackBoard[i] == -1)
		{
			mark_group(blackBoard, i, blackGroupNumber++);
		}
		if (whiteBoard[i] == -1)
		{
			mark_group(whiteBoard, i, whiteGroupNumber++);
		}
	}
	
	char whiteGroupEyes[whiteGroupNumber];
	char blackGroupEyes[blackGroupNumber];
	
	for (int i = 0; i < BOARD_SIZE * BOARD_SIZE; i++)
	{
		if ( blackBoard[i] > 0 && blackBoard[i] <= blackGroupNumber)
		{
			blackGroupEyes[1] = check_group_for_eyes(blackBoard, i, blackBoard[i]);
			
			unmark_score_board(blackBoard, blackGroupNumber);
			// unmark empty spaces!!
		}
		if ( whiteBoard[i] > 0 && whiteBoard[i] <= whiteGroupNumber)
		{
			whiteGroupEyes[whiteBoard[i]] = check_group_for_eyes(whiteBoard, i, whiteBoard[i]);
			// unmark empty spaces!!
		}
	}
}

int check_group_for_eyes(char* oneColorBoard, int coordinates, int groupNumber )
{
	oneColorBoard[coordinates] += GROUP_MARKER; // mark piece to avoid infinite recursion
	
	int numberOfEyes = 0;
	
	if (coordinates > BOARD_SIZE - 1) // not bottom
	{
		if (oneColorBoard[coordinates - BOARD_SIZE] == EMPTY)
		{
			if (check_eyes(oneColorBoard, coordinates - BOARD_SIZE, groupNumber))
			{
				mark_territory(oneColorBoard, coordinates - BOARD_SIZE, groupNumber);
				numberOfEyes++;
			}
		} else if (oneColorBoard[coordinates - BOARD_SIZE] == groupNumber)
		{
			numberOfEyes += check_group_for_eyes(oneColorBoard, coordinates - BOARD_SIZE, groupNumber);
		}
	}
	if (coordinates % BOARD_SIZE != 0) // not left edge
	{
		if (oneColorBoard[coordinates - 1] == EMPTY)
		{
			if (check_eyes(oneColorBoard, coordinates - 1, groupNumber))
			{
				mark_territory(oneColorBoard, coordinates - 1, groupNumber);
				numberOfEyes++;
			}
		} else if (oneColorBoard[coordinates - 1] == groupNumber)
		{
			numberOfEyes += check_group_for_eyes(oneColorBoard, coordinates - 1, groupNumber);
		}
	}
	if (coordinates < BOARD_SIZE * (BOARD_SIZE - 1)) // not top edge
	{
		if (oneColorBoard[coordinates + BOARD_SIZE] == EMPTY)
		{
			if (check_eyes(oneColorBoard, coordinates + BOARD_SIZE, groupNumber))
			{
				mark_territory(oneColorBoard, coordinates + BOARD_SIZE, groupNumber);
				numberOfEyes++;
			}
		} else if (oneColorBoard[coordinates + BOARD_SIZE] == groupNumber)
		{
			numberOfEyes += check_group_for_eyes(oneColorBoard, coordinates + BOARD_SIZE, groupNumber);
		}
	}
	if ((coordinates % BOARD_SIZE) != BOARD_SIZE - 1) // not right edge
	{
		if ( oneColorBoard[coordinates + 1] == EMPTY )
		{
			if (check_eyes(oneColorBoard, coordinates + 1, groupNumber))
			{
				mark_territory(oneColorBoard, coordinates + 1, groupNumber);
				numberOfEyes++;
			}
		} else if (oneColorBoard[coordinates + 1] == groupNumber)
		{
			numberOfEyes += check_group_for_eyes(oneColorBoard, coordinates + 1, groupNumber);
		}
	}
	
	return numberOfEyes;
}

void mark_territory(char* board, int coordinates, int color )
{
	board[coordinates] = -color; // mark piece to avoid infinite recursion
	
	if (coordinates >= BOARD_SIZE) // not bottom
	{
		if (board[coordinates - BOARD_SIZE] == GROUP_MARKER)
		{
			mark_territory(board, coordinates - BOARD_SIZE, color);
		}
	}
	if ((coordinates % BOARD_SIZE) != 0) // not left edge
	{
		if (board[coordinates - 1] == GROUP_MARKER)
		{
			mark_territory(board, coordinates - 1, color);
		}
	}
	if (coordinates < (BOARD_SIZE * (BOARD_SIZE - 1))) // not top edge
	{
		if (board[coordinates + BOARD_SIZE] == GROUP_MARKER)
		{
			mark_territory(board, coordinates + BOARD_SIZE, color);
		}
	}
	if ((coordinates % BOARD_SIZE) != (BOARD_SIZE - 1)) // not right edge
	{
		if (board[coordinates + 1] == GROUP_MARKER)
		{
			mark_territory(board, coordinates + 1, color);
		}
	}
}

int check_territory(char* board, int coordinates, int color, int otherColor)
{
	board[coordinates] = GROUP_MARKER;
	
	if (coordinates >= BOARD_SIZE) // not bottom
	{
		if (board[coordinates - BOARD_SIZE] == EMPTY)
		{
			if (check_territory(board, coordinates - BOARD_SIZE, color, otherColor) == 0 )
			{
				board[coordinates]++;
				return 0;
			}
		} else if (board[coordinates - BOARD_SIZE] == otherColor || board[coordinates - BOARD_SIZE] == GROUP_MARKER + 1)
		{
			board[coordinates]++;
			return 0;
		}
	}
	if ((coordinates % BOARD_SIZE) != 0) // not left edge
	{
		if ( board[coordinates - 1] == EMPTY )
		{
			if ( check_territory(board, coordinates - 1, color, otherColor) == 0 )
			{
				board[coordinates]++;
				return 0;
			}
		} else if ( board[coordinates - 1] == otherColor || board[coordinates - 1] == GROUP_MARKER + 1 )
		{
			board[coordinates]++;
			return 0;
		}
	}
	if (coordinates < (BOARD_SIZE * (BOARD_SIZE - 1))) // not top edge
	{
		if ( board[coordinates + BOARD_SIZE] == EMPTY )
		{
			if ( check_territory( board, coordinates + BOARD_SIZE, color, otherColor ) == 0 )
			{
				board[coordinates]++;
				return 0;
			}
		} else if ( board[coordinates + BOARD_SIZE] == otherColor || board[coordinates + BOARD_SIZE] == GROUP_MARKER + 1 )
		{
			board[coordinates]++;
			return 0;
		}
	}
	if ((coordinates % BOARD_SIZE) != (BOARD_SIZE - 1)) // not right edge
	{
		if (board[coordinates + 1] == EMPTY)
		{
			if (check_territory( board, coordinates + 1, color, otherColor ) == 0 )
			{
				board[coordinates]++;
				return 0;
			}
		} else if ( board[coordinates + 1] == otherColor || board[coordinates + 1] == GROUP_MARKER + 1 )
		{
			board[coordinates]++;
			return 0;
		}
	}
	
	return 1;
}

int check_eyes(char* oneColorBoard, int coordinates, int groupNumber )
{
	oneColorBoard[coordinates] = GROUP_MARKER;
	
	if (coordinates >= BOARD_SIZE) // not bottom
	{
		if (oneColorBoard[coordinates - BOARD_SIZE] == EMPTY)
		{
			if (check_eyes(oneColorBoard, coordinates - BOARD_SIZE, groupNumber) == 0 )
			{
				return 0;
			}
		} else if (oneColorBoard[coordinates - BOARD_SIZE] != groupNumber 
				&& oneColorBoard[coordinates - BOARD_SIZE] != groupNumber ^ GROUP_MARKER)
		{
			return 0;
		}
	}
	if (coordinates % BOARD_SIZE != 0) // not left edge
	{
		if (oneColorBoard[coordinates - 1] == EMPTY)
		{
			if (check_eyes(oneColorBoard, coordinates - 1, groupNumber) == 0 )
			{
				return 0;
			}
		} else if (oneColorBoard[coordinates - 1] != groupNumber 
				&& oneColorBoard[coordinates - 1] != (groupNumber ^ GROUP_MARKER))
		{
			return 0;
		}
	}
	if (coordinates < BOARD_SIZE * (BOARD_SIZE - 1)) // not top edge
	{
		if (oneColorBoard[coordinates + BOARD_SIZE] == EMPTY)
		{
			if (check_eyes(oneColorBoard, coordinates + BOARD_SIZE, groupNumber) == 0 )
			{
				return 0;
			}
		} else if (oneColorBoard[coordinates + BOARD_SIZE] != groupNumber 
				&& oneColorBoard[coordinates + BOARD_SIZE] != (groupNumber ^ GROUP_MARKER))
		{
			return 0;
		}
	}
	if (coordinates % BOARD_SIZE != BOARD_SIZE - 1) // not right edge
	{
		if (oneColorBoard[coordinates + 1] == EMPTY)
		{
			if (check_eyes(oneColorBoard, coordinates + 1, groupNumber) == 0 )
			{
				return 0;
			}
		} else if (oneColorBoard[coordinates + 1] != groupNumber 
				&& oneColorBoard[coordinates + 1] != (groupNumber ^ GROUP_MARKER))
		{
			return 0;
		}
	}
	
	return 1;
}

void mark_group(char* oneColorBoard, int coordinates, int groupNumber )
{
	oneColorBoard[coordinates] = groupNumber; // mark piece to avoid infinite recursion

	if (coordinates > BOARD_SIZE) // not bottom
	{
		if (oneColorBoard[coordinates - BOARD_SIZE] == -1)
		{
			mark_group(oneColorBoard, coordinates - BOARD_SIZE, groupNumber);
		} 
	}
	if (coordinates % BOARD_SIZE != 0) // not left edge
	{
		if (oneColorBoard[coordinates - 1] == -1)
		{
			mark_group(oneColorBoard, coordinates - 1, groupNumber);
		} 
	}
	if (coordinates < BOARD_SIZE * (BOARD_SIZE - 1)) // not top edge
	{
		if (oneColorBoard[coordinates + BOARD_SIZE] == -1)
		{
			mark_group(oneColorBoard, coordinates + BOARD_SIZE, groupNumber);
		}
	}
	if (coordinates % BOARD_SIZE != BOARD_SIZE - 1) // not right edge
	{
		if (oneColorBoard[coordinates + 1] == -1)
		{
			mark_group(oneColorBoard, coordinates + 1, groupNumber);
		} 
	}
	
	return;
}

void unmark_score_board(char* oneColorBoard, int maxGroups)
{
	for (int i = 0; i < BOARD_SIZE * BOARD_SIZE; i++)
	{
		if (oneColorBoard[i] >= maxGroups)
		{
			oneColorBoard[i] ^= GROUP_MARKER;
		}
	}
}

void print_boards(char* board)
{
	printf("score Board: \n");
	for (int i = BOARD_SIZE - 1; i >= 0; i--)
	{
		for (int j = 0; j < BOARD_SIZE; j++)
		{
			printf("%d ", board[i * BOARD_SIZE + j]);
		}
		printf("\n");
	}
}