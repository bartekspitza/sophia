#ifndef SEARCH_H
#define SEARCH_H

#include "typedefs.h"

extern int NODES_SEARCHED;
extern Move BEST_MOVE;

int search(Board board, int depth, Move* move);

#endif