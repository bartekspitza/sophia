#ifndef FEN_H
#define FEN_H

#include "board.h"
#define START_FEN "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1"

void setFen(Board* board, char* fen);

#endif