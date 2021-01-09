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
#include "tt.h"


int main(void) {
    initBitboards();
    initEvaluation();
    initMoveGeneration();
    initZobrist();

    Board board;
    setFen(&board, START_FEN);


    int depth = 6;
    Move bestMove;

    clock_t start = clock();
    for (int i = 0; i < 3;i++) {
        int eval = search(board, depth, &bestMove);
        memset(TT_TABLE, 0, sizeof(TTEntry) * TT_SIZE);
    }
    double elapsed = (double) (clock() - start) / CLOCKS_PER_SEC;
    

    printf("Elapsed: %.2fs\n", elapsed);
    return 0;
}
