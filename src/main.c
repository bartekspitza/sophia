#include "board.h"
#include <stdio.h>
#include <stdlib.h>

int main() {
    initMoveGenerationTables();

    Board board;
    setFen(&board, "3kr3/8/6b1/8/8/8/PP6/K7 b - - 0 1");
    printBoard(board);
    pushSan(&board, "e8e1");
    printBoard(board);

    Move moves[250];
    int length = legalMoves(board, moves);
    printMoves(moves, length);

    return 0;
}