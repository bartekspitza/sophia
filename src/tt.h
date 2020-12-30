#ifndef TT_H
#define TT_H

#include "typedefs.h"

typedef struct {
    Bitboard zobrist;
    int eval;
    int nodeType;
    int depth;
    Move move;
} TTEntry;

enum NODE_TYPE {EXACT, LOWER, UPPER};

extern TTEntry TT_TABLE[];

TTEntry get(Board board, Bitboard* zobrist);
void addTTEntry(Board board, int eval, Move move, int depth, int beta, int alpha);

#endif