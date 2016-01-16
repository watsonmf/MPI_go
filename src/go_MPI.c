/*	CSCI 5576: Final Project: Parallel Go: go_MPI.c
*	
*	Authors: Andrea Miller, Michael Watson
*	
*	Parallel implementation of our go program. Uses the parallel search tree functionality from
*	parallel_move_tree.c to split the current move into subtrees, distribute those trees through 
*	all the processors, and recursively search through each subtree using OpenMP for multitreading.
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <mpi.h>
#include <time.h>
#include "popt.h"
#include "go_definitions.h"
#include "rules.h"
#include "score.h"
#include "influence.h"
#include "board_tools.h"
#include "parallel_move_tree.h"

void play_the_game (GameBoard* goBoard);
void player_move (GameBoard* goBoard);
void player2_move (GameBoard* goBoard);
void computer_move (GameBoard* goBoard);
void smart_computer_move(GameBoard* goBoard);

int player;
int player2;
int computer;

int main (int argc, char** argv)
{
	MPI_Init(&argc, &argv);
	MPI_Comm_size(MPI_COMM_WORLD, &np);
	MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);
	
	int boardSize = 19;
	char* playerSide;
	time_t t;
	GameBoard* goBoard = malloc(sizeof(GameBoard));
	goBoard->blackPrisoners = 0;
	goBoard->whitePrisoners = 0;
	goBoard->turnNumber = 0;
	
	player = BLACK;
	player2 = WHITE;
	computer = WHITE;
	
	
	/*	-------Begin POPT parsing-------	*/

	poptContext	POPT_Context;  /* context for parsing command-line options */
	int			POPT_Ret;      /* used for iterating over the arguments */

	struct poptOption optionsTable[] =
	{
		{ "size", 's', POPT_ARG_INT, &boardSize, 's', "Specify size of game board NxN (default is 19)", "N" },
		{ "player", 'p', POPT_ARG_STRING, &playerSide, 'p', "Assign player to specified side (default is black)", 0 },
		{ "debug", 'd', POPT_ARG_NONE, 0, 'd', "Enable debug output", 0 },
		
		POPT_AUTOHELP
		{ NULL, '\0', 0, NULL, 0}
	};

	POPT_Context = poptGetContext(NULL, argc,  (const char **)argv, optionsTable, 0);
	poptSetOtherOptionHelp(POPT_Context, "[Options]\n\n[Try --help for a more detailed description of the options]\n");

	/* values are filled into the data structures by this function */
	while ((POPT_Ret = poptGetNextOpt(POPT_Context)) >= 0)
	{
		switch (POPT_Ret)
		{
		case 's':
			/* handle board size argument */
			if (boardSize == 9 || boardSize == 13 || boardSize == 19)
			{
				if (my_rank ==0)
				{	
					printf("...Setting game board size to %d x %d\n", boardSize, boardSize);
				}
			} else
			{	
				if (my_rank ==0)
				{
					printf("Error: board size must be either 9, 13, or 19.\n Using default of 19.\n");
				}
			}
			break;
		case 'd':
			/* handle debug argument */
			if (my_rank ==0)
			{
				printf("...Debug messages are enabled\n");
			}
			debug = 1;
			break;
		case 'p':
			/* handle computer argument */
			if (strcmp (playerSide, "black") == 0)
			{
				if (my_rank ==0)
				{
					puts("...Setting player to black");
				}
				player = BLACK;
				computer = WHITE;
			} else if (strcmp (playerSide, "white") == 0)
			{
				if (my_rank ==0)
				{
					puts("...Setting player to white");
				}
				player = WHITE;
				computer = BLACK;
			} else
			{
				if (my_rank ==0)
				{
					puts("Unknown entry. Player set to black.");
				}
			}
		}
	}
	
	fflush(stdout);

	if (POPT_Ret < -1)
	{
		/* an error occurred during option processing */
		fprintf(stderr, "%s: %s\n",
		        poptBadOption(POPT_Context, POPT_BADOPTION_NOALIAS),
		        poptStrerror(POPT_Ret));
		return 1;
	}
	/*	-------End POPT parsing-------	*/
	
	BOARD_SIZE = boardSize;
	srand((unsigned) time(&t));
	
	if (my_rank == 0)
	{
		goBoard->board = calloc(boardSize * boardSize, sizeof(char));
		goBoard->turnNumber = 0;
		
		play_the_game(goBoard);
	} else
	{
		dfs_mpi();
	}
	
	MPI_Finalize();
	return 0;
}


/* Loop through a alternating black and white turns */
void play_the_game(GameBoard* goBoard)
{
	int blackScore;
	int whiteScore;
	
	while(1)
	{
		if (player == BLACK)
		{
			player_move(goBoard);
			blackScore = get_black_score(goBoard->board, goBoard->turnNumber);
			//player2_move(goBoard);
			//computer_move(goBoard);
			smart_computer_move(goBoard);
			whiteScore = get_white_score(goBoard->board, goBoard->turnNumber);

		} else
		{
			smart_computer_move(goBoard);
			blackScore = get_black_score(goBoard->board, goBoard->turnNumber);
			//computer_move(goBoard);
			player_move(goBoard);
			whiteScore = get_white_score(goBoard->board, goBoard->turnNumber);
		}
		
		//blackScore = get_black_score(goBoard->board);
		//whiteScore = get_white_score(goBoard->board);
		
		printf("Black: %d, White: %d\n", blackScore, whiteScore);
		fflush(stdout);
	}
}

/* Human moves */
void player_move(GameBoard* goBoard)
{
	char inputMove[8];
	char c;
	int moveCoordinates[2];
	int move;
	int test;
	int correct;
	
	if (debug) print_influence_map(goBoard->board);
	if (debug) print_board_values (goBoard->board);
	
	print_board(goBoard);
	fflush(stdout);
	
	do
	{
		printf("Player > ");
		fflush(stdout);
		fgets (inputMove, 8, stdin);
		//sscanf(inputMove, "%c%d", &c, &move[1]);
		
		if (strncmp(inputMove, "PASS", 4) == 0)
		{
			puts("Passing move");
			goBoard->turnNumber++;
			return;
		} else if ((test = sscanf(inputMove, "%c%d", &c, &moveCoordinates[1])) != 2 )
		{
			puts("Incorrect move entry.");
			correct = 0;
		} else if (c > ('A' + BOARD_SIZE) || c < 'A' || moveCoordinates[1] < 1 || moveCoordinates[1] > BOARD_SIZE )
		{
			puts("Move out of bounds.");
			correct = 0;
		} else
		{
			correct = 1;
			moveCoordinates[0] = c - 'A'; 
			moveCoordinates[1] = moveCoordinates[1] - 1;
		}
		move = moveCoordinates[1] * BOARD_SIZE + moveCoordinates[0];
		
	} while (!correct || is_legal(move, goBoard->board, goBoard->turnNumber));
	
	goBoard->board[move] = player;
	goBoard->turnNumber++;
	test_capture(goBoard->board, &goBoard->blackPrisoners, &goBoard->whitePrisoners, move, player);
}

/* Second human player used for rule debugging */
void player2_move(GameBoard* goBoard)
{
	char inputMove[8];
	char c;
	int moveCoordinates[2];
	int move;
	int test;
	int correct;
	
	if (debug) print_influence_map(goBoard->board);
	if (debug) print_board_values (goBoard->board);
	
	print_board(goBoard);
	fflush(stdout);
	
	do
	{
		printf("Player2 > ");
		fflush(stdout);
		fgets (inputMove, 8, stdin);
		//sscanf(inputMove, "%c%d", &c, &move[1]);
		
		if (strncmp(inputMove, "PASS", 4) == 0)
		{
			puts("Passing move");
			goBoard->turnNumber++;
			return;
		} else if ((test = sscanf(inputMove, "%c%d", &c, &moveCoordinates[1])) != 2 )
		{
			puts("Incorrect move entry.");
			correct = 0;
		} else if (c > ('A' + BOARD_SIZE) || c < 'A' || moveCoordinates[1] < 1 || moveCoordinates[1] > BOARD_SIZE )
		{
			puts("Move out of bounds.");
			correct = 0;
		} else
		{
			correct = 1;
			moveCoordinates[0] = c - 'A';
			moveCoordinates[1] = moveCoordinates[1] - 1;
		}
		move = moveCoordinates[1] * BOARD_SIZE + moveCoordinates[0];
		
	} while (!correct || is_legal(move, goBoard->board, goBoard->turnNumber));
	
	goBoard->board[move] = player2;
	goBoard->turnNumber++;
	test_capture(goBoard->board, &goBoard->blackPrisoners, &goBoard->whitePrisoners, move, player);
}

/* Generates a random computer move for use in debugging */
void computer_move(GameBoard* goBoard)
{
	int move;

	do
	{
		move = rand() % (BOARD_SIZE * BOARD_SIZE + 1);

		if (move == BOARD_SIZE * BOARD_SIZE)
		{
			printf("PASS\n");
			return;
		}

	} while (is_legal(move, goBoard->board, goBoard->turnNumber));

	goBoard->board[move] = computer;
	goBoard->turnNumber++;
	test_capture(goBoard->board, &goBoard->blackPrisoners, &goBoard->whitePrisoners, move, computer);
}


/* Uses MPI and OpenMP to generate a depth first search tree of future moves. Selects the best move. */
void smart_computer_move(GameBoard* goBoard)
{
	int move;
	
	if ( goBoard->turnNumber < EARLY_TURNS )
	{
		if ( goBoard->turnNumber < 2) //first or second turn is predefined
		{
			int thirds = BOARD_SIZE / 3;
			int bottomLeft = (thirds - 1) * BOARD_SIZE + thirds - 1;
			int bottomRight = (thirds - 1) * BOARD_SIZE + (2 * thirds);
			int topLeft = (2 * thirds) * BOARD_SIZE + thirds - 1;
			int topRight = (2 * thirds) * BOARD_SIZE + (2 * thirds);
			int firstMoves[16] = { bottomLeft, bottomLeft + 1, bottomLeft + BOARD_SIZE, bottomLeft + BOARD_SIZE + 1,
							bottomRight, bottomRight - 1, bottomRight + BOARD_SIZE, bottomRight + BOARD_SIZE - 1,
							topLeft, topLeft + 1, topLeft - BOARD_SIZE, topLeft - BOARD_SIZE + 1,
							topRight, topRight - 1, topRight - BOARD_SIZE, topRight - BOARD_SIZE - 1};
			
			int influenceMap[BOARD_SIZE * BOARD_SIZE];
			
			build_influence_map(goBoard->board, influenceMap);
			
			move = firstMoves[rand() % 16];
			
			while (influenceMap[move] != EMPTY)
			{
				move = firstMoves[rand() % 16];
			}
		} else
		{
			move = get_early_move_mpi(goBoard, computer);
		}
	} else
	{
		move = get_move_mpi(goBoard, computer);
		if (move == -1)
		{
			if (debug) printf("\n MOVE: PASS\n");
			// pass move
			goBoard->turnNumber++;
			return;
		}
	}
	
	
	if (debug) printf("\n MOVE: %d\n", move);
	goBoard->board[move] = computer;
	goBoard->turnNumber++;
	test_capture(goBoard->board, &goBoard->blackPrisoners, &goBoard->whitePrisoners, move, computer);
}

