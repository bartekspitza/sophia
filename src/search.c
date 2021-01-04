#include <string.h>
#include <stdbool.h>
#include "search.h"
#include "evaluation.h"
#include "board.h"
#include "movegen.h"
#include "tt.h"
#include "zobrist.h"

int max(int a, int b);
int min(int a, int b);
int alphabeta(Board board, Move* move, int depth, int alpha, int beta, int origDepth, int* nodesSearched, PVline* pline);

bool movesAreEqual(Move a, Move b) {
    return a.fromSquare == b.fromSquare && a.toSquare == b.toSquare && a.promotion == b.promotion;
}

void scoreMoves(Board board, TTEntry entry, Move moves[], int cmoves) {
    Move pvMove;
    if (board.hash == entry.zobrist == entry.nodeType == EXACT || entry.nodeType == LOWER) {
        pvMove = entry.move;
    }

    for (int i = 0; i < cmoves; i++) {
        if (movesAreEqual(pvMove, moves[i])) {
            moves[i].score = 100;
        }
    }
}

int selectMove(Move moves[], int cmoves) {
    int bestScore = 0;
    int indx = -1;

    for (int i = 0; i < cmoves; i++) {
        if (!moves[i].exhausted && moves[i].score >= bestScore) {
            bestScore = moves[i].score;
            indx = i;
        }
    }

    if (indx != -1) {
        moves[indx].exhausted = true;
    }

    return indx;
}

int search(Board board, int depth, Move* move, int* nodesSearched, PVline* pv) {
    int eval = alphabeta(board, move, depth, MIN_EVAL, MAX_EVAL, depth, nodesSearched, pv);
    return eval;
}

int alphabeta(Board board, Move* move, int depth, int alpha, int beta, int origDepth, int* nodesSearched, PVline* pline) {
    *nodesSearched += 1;
    int origAlpha = alpha;

    PVline line;

    // TT table lookup
    TTEntry entry = getTTEntry(board.hash);
    if (board.hash == entry.zobrist && entry.depth >= depth) {
        if (entry.nodeType == EXACT) {

            if (origDepth == depth) {
                *move = entry.move;
            }

            return entry.eval;
        } else if (entry.nodeType == LOWER) {
            alpha = max(alpha, entry.eval);
        } else if (entry.nodeType == UPPER) {
            beta = min(beta, entry.eval);
        }

        if (alpha >= beta) {
            if (depth == origDepth) {
                *move = entry.move;
            }
            return entry.eval;
        }
    }

    Move moves[256];
    int cmoves = pseudoLegalMoves(board, moves);

    int res = result(board, moves, cmoves);
    if (res) {
        int eval = evaluate(board, res);

        if (res != DRAW) {
            eval += (1 * origDepth - depth) * (board.turn ? 1 : -1);
        }

        pline->length = 0;
        return eval * (board.turn ? 1 : -1);
    } else if (depth == 0) {
        pline->length = 0;
        return evaluate(board, res) * (board.turn ? 1 : -1);
    }

    int eval = MIN_EVAL;
    scoreMoves(board, entry, moves, cmoves);
    int nextMove;

    while (nextMove = selectMove(moves, cmoves), nextMove != -1) {
        // Validate move if it hasn't
        if (moves[nextMove].validation == NOT_VALIDATED)
            validateMove(board, &moves[nextMove]);

        if (moves[nextMove].validation == LEGAL) {
            Board child = board;
            pushMove(&child, moves[nextMove]);

            int childEval = -alphabeta(child, move, depth-1, -beta, -alpha, origDepth, nodesSearched, &line);

            if (childEval > eval) {
                eval = childEval;

                if (depth == origDepth) {
                    *move = moves[nextMove];
                }
            }

            if (childEval > alpha) {
                alpha = childEval;

                // Store the PV line
                pline->moves[0] = moves[nextMove];
                memcpy(pline->moves + 1, line.moves, line.length * sizeof(Move));
                pline->length = line.length + 1;
            }

            if (alpha >= beta) {
                break;
            }
        }
    }

    addTTEntry(board, eval, *move, depth, beta, origAlpha);

    return eval;
}

int min(int a, int b) {
    return (a < b) ? a : b;
}

int max(int a, int b) {
    return (a > b) ? a : b;
}