#include "bitboards.h"

Bitboard SQUARE_BITBOARDS[64];

void initBitboards(void) {
    for (int i = 0; i < 64; i++) {
        Bitboard bb = 1ULL << i;
        SQUARE_BITBOARDS[i] = bb;
    }
}