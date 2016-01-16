/*	CSCI 5576: Final Project: Parallel Go: board_tools.c
*	
*	Authors: Andrea Miller, Michael Watson
*	
*	Miscellaneous tools used for general board functions like printing
*/


#include <stdio.h>
#include <string.h>

#include "go_definitions.h"
#include "board_tools.h"


void print_board(GameBoard* goBoard)
{
	int length = 0;
	char printedBoard[BOARD_SIZE * BOARD_SIZE * 10];
	
	//length += sprintf(printedBoard + length, "");
	
	length += sprintf(printedBoard + length, "%2d %s\u2500", BOARD_SIZE, boardChar("\u250C", goBoard->board[(BOARD_SIZE - 1) * BOARD_SIZE]));
	
	for (int i = 1; i < BOARD_SIZE - 1; i++)
	{
		length += sprintf(printedBoard + length, "%s\u2500", boardChar("\u252C", goBoard->board[(BOARD_SIZE - 1) * BOARD_SIZE + i]));
	}
	
	length += sprintf(printedBoard + length, "%s\n", boardChar("\u2510", goBoard->board[BOARD_SIZE * BOARD_SIZE - 1]));
	
	for (int i = BOARD_SIZE - 2 ; i > 0; i--)
	{
		length += sprintf(printedBoard + length, "%2d %s\u2500", i + 1, boardChar("\u251C" ,goBoard->board[i * BOARD_SIZE]));
		for (int j = 1; j < BOARD_SIZE - 1; j++)
		{
			length += sprintf(printedBoard + length, "%s\u2500", boardChar("\u253C" ,goBoard->board[i * BOARD_SIZE + j]));
		}
		length += sprintf(printedBoard + length, "%s\n", boardChar("\u2524" ,goBoard->board[i * BOARD_SIZE + BOARD_SIZE - 1]));
	}
	
	length += sprintf(printedBoard + length, "%2d %s\u2500", 1, boardChar("\u2514" ,goBoard->board[0]));
	
	for (int i = 1; i < BOARD_SIZE - 1; i++)
	{
		length += sprintf(printedBoard + length, "%s\u2500", boardChar("\u2534" ,goBoard->board[i]));
	}
	
	length += sprintf(printedBoard + length, "%s\n", boardChar("\u2518" ,goBoard->board[BOARD_SIZE - 1]));

	length += sprintf(printedBoard + length, "  ");
	
	for (int i = 0; i < BOARD_SIZE; i++)
	{
		length += sprintf(printedBoard + length, "%2c", 'A' + i);
	}
	
	length += sprintf(printedBoard + length, "\n");
	
	length += sprintf(printedBoard + length, "Captured: %d Black %d White\n\n", goBoard->blackPrisoners, goBoard->whitePrisoners);
	length += sprintf(printedBoard + length, "Turn: %d\n", goBoard->turnNumber);
	fputs(printedBoard, stdout);
}

void print_board_values(char* board)
{
	for (int i = BOARD_SIZE - 1; i >= 0; i--)
	{
		for(int j = 0; j < BOARD_SIZE; j++)
		{
			printf("%d ", board[i * BOARD_SIZE + j]);
		}
		printf("\n");
	}
}