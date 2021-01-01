#include "evaluation.h"
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
    5, 10, 10,-20,-20, 10, 10,  5,
    5, -5,-10,  0,  0,-10, -5,  5,
    0,  0,  0, 20, 20,  0,  0,  0,
    5,  5, 10, 25, 25, 10,  5,  5,
    10, 10, 20, 30, 30, 20, 10, 10,
    50, 50, 50, 50, 50, 50, 50, 50,
    0,  0,  0,  0,  0,  0,  0,  0
};
int KNIGHT_W_PST[] = {
    -50,-40,-30,-30,-30,-30,-40,-50,
    -40,-20,  0,  5,  5,  0,-20,-40,
    -30,  5, 10, 15, 15, 10,  5,-30,
    -30,  0, 15, 20, 20, 15,  0,-30,
    -30,  5, 15, 20, 20, 15,  5,-30,
    -30,  0, 10, 15, 15, 10,  0,-30,
    -40,-20,  0,  0,  0,  0,-20,-40,
    -50,-40,-30,-30,-30,-30,-40,-50
};

int BISHOP_W_PST[] = {
    -20,-10,-10,-10,-10,-10,-10,-20,
    -10,  5,  0,  0,  0,  0,  5,-10,
    -10, 10, 10, 10, 10, 10, 10,-10,
    -10,  0, 10, 10, 10, 10,  0,-10,
    -10,  5,  5, 10, 10,  5,  5,-10,
    -10,  0,  5, 10, 10,  5,  0,-10,
    -10,  0,  0,  0,  0,  0,  0,-10,
    -20,-10,-10,-10,-10,-10,-10,-20
};
int ROOK_W_PST[] = {
     0,  0,  5,  10, 10, 5,  0,  0,
    -5,  0,  0,  0,  0,  0,  0, -5,
    -5,  0,  0,  0,  0,  0,  0, -5,
    -5,  0,  0,  0,  0,  0,  0, -5,
    -5,  0,  0,  0,  0,  0,  0, -5,
    -5,  0,  0,  0,  0,  0,  0, -5,
     5,  10, 10, 10, 10, 10, 10, 5,
     0,  0,  0,  0,  0,  0,  0,  0,
};
int QUEEN_W_PST[] = {
    -20,-10,-10, -5, -5,-10,-10,-20
    -10,  0,  5,  0,  0,  0,  0,-10,
    -10,  5,  5,  5,  5,  5,  0,-10,
     0,  0,  5,  5,  5,  5,  0, -5,
    -5,  0,  5,  5,  5,  5,  0, -5,
    -10,  0,  5,  5,  5,  5,  0,-10,
    -10,  0,  0,  0,  0,  0,  0,-10,
    -20,-10,-10, -5, -5,-10,-10,-20,
};
int KING_W_PST[] = {
     20,  30,  10,  0,   0,   10,  30,  20,
     20,  20,  0,   0,   0,   0,   20,  20,
    -10, -20, -20, -20, -20, -20, -20, -10,
    -20, -30, -30, -40, -40, -30, -30, -20,
    -30, -40, -40, -50, -50, -40, -40, -30,
    -30, -40, -40, -50, -50, -40, -40, -30,
    -30, -40, -40, -50, -50, -40, -40, -30,
    -30, -40, -40, -50, -50, -40, -40, -30,
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
        int sq = __builtin_ctzll(board.pawn_W);
        eval += PIECE_VALUES[PAWN_W];
        eval += PAWN_W_PST[sq];
        board.pawn_W &= board.pawn_W -1 ;
    }
    while (board.knight_W) {
        int sq = __builtin_ctzll(board.knight_W);
        eval += PIECE_VALUES[KNIGHT_W];
        eval += KNIGHT_W_PST[sq];
        board.knight_W &= board.knight_W -1 ;
    }
    while (board.bishop_W) {
        int sq = __builtin_ctzll(board.bishop_W);
        eval += PIECE_VALUES[BISHOP_W];
        eval += BISHOP_W_PST[sq];
        board.bishop_W &= board.bishop_W -1 ;
    }
    while (board.rook_W) {
        int sq = __builtin_ctzll(board.rook_W);
        eval += PIECE_VALUES[ROOK_W];
        eval += ROOK_W_PST[sq];
        board.rook_W &= board.rook_W -1 ;
    }
    while (board.queen_W) {
        int sq = __builtin_ctzll(board.queen_W);
        eval += PIECE_VALUES[QUEEN_W];
        eval += QUEEN_W_PST[sq];
        board.queen_W &= board.queen_W -1 ;
    }
    while (board.pawn_B) {
        int sq = __builtin_ctzll(board.pawn_B);
        eval += PIECE_VALUES[PAWN_B];
        eval -= PAWN_B_PST[sq];
        board.pawn_B &= board.pawn_B -1 ;
    }
    while (board.knight_B) {
        int sq = __builtin_ctzll(board.knight_B);
        eval += PIECE_VALUES[KNIGHT_B];
        eval -= KNIGHT_B_PST[sq];
        board.knight_B &= board.knight_B -1 ;
    }
    while (board.bishop_B) {
        int sq = __builtin_ctzll(board.bishop_B);
        eval += PIECE_VALUES[BISHOP_B];
        eval -= BISHOP_B_PST[sq];
        board.bishop_B &= board.bishop_B -1 ;
    }
    while (board.rook_B) {
        int sq = __builtin_ctzll(board.rook_B);
        eval += PIECE_VALUES[ROOK_B];
        eval -= ROOK_B_PST[sq];
        board.rook_B &= board.rook_B -1 ;
    }
    while (board.queen_B) {
        int sq = __builtin_ctzll(board.queen_B);
        eval += PIECE_VALUES[QUEEN_B];
        eval -= QUEEN_B_PST[sq];
        board.queen_B &= board.queen_B -1 ;
    }

    // King square bonuses
    eval += KING_W_PST[board.whiteKingSq];
    eval -= KING_B_PST[board.blackKingSq];

    return eval;
}