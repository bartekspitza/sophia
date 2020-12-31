#ifndef MOVEGEN_H
#define MOVEGEN_H

#include <stdbool.h>
#include "typedefs.h"
#include "magics.h"

extern Bitboard PAWN_START_WHITE;
extern Bitboard PAWN_START_BLACK;

int bitScanForward(Bitboard* bb);
void initMoveGeneration(void);
bool isSquareAttacked(Board board, int square);
int legalMoves(Board board, Move moves[]);

#endif