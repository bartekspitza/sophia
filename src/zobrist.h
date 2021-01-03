#ifndef ZOBRIST_H
#define ZOBRIST_H

#include "typedefs.h"

extern Bitboard PIECES[12][64];
extern Bitboard EN_PASSANT[64];
extern Bitboard CASTLING[16];
extern Bitboard WHITE_TO_MOVE;

void initZobrist(void);
Bitboard hash(Board board);

#endif