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

    Move moves[250];
    int numMoves = legalMoves(board, moves);

    int res = result(board, numMoves);
    if (res) {
        int eval = evaluate(board, res);

        if (res != DRAW) {
            eval += (1 * origDepth - depth) * (board.turn ? 1 : -1);
        }

        return eval * (board.turn ? 1 : -1);
    } else if (depth == 0) {
        return evaluate(board, res) * (board.turn ? 1 : -1);
    }

    int eval = MIN_EVAL;
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

int search(Board board, int depth, Move* move, int* nodesSearched) {
    int eval = negamax(board, move, depth, MIN_EVAL, MAX_EVAL, depth, nodesSearched);
    return eval;
}