#ifndef MOVEGENERATOR_H
#define MOVEGENERATOR_H

#include "typedefs.h"

void moveToSan(Move move, char* san);
void sanToMove(Move* move, char* san);
Move* legalMoves(Board board, int* length);

#endif