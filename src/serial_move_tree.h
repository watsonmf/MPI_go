#ifndef SERIAL_MOVE_TREE
#define SERIAL_MOVE_TREE
/*
typedef struct _node_19{
	int  value;
	int  parent;
	int  depth;
	char board[19*19];
	
	int  child_count;
	int  children[MAX_CHILDREN];
} Node19;

typedef struct _node_13{
	int  value;
	int  parent;
	int  depth;
	char board[13*13];
	
	int  child_count;
	int  children[MAX_CHILDREN];
} Node13;

typedef struct _node_9{
	int  value;
	int  parent;
	int  depth;
	char board[9*9];
	
	int  child_count;
	int  children[MAX_CHILDREN];
} Node9;

typedef Node19* Tree19[MAX_NODES];
typedef Node13* Tree13[];
typedef Node9* Tree9[];*/

int get_good_move(GameBoard* goBoard, int color);
int dfs(char* board, int blackPrisoners, int whitePrisoners, int depth, int color, int turnNumber);

#endif