#include <string.h>
#include <stdbool.h>
#include "search.h"
#include "evaluation.h"
#include "board.h"
#include "movegen.h"
#include "tt.h"
#include "zobrist.h"
#include "moveorderer.h"

#define min(a, b) (a < b) ? a : b;
#define max(a, b) (a > b) ? a : b;

int alphabeta(Board board, int depth, int alpha, int beta);

// Search variables
int SEARCH_NODES_SEARCHED = 0;
Move SEARCH_BEST_MOVE;
int _DEPTH;


int search(Board board, int depth) {
    // Reset previous search results
    memset(&SEARCH_BEST_MOVE, 0, sizeof(Move));
    SEARCH_NODES_SEARCHED = 0;
    _DEPTH = depth;

    int eval = alphabeta(board, depth, MIN_EVAL, MAX_EVAL);
    return eval;
}

int alphabeta(Board board, int depth, int alpha, int beta) {
    SEARCH_NODES_SEARCHED++;
    int origAlpha = alpha;

    // TT table lookup
    TTEntry entry = getTTEntry(board.hash);
    if (board.hash == entry.zobrist && entry.depth >= depth) {

        if (entry.nodeType == EXACT) {

            if (depth == _DEPTH)
                SEARCH_BEST_MOVE = entry.move;

            return entry.eval;
        } else if (entry.nodeType == LOWER) {
            alpha = max(alpha, entry.eval);
        } else if (entry.nodeType == UPPER) {
            beta = min(beta, entry.eval);
        }

        if (alpha >= beta) {
            if (depth == _DEPTH)
                SEARCH_BEST_MOVE = entry.move;

            return entry.eval;
        }
    }

    Move moves[256];
    int cmoves = legalMoves(&board, moves);

    int res = result(board, moves, cmoves);
    if (res) {
        int eval = evaluate(board, res);

        if (res != DRAW)
            eval += (1 * _DEPTH - depth) * (board.turn ? 1 : -1);

        return eval * (board.turn ? 1 : -1);
    } else if (depth == 0) {
        return evaluate(board, res) * (board.turn ? 1 : -1);
    }

    int nextMove;
    int eval = MIN_EVAL;
    Move best_move;
    score_moves(board, entry, moves, cmoves);

    while (nextMove = select_move(moves, cmoves), nextMove != -1) {
        if (moves[nextMove].validation == LEGAL) {
            Board child = board;
            pushMove(&child, moves[nextMove]);
            int childEval = -alphabeta(child, depth-1, -beta, -alpha);

            if (childEval > eval) {
                eval = childEval;
                best_move = moves[nextMove];

                if (depth == _DEPTH) 
                    SEARCH_BEST_MOVE = best_move; 
            }

            alpha = max(alpha, childEval);

            if (alpha >= beta)
                break;
        }
    }

    addTTEntry(board, eval, best_move, depth, beta, origAlpha);

    return eval;
}
