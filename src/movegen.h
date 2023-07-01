#ifndef MOVEGEN_H
#define MOVEGEN_H

#include <stdbool.h>
#include "typedefs.h"
#include "magics.h"

extern Bitboard PAWN_START_WHITE;
extern Bitboard PAWN_START_BLACK;
extern Bitboard PAWN_W_ATTACKS_EAST[64];
extern Bitboard PAWN_W_ATTACKS_WEST[64];
extern Bitboard PAWN_B_ATTACKS_EAST[64];
extern Bitboard PAWN_B_ATTACKS_WEST[64];
extern Bitboard KNIGHT_MOVEMENT[64];
extern Bitboard BISHOP_MOVEMENT[64];
extern Bitboard ROOK_MOVEMENT[64];
extern Bitboard KING_MOVEMENT[64];

void initMoveGeneration(void);
int legalMoves(Board* board, Move moves[]);
int bitScanForward(Bitboard* bb);
bool isSquareAttacked(Board board, int square);
Bitboard getRookAttacks(int square, Bitboard occupancy);
Bitboard getBishopAttacks(int square, Bitboard occupancy);

#endif