#include "board.h"
#include <stdio.h>
#include <stdlib.h>

int main() {
    Board board;
    setFen(&board, "4k3/8/8/3r4/3q1b2/8/1P6/4K3 b - - 0 1");
    printBoard(board);

    initMoveGenerationTables();

    int length;
    Move* move = legalMoves(board, &length);
    printf("Amount moves: %d\n", length);

    for (int i = 0; i < length; i++) {
        char san[6];
        moveToSan(*move, &san[0]);
        printf("%s\n", san);
        ++move;
    }

    return 0;
}