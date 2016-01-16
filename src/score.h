#ifndef SCORE_H
#define SCORE_H


int get_black_score(char* board, int turnNumber);
int get_white_score(char* board, int turnNumber);

int get_board_score(char* board, int* blackScore, int* whiteScore);
int check_group_for_eyes(char* oneColorBoard, int coordinates, int groupNumber );
void mark_territory(char* oneColorBoard, int coordinates, int color );
int check_eyes(char* oneColorBoard, int coordinates, int groupNumber );
void mark_group(char* oneColorBoard, int coordinates, int groupNumber );
void print_boards(char* board);
int check_territory(char* board, int coordinates, int color, int otherColor);
void unmark_score_board(char* oneColorBoard, int maxGroups);

#endif