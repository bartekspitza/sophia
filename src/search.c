#include "search.h"
#include "evaluation.h"
#include "board.h"
#include "movegen.h"
#include "tt.h"

int max(int a, int b);
int min(int a, int b);
int negamax(Board board, Move* move, int depth, int alpha, int beta, int origDepth, int* nodesSearched);


/*

        # TT table lookup
        entry = self.TT.get(board, piece_map)
        if entry and entry.depth >= depth:
            if entry.node_type == NodeType.EXACT:
                return entry.value, entry.move
            elif entry.node_type == NodeType.LOWER:
                alpha = max(alpha, entry.value)
            elif entry.node_type == NodeType.UPPER:
                beta = min(beta, entry.value)
            
            if alpha >= beta:
                return entry.value, entry.move
                */

int search(Board board, int depth, Move* move, int* nodesSearched) {
    int eval = negamax(board, move, depth, MIN_EVAL, MAX_EVAL, depth, nodesSearched);
    return eval;
}

int negamax(Board board, Move* move, int depth, int alpha, int beta, int origDepth, int* nodesSearched) {
    *nodesSearched += 1;
    int origAlpha = alpha;

    // TT table lookup
    Bitboard zobrist;
    TTEntry entry = get(board, &zobrist);
    if (zobrist == entry.zobrist && entry.depth >= depth) {
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

    addTTEntry(board, eval, *move, depth, beta, origAlpha);

    return eval;
}

int min(int a, int b) {
    if (a < b) return a;
    return b;
}

int max(int a, int b) {
    if (a > b) return a;
    return b;
}