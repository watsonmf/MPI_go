#ifndef PARALLEL_MOVE_TREE
#define PARALLEL_MOVE_TREE

int get_move_mpi(GameBoard* goBoard, int color);
int get_early_move_mpi(GameBoard* goBoard, int color);
void dfs_mpi();
long parallel_dfs(char* board, int blackPrisoners, int whitePrisoners, int depth, int color, int turnNumber);
long parallel_dfs_group_focused(char* board, int blackPrisoners, int whitePrisoners, int depth, int color, int turnNumber);
int has_neighbors(int coordinates, char* board);
int has_neighbors_color(int coordinates, char* board, int color);

#endif