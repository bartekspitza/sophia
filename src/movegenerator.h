#ifndef MOVEGENERATOR_H
#define MOVEGENERATOR_H

#include "typedefs.h"
#include "magics.h"

void initMoveGenerationTables(void);
void moveToSan(Move move, char san[]);
void sanToMove(Move* move, char* san);
/*
 * Returns the amount of moves generated
*/
int legalMoves(Board board, Move moves[]);

#endif