/*	CSCI 5576: Final Project: Parallel Go: parallel_move_tree.c
*	
*	Authors: Andrea Miller, Michael Watson
*	
*	Uses the parallel search tree to split the current move into subtrees, distribute those trees through 
*	all the processors, and recursively search through each subtree using OpenMP for multitreading.
*/


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <mpi.h>
#include <omp.h>

#include "go_definitions.h"
#include "rules.h"
#include "score.h"
#include "influence.h"
#include "board_tools.h"
#include "parallel_move_tree.h"

// Called on process 0, calculates all the legal moves in the current turn and divides them up to all the processors
// Waits until all processors have finished their calculation and then returns the optimum move.
int get_move_mpi(GameBoard* goBoard, int color)
{
	int legalMoves = 0;
	char tempBoard[BOARD_SIZE * BOARD_SIZE + 4];
	long possibleMoves[BOARD_SIZE * BOARD_SIZE + 1];
	long bestMove = 0;
	int bestMoveIndex;
	int tempBlackPrisoners;
	int tempWhitePrisoners;
	
	MPI_Request requestArray[BOARD_SIZE * BOARD_SIZE + 1];
	MPI_Status	statusArray[BOARD_SIZE * BOARD_SIZE + 1];
	
	for ( int i = 0; i < BOARD_SIZE * BOARD_SIZE; i++ )
	{
		if ( is_legal(i, goBoard->board, goBoard->turnNumber) == 0 )
		{
			legalMoves++;
			memcpy( tempBoard, goBoard->board, (BOARD_SIZE * BOARD_SIZE) * sizeof(char) );
			
			tempBlackPrisoners = goBoard->blackPrisoners;
			tempWhitePrisoners = goBoard->whitePrisoners;
			
			tempBoard[i] = color;
			test_capture( tempBoard, &tempBlackPrisoners, &tempWhitePrisoners, i, color );
			
			tempBoard[BOARD_SIZE * BOARD_SIZE] = (char) tempBlackPrisoners;
			tempBoard[BOARD_SIZE * BOARD_SIZE + 1] = (char) tempWhitePrisoners;
			tempBoard[BOARD_SIZE * BOARD_SIZE + 2] = (char) goBoard->turnNumber + 2;
			tempBoard[BOARD_SIZE * BOARD_SIZE + 3] = (char) color;
			// last four entries of tempBoard contain blackPrisoners, whitePrisoners, turnNumber, and current color.
			
			MPI_Send(tempBoard, BOARD_SIZE * BOARD_SIZE + 4, MPI_CHAR, legalMoves, 19, MPI_COMM_WORLD); // send board to processor with rank equal to legalMoves
			
			MPI_Irecv( &possibleMoves[i], 1, MPI_LONG, legalMoves, 19, MPI_COMM_WORLD, &requestArray[legalMoves - 1] ); // start non-blocking receive
		} else
		{
			possibleMoves[i] = 0;
		}
	}
	
	legalMoves++;
	memcpy( tempBoard, goBoard->board, (BOARD_SIZE * BOARD_SIZE) * sizeof(char));
	tempBoard[BOARD_SIZE * BOARD_SIZE] = (char) goBoard->blackPrisoners;
	tempBoard[BOARD_SIZE * BOARD_SIZE + 1] = (char) goBoard->whitePrisoners;
	tempBoard[BOARD_SIZE * BOARD_SIZE + 2] = (char) goBoard->turnNumber + 2;
	tempBoard[BOARD_SIZE * BOARD_SIZE + 3] = (char) color;
				
	MPI_Send(tempBoard, BOARD_SIZE * BOARD_SIZE + 4, MPI_CHAR, legalMoves, 19, MPI_COMM_WORLD); // send board to processor with rank equal to legalMoves
	
	MPI_Irecv(&possibleMoves[BOARD_SIZE * BOARD_SIZE], 1, MPI_LONG, legalMoves, 19, MPI_COMM_WORLD, &requestArray[legalMoves - 1]); // start non-blocking receive
	
	// wait until all IRecv is complete
	MPI_Waitall(legalMoves, requestArray, statusArray);
	
	if (debug)
	{
		for (int i = BOARD_SIZE - 1; i >= 0 ; i--)
		{
			printf("\n%d:\n", i);
			for (int j = 0; j < BOARD_SIZE; j++)
			{
				printf("%ld ", possibleMoves[i * BOARD_SIZE + j]);
			}
			printf("\n");
			fflush(stdout);
		}
		printf( "PASS: %ld\n", possibleMoves[BOARD_SIZE * BOARD_SIZE] );
	}
	
	do
	{
		bestMoveIndex = rand() % ( BOARD_SIZE * BOARD_SIZE );
	} while (is_legal(bestMoveIndex, goBoard->board, goBoard->turnNumber) != 0);
	
	bestMove = possibleMoves[bestMoveIndex];	
	
	// check each possible move and select the best outcome
	if (color == BLACK)
	{
		for (int i = 0; i < BOARD_SIZE * BOARD_SIZE + 1; i++)
		{
			if ( possibleMoves[i] > bestMove && possibleMoves[i] != 0)
			{
				bestMove = possibleMoves[i];
				bestMoveIndex = i;
			}
		}
	} else
	{
		for (int i = 0; i < BOARD_SIZE * BOARD_SIZE + 1; i++)
		{
			if ( possibleMoves[i] < bestMove && possibleMoves[i] != 0)
			{
				bestMove = possibleMoves[i];
				bestMoveIndex = i;
			}
		}
	}
	
	//printf("BEST MOVE: %ld BEST MOVE INDEX: %d\n", bestMove, bestMoveIndex);
	//fflush (stdout);
	
	if (bestMoveIndex == BOARD_SIZE * BOARD_SIZE)
	{
		return -1; // passing move
	}
	
	return bestMoveIndex;
}

// Called on process 0, calculates all the legal moves in the current turn and divides them up to all the processors
// Waits until all processors have finished their calculation and then returns the optimum move.
int get_early_move_mpi(GameBoard* goBoard, int color)
{
	int legalMoves = 0;
	char tempBoard[BOARD_SIZE * BOARD_SIZE + 4];
	long possibleMoves[BOARD_SIZE * BOARD_SIZE];
	long bestMove = 0;
	int bestMoveIndex;
	int tempBlackPrisoners;
	int tempWhitePrisoners;
	
	MPI_Request requestArray[BOARD_SIZE * BOARD_SIZE + 1];
	MPI_Status	statusArray[BOARD_SIZE * BOARD_SIZE + 1];
	
	for ( int i = 0; i < BOARD_SIZE * BOARD_SIZE; i++ )
	{
		if (has_neighbors_color(i, goBoard->board, color))
		{
			if ( is_legal(i, goBoard->board, goBoard->turnNumber) == 0 )
			{
				legalMoves++;
				memcpy( tempBoard, goBoard->board, (BOARD_SIZE * BOARD_SIZE) * sizeof(char) );
				
				tempBlackPrisoners = goBoard->blackPrisoners;
				tempWhitePrisoners = goBoard->whitePrisoners;
				
				tempBoard[i] = color;
				test_capture( tempBoard, &tempBlackPrisoners, &tempWhitePrisoners, i, color );
				
				tempBoard[BOARD_SIZE * BOARD_SIZE] = (char) tempBlackPrisoners;
				tempBoard[BOARD_SIZE * BOARD_SIZE + 1] = (char) tempWhitePrisoners;
				tempBoard[BOARD_SIZE * BOARD_SIZE + 2] = (char) goBoard->turnNumber + 2;
				tempBoard[BOARD_SIZE * BOARD_SIZE + 3] = (char) color;
				// last four entries of tempBoard contain blackPrisoners, whitePrisoners, turnNumber, and current color.
				
				MPI_Send(tempBoard, BOARD_SIZE * BOARD_SIZE + 4, MPI_CHAR, legalMoves, 19, MPI_COMM_WORLD); // send board to processor with rank equal to legalMoves
				
				MPI_Irecv( &possibleMoves[i], 1, MPI_LONG, legalMoves, 19, MPI_COMM_WORLD, &requestArray[legalMoves - 1] ); // start non-blocking receive
			} 
		}	else
		{
			possibleMoves[i] = 0;
		}
	}
	
	// wait until all IRecv is complete
	MPI_Waitall(legalMoves, requestArray, statusArray);
	
	if (debug)
	{
		for (int i = BOARD_SIZE - 1; i >= 0 ; i--)
		{
			printf("\n%d:\n", i);
			for (int j = 0; j < BOARD_SIZE; j++)
			{
				printf("%ld ", possibleMoves[i * BOARD_SIZE + j]);
			}
			printf("\n");
			fflush(stdout);
		}
	}
	
	do
	{
		bestMoveIndex = rand() % ( BOARD_SIZE * BOARD_SIZE );
	} while ( (has_neighbors_color(bestMoveIndex, goBoard->board, color) == 0) && (is_legal(bestMoveIndex, goBoard->board, goBoard->turnNumber) != 0));
	
	bestMove = possibleMoves[bestMoveIndex];	
	
	// check each possible move and select the best outcome
	if (color == BLACK)
	{
		for (int i = 0; i < BOARD_SIZE * BOARD_SIZE; i++)
		{
			if ( possibleMoves[i] > bestMove && possibleMoves[i] != 0)
			{
				bestMove = possibleMoves[i];
				bestMoveIndex = i;
			}
		}
	} else
	{
		for (int i = 0; i < BOARD_SIZE * BOARD_SIZE; i++)
		{
			if ( possibleMoves[i] < bestMove && possibleMoves[i] != 0)
			{
				bestMove = possibleMoves[i];
				bestMoveIndex = i;
			}
		}
	}
	
	//printf("BEST MOVE: %ld BEST MOVE INDEX: %d\n", bestMove, bestMoveIndex);
	//fflush (stdout);
	
	return bestMoveIndex;
}

// Every processor other than 0 will loop in this function until a terminate signal (comm with count 0) is received.
void dfs_mpi()
{
	MPI_Status probeStatus;
	int count;
	int legalMoves;
	long subtreeScore;
	long nodeScore;
	char tempBoard[BOARD_SIZE * BOARD_SIZE];
	int tempWhitePrisoners;
	int tempBlackPrisoners;
	int color;
	int turnNumber;
	
	while (1)
	{
		MPI_Probe(0, 19, MPI_COMM_WORLD, &probeStatus);
		MPI_Get_count(&probeStatus, MPI_CHAR, &count);
		
		if (count == 0) 
		{ 
			return; // exit 
		} else 
		{
			legalMoves = 0;
			//printf("%d: receiving %d\n", my_rank, count);
			//fflush(stdout);
			char board[count];
			MPI_Recv(board, count, MPI_CHAR, 0, 19, MPI_COMM_WORLD, MPI_STATUS_IGNORE ); // receive board state from process 0
			
			turnNumber = (int) board[count - 2];
			color = (int) board[count - 1];
			
			get_black_score(board, turnNumber);
			get_white_score(board, turnNumber);
			nodeScore = calculate_influence(board);
			subtreeScore = 0;
			
			// only focus on expanding groups in the early turns
			if (turnNumber < EARLY_TURNS)
			{
				#pragma omp parallel default(shared) private(tempBoard) reduction(+: subtreeScore, legalMoves)
				#pragma omp for 
				for ( int i = 0; i < BOARD_SIZE * BOARD_SIZE; i++ )
				{
					if ( has_neighbors_color(i, board, color) && (is_legal (i, board, turnNumber) == 0) )
					{
						legalMoves++;
						memcpy( tempBoard, board, BOARD_SIZE * BOARD_SIZE * sizeof(char) );
						tempBlackPrisoners = (int) board[count - 4];
						tempWhitePrisoners = (int) board[count - 3];
						tempBoard[i] = color;
						
						//subtreeScore += parallel_dfs(tempBoard, tempBlackPrisoners, tempWhitePrisoners, 1, color, turnNumber + 2);
						subtreeScore += parallel_dfs_group_focused(tempBoard, tempBlackPrisoners, tempWhitePrisoners, 1, color, turnNumber + 2);
					}
				}																				
			} else
			{
				// iterate through all legal moves using openMP for additional parallelization
				// calculate the score of each subtree using a recursive depth-first search
				#pragma omp parallel default(shared) private(tempBoard) reduction(+: subtreeScore, legalMoves)
				#pragma omp for 
				for ( int i = 0; i < BOARD_SIZE * BOARD_SIZE; i++ )
				{
					if ( is_legal (i, board, turnNumber) == 0 )
					{
						legalMoves++;
						memcpy( tempBoard, board, BOARD_SIZE * BOARD_SIZE * sizeof(char) );
						tempBlackPrisoners = (int) board[count - 4];
						tempWhitePrisoners = (int) board[count - 3];
						tempBoard[i] = color;
						
						//subtreeScore += parallel_dfs(tempBoard, tempBlackPrisoners, tempWhitePrisoners, 1, color, turnNumber + 2);
						subtreeScore += parallel_dfs_group_focused(tempBoard, tempBlackPrisoners, tempWhitePrisoners, 1, color, turnNumber + 2);
					}
				}
				
				// check score for passing move
				legalMoves++;
				memcpy( tempBoard, board, BOARD_SIZE * BOARD_SIZE * sizeof(char) );
				tempBlackPrisoners = board[count - 3];
				tempWhitePrisoners = board[count - 2];
				
				//subtreeScore += parallel_dfs(tempBoard, tempBlackPrisoners, tempWhitePrisoners, 1, color, turnNumber + 2);
				subtreeScore += parallel_dfs_group_focused(tempBoard, tempBlackPrisoners, tempWhitePrisoners, 1, color, turnNumber + 2);
			}
			
			nodeScore = nodeScore + ((subtreeScore)/ legalMoves);
			
			// send results back to process 0
			MPI_Send( &nodeScore, 1, MPI_LONG, 0, 19, MPI_COMM_WORLD );
		}
	}
}

// recursive DFS for calculating score of all subtree nodes
long parallel_dfs(char* board, int blackPrisoners, int whitePrisoners, int depth, int color, int turnNumber)
{
	char tempBoard[BOARD_SIZE * BOARD_SIZE];
	int tempBlackPrisoners;
	int tempWhitePrisoners;
	int legalMoves = 0;
	
	// printf("%d - %d", my_rank, depth);
	// fflush(stdout);

	get_black_score(board, turnNumber);
	get_white_score(board, turnNumber);
	
	long nodeScore = calculate_influence(board);
	long subTreeScore = 0;
	
	int move;
	
	if ( depth <= MAX_PARALLEL_DEPTH )
	{
		for ( int i = 0; i < BOARD_SIZE * BOARD_SIZE && legalMoves < MAX_RANDOM_MOVES; i++ )
		{
			move = rand() % ( BOARD_SIZE * BOARD_SIZE );
			if ( is_legal(move, board, turnNumber) == 0 ) 
			{
				legalMoves++;
				memcpy( tempBoard, board, BOARD_SIZE * BOARD_SIZE * sizeof(char) );
				
				tempBlackPrisoners = blackPrisoners;
				tempWhitePrisoners = whitePrisoners;
				
				tempBoard[move] = color;
				test_capture( tempBoard, &tempBlackPrisoners, &tempWhitePrisoners, move, color );
				
				subTreeScore += parallel_dfs(tempBoard, tempBlackPrisoners, tempWhitePrisoners, depth + 1, color, turnNumber + 2);
			}
		}
		// pass move
		// printf("legalMoves = %d\n", legalMoves);
		
		legalMoves++;
		tempBlackPrisoners = blackPrisoners;
		tempWhitePrisoners = whitePrisoners;
		subTreeScore += parallel_dfs( board, tempBlackPrisoners, tempWhitePrisoners, depth + 1, color, turnNumber);
		
		return nodeScore + ((subTreeScore * 5 * depth )/ legalMoves);
	}
	
	return nodeScore;
}


// recursive DFS for calculating score of all subtree nodes
long parallel_dfs_group_focused(char* board, int blackPrisoners, int whitePrisoners, int depth, int color, int turnNumber)
{
	char tempBoard[BOARD_SIZE * BOARD_SIZE];
	int tempBlackPrisoners;
	int tempWhitePrisoners;
	int legalMoves = 0;
	
	// printf("%d - %d", my_rank, depth);
	// fflush(stdout);

	get_black_score(board, turnNumber);
	get_white_score(board, turnNumber);
	
	long nodeScore = calculate_influence(board);
	long subTreeScore = 0;
	
	if ( depth <= MAX_PARALLEL_DEPTH )
	{
		for (int i = 0; i < BOARD_SIZE * BOARD_SIZE; i++)
		{
			if ( is_legal(i, board, turnNumber) == 0 ) 
			{
				if ( has_neighbors(i, board) )
				{
					legalMoves++;
					memcpy( tempBoard, board, BOARD_SIZE * BOARD_SIZE * sizeof(char) );
					
					tempBlackPrisoners = blackPrisoners;
					tempWhitePrisoners = whitePrisoners;
					
					tempBoard[i] = color;
					test_capture( tempBoard, &tempBlackPrisoners, &tempWhitePrisoners, i, color );
					
					subTreeScore += parallel_dfs_group_focused(tempBoard, tempBlackPrisoners, tempWhitePrisoners, depth + 1, color, turnNumber + 2);
				}
			}
		}
		
/*		for ( int i = 0; i < BOARD_SIZE * BOARD_SIZE && legalMoves < MAX_RANDOM_MOVES; i++ )
		{
			move = rand() % ( BOARD_SIZE * BOARD_SIZE );
			if ( is_legal(move, board, turnNumber) == 0 ) 
			{
				legalMoves++;
				memcpy( tempBoard, board, BOARD_SIZE * BOARD_SIZE * sizeof(char) );
				
				tempBlackPrisoners = blackPrisoners;
				tempWhitePrisoners = whitePrisoners;
				
				tempBoard[move] = color;
				test_capture( tempBoard, &tempBlackPrisoners, &tempWhitePrisoners, move, color );
				
				subTreeScore += parallel_dfs(tempBoard, tempBlackPrisoners, tempWhitePrisoners, depth + 1, color ^ 2, turnNumber + 1);
			}
		}*/
		
		// pass move
		// printf("legalMoves = %d\n", legalMoves);
		
		legalMoves++;
		tempBlackPrisoners = blackPrisoners;
		tempWhitePrisoners = whitePrisoners;
		subTreeScore += parallel_dfs_group_focused( board, tempBlackPrisoners, tempWhitePrisoners, depth + 1, color, turnNumber + 2 );
		
		return nodeScore + ((subTreeScore * 5 * depth ) / legalMoves);
	}
	
	return nodeScore;
}


int has_neighbors(int coordinates, char* board)
{
	if (coordinates >= BOARD_SIZE) // not bottom
	{
		if (board[coordinates - BOARD_SIZE] != EMPTY)
		{
			return 1;
		}
	}
	if ((coordinates % BOARD_SIZE) != 0) // not left edge
	{
		if (board[coordinates - 1] != EMPTY)
		{
			return 1;
		}
	}
	if (coordinates < (BOARD_SIZE * (BOARD_SIZE - 1))) // not top edge
	{
		if (board[coordinates + BOARD_SIZE] != EMPTY)
		{
			return 1;
		}
	}
	if ((coordinates % BOARD_SIZE) != (BOARD_SIZE - 1)) // not right edge
	{
		if (board[coordinates + 1] != EMPTY)
		{
			return 1;
		}
	}
	
	return 0;
}

int has_neighbors_color(int coordinates, char* board, int color)
{
	if (coordinates >= BOARD_SIZE) // not bottom
	{
		if (board[coordinates - BOARD_SIZE] == color)
		{
			return 1;
		}
	}
	if ((coordinates % BOARD_SIZE) != 0) // not left edge
	{
		if (board[coordinates - 1] == color)
		{
			return 1;
		}
	}
	if (coordinates < (BOARD_SIZE * (BOARD_SIZE - 1))) // not top edge
	{
		if (board[coordinates + BOARD_SIZE] == color)
		{
			return 1;
		}
	}
	if ((coordinates % BOARD_SIZE) != (BOARD_SIZE - 1)) // not right edge
	{
		if (board[coordinates + 1] == color)
		{
			return 1;
		}
	}
	
	return 0;
}




