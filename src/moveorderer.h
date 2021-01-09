#ifndef MOVEORDERER_H
#define MOVEORDERER_H

#include "typedefs.h"
#include "tt.h"

void score_moves(Board board, TTEntry entry, Move moves[], int cmoves);
int select_move(Move moves[], int cmoves);

#endif
