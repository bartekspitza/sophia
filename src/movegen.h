#ifndef MOVEGEN_H
#define MOVEGEN_H

#include <stdbool.h>
#include "typedefs.h"
#include "magics.h"

extern Bitboard PAWN_START_WHITE;
extern Bitboard PAWN_START_BLACK;

void initMoveGeneration(void);
int pseudoLegalMoves(Board board, Move moves[]);
int bitScanForward(Bitboard* bb);
bool isSquareAttacked(Board board, int square);
void validateMove(Board board, Move* move);

#endif