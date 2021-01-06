#include <stdlib.h>
#include "zobrist.h"

Bitboard randomBitboard();

Bitboard PIECES[12][64];
Bitboard EN_PASSANT[64];
Bitboard CASTLING[16];
Bitboard WHITE_TO_MOVE;

Bitboard hash(Board board) {
    Bitboard hash = 0LL;

    // Castling rights
    hash ^= CASTLING[board.castling];

    // Pieces
    for (int i = 0; i < 12; i++) {
        Bitboard bb = *(&(board.pawn_W)+i);

        while (bb) {
            int square = __builtin_ctzll(bb);
            hash ^= PIECES[i][square];
            bb &= bb -1;
        }
    }

    // En passant
    if (board.epSquare != -1) {
        hash ^= EN_PASSANT[board.epSquare];
    }

    // Side to move
    if (board.turn) {
        hash ^= WHITE_TO_MOVE;
    }

    return hash;
}

void initZobrist() {
    for (int i = 0; i < 12; i++) for (int j = 0; j < 64;j++) PIECES[i][j] = randomBitboard();
    for (int i = 0; i < 64; i++) EN_PASSANT[i] = randomBitboard();
    for (int i = 0; i < 16; i++) CASTLING[i] = randomBitboard();
    WHITE_TO_MOVE = randomBitboard();
}

Bitboard randomBitboard(void) {
    Bitboard r = 0;
    for (int i=0; i < 64; i++) {
        Bitboard tmp = (Bitboard) rand() % 2;
        r |= tmp << i;
    }
    return r;
}
