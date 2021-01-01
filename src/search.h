#ifndef SEARCH_H
#define SEARCH_H

#include "typedefs.h"

typedef struct {
    int length;
    Move moves[15];  
} PVline;


int search(Board board, int depth, Move* move, int* nodesSearched, PVline* pv);

#endif