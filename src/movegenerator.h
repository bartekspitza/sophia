#ifndef MOVEGENERATOR_H
#define MOVEGENERATOR_H

#include "typedefs.h"
#include "magics.h"

extern Bitboard PAWN_START_WHITE;
extern Bitboard PAWN_START_BLACK;

void initMoveGenerationTables(void);
void moveToSan(Move move, char san[]);
void sanToMove(Move* move, char* san);
/*
 * Returns the amount of moves generated
*/
int legalMoves(Board board, Move moves[]);

#endif