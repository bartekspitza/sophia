#include "board.h"
#include <stdio.h>
#include <stdlib.h>

int main() {
    Board board;
    setFen(&board, "4k3/8/8/4p3/3P4/8/8/K7 w - - 0 1");
    printBoard(board);

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