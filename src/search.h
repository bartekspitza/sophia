#ifndef SEARCH_H
#define SEARCH_H

#include "typedefs.h"

extern int SEARCH_NODES_SEARCHED;
extern Move SEARCH_BEST_MOVE;

int search(Board board, int depth);

#endif