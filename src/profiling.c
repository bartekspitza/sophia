#include <stdio.h>
#include <stdlib.h>
#include <string.h>
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

    Move moves[250];
    int cmoves = pseudoLegalMoves(board, moves);
    int res = result(board, moves, cmoves);

    int depth = 4;
    while (res == UN_DETERMINED) {

        int nodesSearched = 0;

        Move bestMove;
        PVline pvLine;
        int eval = search(board, depth, &bestMove, &nodesSearched, &pvLine);

        pushMove(&board, bestMove);
        
        cmoves = pseudoLegalMoves(board, moves);
        res = result(board, moves, cmoves);
    }
    return 0;
}
