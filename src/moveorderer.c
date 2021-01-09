#include "moveorderer.h"
#include "bitboards.h"

const int VICTIMS[] = {
    100 * 16,
    320 * 16,
    330 * 16,
    500 * 16,
    900 * 16,
};
const int ATTACKERS[] = {
    100,
    200,
    300,
    400,
    500,
    600,
    100, 200, 300, 400, 500, 600, // Same as first 6 for easier indexing
};

const int MAX_MOVE_SCORE = 100000;
const int MVV_LVA_PAWN_EXCHANGE = (100 * 16) - 100;


bool moves_are_equal(Move a, Move b);

void score_moves(Board board, TTEntry entry, Move moves[], int cmoves) {
    Move pvMove;
    if (board.hash == entry.zobrist && entry.nodeType < UPPER) {
        pvMove = entry.move;
    }

    for (int i = 0; i < cmoves; i++) {
        Move* move = &moves[i];

        // Pv Move
        if (moves_are_equal(pvMove, *move)) {
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

int select_move(Move moves[], int cmoves) {
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

bool moves_are_equal(Move a, Move b) {
    return a.fromSquare == b.fromSquare && a.toSquare == b.toSquare && a.promotion == b.promotion;
}
