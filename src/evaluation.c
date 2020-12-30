#include "evaluation.h"
#include "movegen.h"
#include "board.h"

int PIECE_VALUES[] = {
    100,
    320,
    330,
    500,
    900,
    2000,
   -100,
   -320,
   -330,
   -500,
   -900,
   -2000
};

int PAWN_W_PST[] = {
    0,  0,  0,  0,  0,  0,  0,  0,
    50, 50, 50, 50, 50, 50, 50, 50,
    10, 10, 20, 30, 30, 20, 10, 10,
    5,  5, 10, 25, 25, 10,  5,  5,
    0,  0,  0, 20, 20,  0,  0,  0,
    5, -5,-10,  0,  0,-10, -5,  5,
    5, 10, 10,-20,-20, 10, 10,  5,
    0,  0,  0,  0,  0,  0,  0,  0
};
int KNIGHT_W_PST[] = {
    -50,-40,-30,-30,-30,-30,-40,-50,
    -40,-20,  0,  0,  0,  0,-20,-40,
    -30,  0, 10, 15, 15, 10,  0,-30,
    -30,  5, 15, 20, 20, 15,  5,-30,
    -30,  0, 15, 20, 20, 15,  0,-30,
    -30,  5, 10, 15, 15, 10,  5,-30,
    -40,-20,  0,  5,  5,  0,-20,-40,
    -50,-40,-30,-30,-30,-30,-40,-50
};
int BISHOP_W_PST[] = {
    -20,-10,-10,-10,-10,-10,-10,-20,
    -10,  0,  0,  0,  0,  0,  0,-10,
    -10,  0,  5, 10, 10,  5,  0,-10,
    -10,  5,  5, 10, 10,  5,  5,-10,
    -10,  0, 10, 10, 10, 10,  0,-10,
    -10, 10, 10, 10, 10, 10, 10,-10,
    -10,  5,  0,  0,  0,  0,  5,-10,
    -20,-10,-10,-10,-10,-10,-10,-20
};
int ROOK_W_PST[] = {
    0,  0,  0,  0,  0,  0,  0,  0,
    5, 10, 10, 10, 10, 10, 10,  5,
    -5,  0,  0,  0,  0,  0,  0, -5,
    -5,  0,  0,  0,  0,  0,  0, -5,
    -5,  0,  0,  0,  0,  0,  0, -5,
    -5,  0,  0,  0,  0,  0,  0, -5,
    -5,  0,  0,  0,  0,  0,  0, -5,
    0,  0,  5,  10,  10,  5,  0,  0
};
int QUEEN_W_PST[] = {
    -20,-10,-10, -5, -5,-10,-10,-20,
    -10,  0,  0,  0,  0,  0,  0,-10,
    -10,  0,  5,  5,  5,  5,  0,-10,
    -5,  0,  5,  5,  5,  5,  0, -5,
     0,  0,  5,  5,  5,  5,  0, -5,
    -10,  5,  5,  5,  5,  5,  0,-10,
    -10,  0,  5,  0,  0,  0,  0,-10,
    -20,-10,-10, -5, -5,-10,-10,-20
};
int KING_W_PST[] = {
    -30, -40, -40, -50, -50, -40, -40, -30,
    -30, -40, -40, -50, -50, -40, -40, -30,
    -30, -40, -40, -50, -50, -40, -40, -30,
    -30, -40, -40, -50, -50, -40, -40, -30,
    -20, -30, -30, -40, -40, -30, -30, -20,
    -10, -20, -20, -20, -20, -20, -20, -10,
     20,  20,  0,   0,   0,   0,   20,  20,
     20,  30,  10,  0,   0,   10,  30,  20
};

int PAWN_B_PST[64];
int KNIGHT_B_PST[64];
int BISHOP_B_PST[64];
int ROOK_B_PST[64];
int QUEEN_B_PST[64];
int KING_B_PST[64];

int MIN_EVAL = -100000;
int MAX_EVAL = 100000;

void initEvaluation(void) {
    for (int i = 0; i < 64; i++) {
        PAWN_B_PST[i] = PAWN_W_PST[63-i];
        KNIGHT_B_PST[i] = KNIGHT_W_PST[63-i];
        BISHOP_B_PST[i] = BISHOP_W_PST[63-i];
        ROOK_B_PST[i] = ROOK_W_PST[63-i];
        QUEEN_B_PST[i] = QUEEN_W_PST[63-i];
        KING_B_PST[i] = KING_W_PST[63-i];
    }
}

int evaluate(Board board, int result) {
    if (result == DRAW) return 0;
    else if (result == WHITE_WIN) return MAX_EVAL;
    else if (result == BLACK_WIN) return MIN_EVAL;

    int eval = 0;

    while (board.pawn_W) {
        int indx = bitScanForward(&board.pawn_W);
        eval += PIECE_VALUES[PAWN_W];
    }
    while (board.knight_W) {
        int indx = bitScanForward(&board.knight_W);
        eval += PIECE_VALUES[KNIGHT_W];
    }
    while (board.bishop_W) {
        int indx = bitScanForward(&board.bishop_W);
        eval += PIECE_VALUES[BISHOP_W];
    }
    while (board.rook_W) {
        int indx = bitScanForward(&board.rook_W);
        eval += PIECE_VALUES[ROOK_W];
    }
    while (board.queen_W) {
        int indx = bitScanForward(&board.queen_W);
        eval += PIECE_VALUES[QUEEN_W];
    }
    while (board.pawn_B) {
        int indx = bitScanForward(&board.pawn_B);
        eval += PIECE_VALUES[PAWN_B];
    }
    while (board.knight_B) {
        int indx = bitScanForward(&board.knight_B);
        eval += PIECE_VALUES[KNIGHT_B];
    }
    while (board.bishop_B) {
        int indx = bitScanForward(&board.bishop_B);
        eval += PIECE_VALUES[BISHOP_B];
    }
    while (board.rook_B) {
        int indx = bitScanForward(&board.rook_B);
        eval += PIECE_VALUES[ROOK_B];
    }
    while (board.queen_B) {
        int indx = bitScanForward(&board.queen_B);
        eval += PIECE_VALUES[QUEEN_B];
    }

    return eval;
}