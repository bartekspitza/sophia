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
int alphabeta(Board board, Move* move, int depth, int alpha, int beta, int origDepth, int* nodesSearched);

bool movesAreEqual(Move a, Move b) {
    return a.fromSquare == b.fromSquare && a.toSquare == b.toSquare && a.promotion == b.promotion;
}

int VICTIMS[] = {
    100 * 16,
    320 * 16,
    330 * 16,
    500 * 16,
    900 * 16,
};
int ATTACKERS[] = {
    100,
    200,
    300,
    400,
    500,
    600,
    100,
    200,
    300,
    400,
    500,
    600,
};

const int MAX_MOVE_SCORE = 100000;
const int MVV_LVA_PAWN_EXCHANGE = (100 * 16) - 100;

void scoreMoves(Board board, TTEntry entry, Move moves[], int cmoves) {
    Move pvMove;
    if (board.hash == entry.zobrist && entry.nodeType < UPPER) {
        pvMove = entry.move;
    }

    for (int i = 0; i < cmoves; i++) {
        Move* move = &moves[i];

        // Pv Move
        if (movesAreEqual(pvMove, *move)) {
            move->score = MAX_MOVE_SCORE;
        
        // MVV_LVA
        } else {
            Bitboard enemyOcc = board.turn ? board.occupancyBlack : board.occupancyWhite;
            bool isCapture = enemyOcc & SQUARE_BITBOARDS[move->toSquare];
            bool isEnPassant = board.epSquare == move->toSquare;

            if (isEnPassant) {
                moves[i].score = MVV_LVA_PAWN_EXCHANGE;
            } else if (isCapture) {
                // Find captured piece type
                int capturedPiece;
                Bitboard toSquare = SQUARE_BITBOARDS[moves[i].toSquare];
                Bitboard* bb = board.turn ? &board.pawn_B : &board.pawn_W;
                for (int i = 0; i < 5; i++) {
                    if (toSquare & *bb) {
                        capturedPiece = i;
                        break;
                    }
                    bb++;
                }

                move->score = VICTIMS[capturedPiece] - ATTACKERS[move->pieceType];
            }
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

int search(Board board, int depth, Move* move, int* nodesSearched) {
    int eval = alphabeta(board, move, depth, MIN_EVAL, MAX_EVAL, depth, nodesSearched);
    return eval;
}

int alphabeta(Board board, Move* move, int depth, int alpha, int beta, int origDepth, int* nodesSearched) {
    *nodesSearched += 1;
    int origAlpha = alpha;


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

        return eval * (board.turn ? 1 : -1);
    } else if (depth == 0) {
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

            int childEval = -alphabeta(child, move, depth-1, -beta, -alpha, origDepth, nodesSearched);

            if (childEval > eval) {
                eval = childEval;

                if (depth == origDepth) {
                    *move = moves[nextMove];
                }
            }

            if (childEval > alpha) {
                alpha = childEval;
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