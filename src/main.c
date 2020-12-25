#include "board.h"
#include <stdio.h>
#include <stdlib.h>

int main() {
    initMoveGenerationTables();

    Board board;

    // White fen
    setFen(&board, "3k4/6P1/8/8/8/8/3QRBNP/3K4 w - - 0 1");
    printBoard(board);
    Move moves[250];
    int amount = legalMoves(board, moves);
    printf("%d\n", amount);

    // Black fen
    setFen(&board, "3k4/3qrbnp/8/8/8/8/6p1/3K4 b - - 0 1");
    printBoard(board);
    amount = legalMoves(board, moves);
    printf("%d\n", amount);

    /*
    for (int i = 0; i < amount; i++) {
        char san[6];
        moveToSan(moves[i], san);
        printf("%s\n", san);
    }
    */

    return 0;
}