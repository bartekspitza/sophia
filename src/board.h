#ifndef BOARD_H
#define BOARD_H

#include "typedefs.h"

#define getBit(bitboard, square) (bitboard & (1ULL << square))
#define popBit(bitboard, square) (bitboard & ~(1ULL << square))
#define setBit(bitboard, square) bitboard | (1ULL << square)
#define toggleBit(bitboard, square) (bitboard ^= (1ULL << square))

int result(Board board, int numMoves);
void computeOccupancyMasks(Board* board);
void printBoard(Board board);
void pushMove(Board* board, Move move);

#endif