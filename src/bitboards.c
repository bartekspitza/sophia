#include "bitboards.h"

Bitboard SQUARE_BITBOARDS[64];
Bitboard RANK_1 = 0xFF00;
Bitboard RANK_7 = 0x00FF000000000000;

void initBitboards(void) {
    for (int i = 0; i < 64; i++) {
        Bitboard bb = 1ULL << i;
        SQUARE_BITBOARDS[i] = bb;
    }
}