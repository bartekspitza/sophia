#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "board.h"
#include "fen.h"
#include "search.h"
#include "movegen.h"
#include "evaluation.h"
#include "zobrist.h"
#include "san.h"
#include "bitboards.h"


int main(void) {
    initBitboards();
    initEvaluation();
    initMoveGeneration();
    initZobrist();

    Board board;
    setFen(&board, START_FEN);


    int depth = 6;
    int nodesSearched = 0;
    Move bestMove;
    PVline pvLine;

    clock_t start = clock();
    int eval = search(board, depth, &bestMove, &nodesSearched, &pvLine);
    double elapsed = (double) (clock() - start) / CLOCKS_PER_SEC;
    

    printf("Elapsed: %.2fs\n", elapsed);
    return 0;
}
