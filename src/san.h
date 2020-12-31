#ifndef SAN_H
#define SAN_H

#include "typedefs.h"

void moveToSan(Move move, char san[]);
void sanToMove(Board board, Move* move, char* san);
void pushSan(Board* board, char* san);

#endif