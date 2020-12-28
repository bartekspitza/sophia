#include "board.h"
#include "utils.h"
#include <stdio.h>
#include <stdlib.h>

int main() {
    initMoveGeneration();

    Bitboard test = 9;
    printf("%lu\n", test);
    printBits(test);

    test = popBit(test, 3);
    printf("%lu\n", test);
    printBits(test);

    test = popBit(test, 0);
    printf("%lu\n", test);
    printBits(test);

    /*
    Board board;
    setFen(&board, START_FEN);
    printBoard(board);

    Move moves[250];
    int length = legalMoves(board, moves);
    printMoves(moves, length);
    */


    return 0;
}