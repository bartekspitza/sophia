#include "board.h"
#include <stdio.h>
#include <stdlib.h>

int main() {
    Board board;
    setFen(&board, "3k4/6P1/8/8/8/8/8/3K4 w - - 0 1");
    printBoard(board);

    initMoveGenerationTables();

    Move moves[250];
    int amount = legalMoves(board, moves);
    printf("Amount moves: %d\n", amount);

    for (int i = 0; i < amount; i++) {
        char san[6];
        moveToSan(moves[i], san);
        printf("%s\n", san);
    }

    return 0;
}