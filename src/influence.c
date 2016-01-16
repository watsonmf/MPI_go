/*	CSCI 5576: Final Project: Parallel Go: influence.c
*	
*	Authors: Andrea Miller, Michael Watson
*	
*	Builds an influence map of the current board state
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "go_definitions.h"
#include "rules.h"
#include "influence.h"

void build_influence_map(char* board, int* influenceMap)
{
	//int* influenceMap = calloc(sizeof(int), BOARD_SIZE * BOARD_SIZE);
	
	for (int i = 0; i < BOARD_SIZE * BOARD_SIZE; i++)
	{
		influenceMap[i] = 0;
	}
	
	int influence;
	int coordinates;
	
	for (int i = 0; i < BOARD_SIZE; i++)
	{
		for (int j = 0; j < BOARD_SIZE; j++)
		{
			coordinates = i * BOARD_SIZE + j;
			
			switch (board[coordinates])
			{
			case EMPTY:
				continue;
			case BLACK:
				influence = INFLUENCE;
				break;
			case WHITE:
				influence = -INFLUENCE;
				break;
			case -BLACK:
				influenceMap[coordinates] = TERRITORY;
				continue;
			case -WHITE:
				influenceMap[coordinates] = -TERRITORY;
				continue;
			default:
				continue;
			}
			
			/* Project influence to all empty neighbors */
			
			if (coordinates > BOARD_SIZE) // not bottom
			{
				if (board[coordinates - BOARD_SIZE] == EMPTY)
				{
					project_influence(influenceMap, board, coordinates - BOARD_SIZE, influence, 0);
				} 
			}
			if (coordinates % BOARD_SIZE != 0) // not left edge
			{
				if (board[coordinates - 1] == EMPTY)
				{
					project_influence(influenceMap, board, coordinates - 1, influence, 0);
				} 
			}
			if (coordinates < BOARD_SIZE * (BOARD_SIZE - 1)) // not top edge
			{
				if (board[coordinates + BOARD_SIZE] == EMPTY)
				{
					project_influence(influenceMap, board, coordinates + BOARD_SIZE, influence, 0);
				}
			}
			if ( (coordinates % BOARD_SIZE) != (BOARD_SIZE - 1) ) // not right edge
			{
				if (board[coordinates + 1] == EMPTY)
				{
					project_influence(influenceMap, board, coordinates + 1, influence, 0);
				} 
			}
		}
	}
	
	//return influenceMap;
}

void project_influence (int* influenceMap, char* board, int coordinates, int influence, int distance)
{
	++distance;
	influenceMap[coordinates] += influence;
	
	if (distance == MAX_INFLUENCE_DISTANCE)
	{
		return;
	} else
	{
		influence /= INFLUENCE_DIVISOR;
		
		/* Project influence to all empty neighbors */
		
		if ( coordinates > BOARD_SIZE ) // not bottom
		{
			if ( board[coordinates - BOARD_SIZE] == EMPTY )
			{
				project_influence(influenceMap, board, coordinates - BOARD_SIZE, influence, distance);
			} 
		}
		if ( coordinates % BOARD_SIZE != 0 ) // not left edge
		{
			if ( board[coordinates - 1] == EMPTY )
			{
				project_influence(influenceMap, board, coordinates - 1, influence, distance);
			} 
		}
		if ( coordinates < BOARD_SIZE * (BOARD_SIZE - 1) ) // not top edge
		{
			if ( board[coordinates + BOARD_SIZE] == EMPTY )
			{
				project_influence(influenceMap, board, coordinates + BOARD_SIZE, influence, distance);
			} 
		}
		if ( (coordinates % BOARD_SIZE) != (BOARD_SIZE - 1) ) // not right edge
		{
			if ( board[coordinates + 1] == EMPTY )
			{
				project_influence(influenceMap, board, coordinates + 1, influence, distance);
			} 
		}
	}
}

void print_influence_map(char* goBoard)
{
	int influenceMap[BOARD_SIZE * BOARD_SIZE];
	
	build_influence_map(goBoard, influenceMap);
	
	printf ("\nInfluence Map: \n");
	
	for (int i = BOARD_SIZE - 1; i >= 0 ; i--)
	{
		for (int j = 0; j < BOARD_SIZE; j++)
		{
			printf("%5d", influenceMap[i * BOARD_SIZE + j]);
		}
		printf("\n");
	}
	
	printf("Total Influence: %d\n\n", get_total_influence(influenceMap) );
}

long calculate_influence(char* board)
{
	long total = 0;
	int influence;
	int influenceMap [BOARD_SIZE * BOARD_SIZE];
	
	for (int i = 0; i < BOARD_SIZE * BOARD_SIZE; i++)
	{
		influenceMap[i] = 0;
	}
	
	for (int i = 0; i < BOARD_SIZE * BOARD_SIZE; i++)
	{
		switch (board[i])
		{
		case EMPTY:
			continue;
		case BLACK:
			influence = INFLUENCE;
			break;
		case WHITE:
			influence = -INFLUENCE;
			break;
		case -BLACK:
			influenceMap[i] = TERRITORY;
			continue;
		case -WHITE:
			influenceMap[i] = -TERRITORY;
			continue;
		default:
			continue;
		}
		
		/* Project influence to all empty neighbors */
		
		if (i > BOARD_SIZE) // not bottom
		{
			if (board[i - BOARD_SIZE] == EMPTY)
			{
				project_influence(influenceMap, board, i - BOARD_SIZE, influence, 0);
			} 
		}
		if (i % BOARD_SIZE != 0) // not left edge
		{
			if (board[i - 1] == EMPTY)
			{
				project_influence(influenceMap, board, i - 1, influence, 0);
			} 
		}
		if (i < BOARD_SIZE * (BOARD_SIZE - 1)) // not top edge
		{
			if (board[i + BOARD_SIZE] == EMPTY)
			{
				project_influence(influenceMap, board, i + BOARD_SIZE, influence, 0);
			}
		}
		if ( (i % BOARD_SIZE) != (BOARD_SIZE - 1) ) // not right edge
		{
			if (board[i + 1] == EMPTY)
			{
				project_influence(influenceMap, board, i + 1, influence, 0);
			} 
		}
	}

	
	for ( int i = 0; i < BOARD_SIZE * BOARD_SIZE; i++ )
	{
		total += influenceMap[i];
	}
	
	return total;
}

int get_total_influence(int* influenceMap)
{
	int total = 0;
	
	for ( int i = 0; i < BOARD_SIZE * BOARD_SIZE; i++ )
	{
		total += influenceMap[i];
	}
	
	return total;
}






