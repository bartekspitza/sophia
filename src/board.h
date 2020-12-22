#ifndef BOARD_H
#define BOARD_H

#include "typedefs.h"
#include "fen.h"
#include "movegenerator.h"

void printBoard(Board board);
void pushMove(Board* board, Move move);
void pushSan(Board* board, char* san);
Bitboard* pieceBitboard(Board* board, int pieceType);

#endif