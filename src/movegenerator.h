#ifndef MOVEGENERATOR_H
#define MOVEGENERATOR_H

#include <stdbool.h>
#include "typedefs.h"
#include "magics.h"

extern Bitboard PAWN_START_WHITE;
extern Bitboard PAWN_START_BLACK;

bool isSquareAttacked(Board board, int square);
void initMoveGenerationTables(void);
void moveToSan(Move move, char san[]);
void sanToMove(Board board, Move* move, char* san);
void printMoves(Move moves[], int length);
/*
 * Returns the amount of moves generated
*/
int legalMoves(Board board, Move moves[]);

#endif