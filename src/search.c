#include "search.h"
#include "evaluation.h"
#include "board.h"
#include "movegen.h"

int max(int a, int b) {
    if (a > b) return a;
    else return b;
}

int negamax(Board board, Move* move, int depth, int alpha, int beta, int origDepth, int* nodesSearched) {
    *nodesSearched += 1;

    int res = result(board);
    if (res != NOT_DETERMINED) {
        int eval = evaluate(board);    

        if (res != DRAW) {
            eval += (1 * origDepth - depth) * (board.turn ? 1 : -1);
        }

        return eval * (board.turn ? 1 : -1);
    } else if (depth == 0) {
        return evaluate(board) * (board.turn ? 1 : -1);
    }

    int eval = MIN_EVAL;
    Move moves[250];
    int numMoves = legalMoves(board, moves);

    for (int i = 0; i < numMoves; i++) {
        Board child = board;
        pushMove(&child, moves[i]);

        int childEval = -negamax(child, move, depth-1, -beta, -alpha, origDepth, nodesSearched);

        if (childEval > eval) {
            eval = childEval;

            if (depth == origDepth) {
                *move = moves[i];
            }
        }

        alpha = max(alpha, childEval);
        if (alpha >= beta) {
            break;
        }
    }

    return eval;
}

int search(Board board, Move* move, int* nodesSearched) {
    *nodesSearched = 0;
    int depth = 4;
    int eval = negamax(board, move, depth, MIN_EVAL, MAX_EVAL, depth, nodesSearched);

    return eval;
}