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
void moveToSan(Move move, char san[]);
void sanToMove(Board board, Move* move, char* san);
void printMoves(Move moves[], int length);
int legalMoves(Board board, Move moves[]);

#endif