#ifndef BOARD_H
#define BOARD_H

#include "typedefs.h"
#include "fen.h"
#include "movegenerator.h"

#define getBit(bitboard, square) (bitboard & (1ULL << square))
#define popBit(bitboard, square) (bitboard & ~(1ULL << square))
#define setBit(bitboard, square) bitboard | (1ULL << square)
#define toggleBit(bitboard, square) (bitboard ^= (1ULL << square))

void printBoard(Board board);
void pushMove(Board* board, Move move);
void pushSan(Board* board, char* san);

#endif