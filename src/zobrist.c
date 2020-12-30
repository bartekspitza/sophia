#include <stdlib.h>
#include <stdio.h>
#include "zobrist.h"
#include "movegen.h"


unsigned int randomU32();
Bitboard randomBitboard();

Bitboard PIECES[12][64];
Bitboard EN_PASSANT[64];
Bitboard CASTLING[16];
Bitboard WHITE_TO_MOVE;

Bitboard hash(Board board) {
    // Castling rights
    Bitboard hash = CASTLING[board.castling];

    // Pieces
    for (int i = 0; i < 12; i++) {
        Bitboard bb = *(&(board.pawn_W)+i);

        while (bb) {
            int square = bitScanForward(&bb);
            hash ^= PIECES[i][square];
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
    // Pieces
    for (int i = 0; i < 12; i++) for (int j = 0; j < 64;j++) PIECES[i][j] = randomBitboard();

    // En passant
    for (int i = 0; i < 64; i++) EN_PASSANT[i] = randomBitboard();

    // Castling
    for (int i = 0; i < 16; i++) CASTLING[i] = randomBitboard();

    WHITE_TO_MOVE = randomBitboard();
}

unsigned int RANDOM_STATE = 1804289383;

unsigned int randomU32() {
    unsigned int number = RANDOM_STATE;
    number ^= number << 13;
    number ^= number >> 17;
    number ^= number << 5;
    RANDOM_STATE = number;
    return number;
}

Bitboard randomBitboard() {
    Bitboard n1, n2, n3, n4;
    
    // init random numbers slicing 16 bits from MS1B side
    n1 = (Bitboard)(randomU32()) & 0xFFFF;
    n2 = (Bitboard)(randomU32()) & 0xFFFF;
    n3 = (Bitboard)(randomU32()) & 0xFFFF;
    n4 = (Bitboard)(randomU32()) & 0xFFFF;
    
    return n1 | (n2 << 16) | (n3 << 32) | (n4 << 48);
}


