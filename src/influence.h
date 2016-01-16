#ifndef INFLUENCE_H
#define INFLUENCE_H

void build_influence_map(char* board, int* influenceMap);
void project_influence (int* influenceMap, char* board, int coordinates, int influence, int distance);
void print_influence_map(char* goBoard);
long calculate_influence(char* board);
int get_total_influence(int* influenceMap);

#endif