/*	CSCI 5576: Final Project: Parallel Go: go_Serial.c
*	
*	Authors: Andrea Miller, Michael Watson
*	
*	Serial implementation of go. Uses a recursive depth first seach to look through future moves.
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "popt.h"
#include "go_definitions.h"
#include "rules.h"
#include "influence.h"
#include "score.h"
#include "board_tools.h"
#include "serial_move_tree.h"

void play_the_game (GameBoard* goBoard);
void player_move (GameBoard* goBoard);
//void player2_move(GameBoard* goBoard);

void computer_move (GameBoard* goBoard);
void smart_computer_move(GameBoard* goBoard);


int player;
//int player2;
int computer;

int main (int argc, char** argv)
{
	time_t t;
	int boardSize = 19;
	char* playerSide;
	GameBoard* goBoard = malloc(sizeof(GameBoard));
	goBoard->blackPrisoners = 0;
	goBoard->whitePrisoners = 0;
	goBoard->turnNumber = 0;
	
	player = BLACK;
	//player2 = WHITE;
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
			/*if (boardSize == 9 || boardSize == 13 || boardSize == 19)
			{
				printf("...Setting game board size to %d x %d\n", boardSize, boardSize);
			} else
			{
				printf("Error: board size must be either 9, 13, or 19.\n Using default of 19.\n");
			}*/
			break;
		case 'd':
			/* handle debug argument */
			printf("...Debug messages are enabled\n");
			debug = 1;
			break;
		case 'p':
			/* handle computer argument */
			if (strcmp (playerSide, "black") == 0)
			{
				puts("...Setting player to black");
				player = BLACK;
				computer = WHITE;
			} else if (strcmp (playerSide, "white") == 0)
			{
				puts("...Setting player to white");
				player = WHITE;
				computer = BLACK;
			} else
			{
				puts("Unknown entry. Player set to black.");
			}
		}
	}

	if (POPT_Ret < -1)
	{
		/* an error occurred during option processing */
		fprintf(stderr, "%s: %s\n",
		        poptBadOption(POPT_Context, POPT_BADOPTION_NOALIAS),
		        poptStrerror(POPT_Ret));
		return 1;
	}
	/*	-------End POPT parsing-------	*/
	
	goBoard->board = calloc(boardSize * boardSize, sizeof(char));
	goBoard->turnNumber = 0;
	BOARD_SIZE = boardSize;
	
	srand((unsigned) time(&t));
	
	play_the_game(goBoard);
	
	return 0;
}

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
	}
}

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
	
	do
	{
		printf("Player > ");
		fgets (inputMove, 8, stdin);
		//sscanf(inputMove, "%c%d", &c, &move[1]);
		
		if (strncmp(inputMove, "PASS", 4) == 0)
		{
			puts("Passing move");
			break;
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

void smart_computer_move(GameBoard* goBoard)
{
	int move = get_good_move(goBoard, computer);
	goBoard->board[move] = computer;
	goBoard->turnNumber++;
	test_capture(goBoard->board, &goBoard->blackPrisoners, &goBoard->whitePrisoners, move, computer);
}





