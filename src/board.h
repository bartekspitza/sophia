#ifndef BOARD_H
#define BOARD_H

#include "typedefs.h"
#include "bitboards.h"

#define getBit(bitboard, square) (bitboard & SQUARE_BITBOARDS[square])
#define popBit(bitboard, square) (bitboard & ~SQUARE_BITBOARDS[square])
#define setBit(bitboard, square) bitboard | SQUARE_BITBOARDS[square]
#define toggleBit(bitboard, square) (bitboard ^ SQUARE_BITBOARDS[square])

int result(Board board, Move pseudoLegal[], int length);
void computeOccupancyMasks(Board* board);
void printBoard(Board board);
void pushMove(Board* board, Move move);

#endif