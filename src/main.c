#include "board.h"
#include <stdio.h>
#include <stdlib.h>

int main() {
    Board board;
    setFen(&board, "4k3/8/8/3r4/3q1b2/8/1P6/4K3 b - - 0 1");
    printBoard(board);

    initMoveGenerationTables();

    Move moves[250];
    int amount = legalMoves(board, moves);
    printf("Amount moves: %d\n", amount);

    for (int i = 0; i < amount; i++) {
        char san[6];
        moveToSan(moves[i], san);
        //printf("%s\n", san);
    }

    return 0;
}