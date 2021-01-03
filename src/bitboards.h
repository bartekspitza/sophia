#ifndef BITBOARDS_H
#define BITBOARDS_H

#include "typedefs.h"

extern Bitboard SQUARE_BITBOARDS[64];
extern Bitboard RANK_1;
extern Bitboard RANK_7;

void initBitboards(void);

#endif