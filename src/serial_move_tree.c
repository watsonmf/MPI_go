/*	CSCI 5576: Final Project: Parallel Go: serial_move_tree.c
*	
*	Authors: Andrea Miller, Michael Watson
*	
*	Depth first recursive search for finding future moves
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "go_definitions.h"
#include "rules.h"
#include "score.h"
#include "influence.h"
#include "serial_move_tree.h"


int get_good_move(GameBoard* goBoard, int color)
{
	char tempBoard[BOARD_SIZE * BOARD_SIZE];
	int possibleMoves[BOARD_SIZE * BOARD_SIZE + 1];
	int bestMove = 0;
	int bestMoveIndex;
	
/*	if (goBoard->turnNumber <= 10)
	{
		return get_good_early_move(goBoard->board, goBoard->blackPrisoners, goBoard->whitePrisoners, )
	}*/
	
	for (int i = 0; i < BOARD_SIZE * BOARD_SIZE; i++)
	{
		if (is_legal(i, goBoard->board, goBoard->turnNumber) == 0)
		{
			memcpy( tempBoard, goBoard->board, BOARD_SIZE * BOARD_SIZE * sizeof(char));
			possibleMoves[i] = dfs(tempBoard, goBoard->blackPrisoners, goBoard->whitePrisoners, 1, color, goBoard->turnNumber );
			//printf("test %d: %d ", i, possibleMoves[i]);
			if (color == BLACK)
			{
				if ( possibleMoves[i] > bestMove )
				{
					bestMove = possibleMoves[i];
					bestMoveIndex = i;
				}
			} else
			{
				if ( possibleMoves[i] < bestMove )
				{
					bestMove = possibleMoves[i];
					bestMoveIndex = i;
				}
			}
		} else
		{
			possibleMoves[i] = 0;
		}
	}
	possibleMoves[ BOARD_SIZE * BOARD_SIZE ] = dfs(goBoard->board, goBoard->blackPrisoners, goBoard->whitePrisoners, 1, color, goBoard->turnNumber );

	if (color == BLACK)
	{
		if ( possibleMoves[BOARD_SIZE * BOARD_SIZE] > bestMove )
		{
			bestMove = possibleMoves[BOARD_SIZE * BOARD_SIZE];
			bestMoveIndex = BOARD_SIZE * BOARD_SIZE;
		}
	} else
	{
		if ( possibleMoves[BOARD_SIZE * BOARD_SIZE] < bestMove )
		{
			bestMove = possibleMoves[BOARD_SIZE * BOARD_SIZE];
			bestMoveIndex = BOARD_SIZE * BOARD_SIZE;
		}
	}

	return bestMoveIndex;
}

int get_good_early_move(char* board, int blackPrisoners, int whitePrisoners, int depth, int color, int turnNumber)
{
	// First move: choose move away from human opponent and at least BOARD_SIZE / 4 distance from edges
	if (turnNumber == 0)
	{
		
	} else if (turnNumber == 1)
	{
		
	}
	
	// Second move: choose move that connects to first move and moves towards one of the two close game board edge
	// Alternative: build search tree that connects existing pieces or + X random moves
	// third and fourth move: similar to Second
	// After that, begin move calculations.
}

int dfs(char* board, int blackPrisoners, int whitePrisoners, int depth, int color, int turnNumber)
{
	char tempBoard[BOARD_SIZE * BOARD_SIZE];
	int tempBlackPrisoners;
	int tempWhitePrisoners;
	int legalMoves = 1;

	//int score = get_board_score(board, blackPrisoners, whitePrisoners);
	int nodeScore = get_black_score(board, turnNumber) + whitePrisoners - get_white_score(board, turnNumber) - blackPrisoners;
	int subTreeScore = 0;
	
/*	if (depth >= 4 && depth < 5)
	{
		printf("Depth: %d - %d\n", depth, nodeScore);
	}*/
	int move;
	
	if (depth <= MAX_SERIAL_DEPTH)
	{
		for (int i = 0; i < BOARD_SIZE * BOARD_SIZE && legalMoves < 21; i++)
		{
			move = rand() % (BOARD_SIZE * BOARD_SIZE + 1);
			if (is_legal(move, board, turnNumber) == 0)
			{
				legalMoves++;
				memcpy( tempBoard, board,  BOARD_SIZE * BOARD_SIZE * sizeof(char) );
				tempBlackPrisoners = blackPrisoners;
				tempWhitePrisoners = whitePrisoners;
				
				tempBoard[move] = color;
				test_capture(tempBoard, &tempBlackPrisoners, &tempWhitePrisoners, move, color );
				
				subTreeScore += dfs(tempBoard, tempBlackPrisoners, tempWhitePrisoners, depth + 1, color ^ 2, turnNumber + 1);
				//printf("DFS: %d-%d: %d", depth, i, subTreeScore);
			}
		}
		// pass move
		//printf("legalMoves = %d\n", legalMoves);
		
		tempBlackPrisoners = blackPrisoners;
		tempWhitePrisoners = whitePrisoners;
		subTreeScore += dfs( board, tempBlackPrisoners, tempWhitePrisoners, depth + 1, color ^ 2, turnNumber + 1 );
	}
	
	return nodeScore + ((subTreeScore * depth * 100 )/ legalMoves);
}












