#ifndef UTILS_H
#define UTILS_H

#include "typedefs.h"

void printBitboard(Bitboard bb);
void printBits(Bitboard bb);
int getBit(Bitboard bb, int bit);
void clearBit(Bitboard* bb, int bit);
void setBit(Bitboard* bb, int bit);

#endif