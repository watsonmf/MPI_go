
int is_legal(int move, char* board, int turnNumber);
int is_stupid(char* board, int move, int turnNumber);
int is_jisatsu(int move, char* board, int color);
int is_atari(int move, char* board, int color);
int is_ko(int move, char* board, int color);
int get_group_liberties(int coordinates, char* board, int color);
//void get_score(char* board);
//void calculate_territory(char* board, int coordinates, Territory* territory)
void unmark_board(char* board);
int capture_group(int coordinates, char* board, int color);
void test_capture (char* board, int* blackPrisoners, int* whitePrisoners, int coordinates, int color);