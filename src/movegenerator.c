#include <stdlib.h>
#include "movegenerator.h"
#include "utils.h"

Bitboard pawnStartWhite = 0xFF00;
Bitboard pawnStartBlack = 0x00FF000000000000;

/*
enum SQUARES {
    H1, G1, F1, E1, D1, C1, B1, A1,
    H2, G2, F2, E2, D2, C2, B2, A2,
    H3, G3, F3, E3, D3, C3, B3, A3,
    H4, G4, F4, E4, D4, C4, B4, A4,
    H5, G5, F5, E5, D5, C5, B5, A5,
    H6, G6, F6, E6, D6, C6, B6, A6,
    H7, G7, F7, E7, D7, C7, B7, A7,
    H8, G8, F8, E8, D8, C8, B8, A8
};
*/

Bitboard PAWN_W_ATTACKS_EAST[64] = {
    0,       0,       0,       0,       0,       0,       0,       0,

    0, 1LL<<H3, 1LL<<G3, 1LL<<F3, 1LL<<E3, 1LL<<D3, 1LL<<C3, 1LL<<B3,
    0, 1LL<<H4, 1LL<<G4, 1LL<<F4, 1LL<<E4, 1LL<<D4, 1LL<<C4, 1LL<<B4,
    0, 1LL<<H5, 1LL<<G5, 1LL<<F5, 1LL<<E5, 1LL<<D5, 1LL<<C5, 1LL<<B5,
    0, 1LL<<H6, 1LL<<G6, 1LL<<F6, 1LL<<E6, 1LL<<D6, 1LL<<C6, 1LL<<B6,
    0, 1LL<<H7, 1LL<<G7, 1LL<<F7, 1LL<<E7, 1LL<<D7, 1LL<<C7, 1LL<<B7,
    0, 1LL<<H8, 1LL<<G8, 1LL<<F8, 1LL<<E8, 1LL<<D8, 1LL<<C8, 1LL<<B8,

    0,       0,       0,       0,       0,       0,       0,       0,
};
Bitboard PAWN_W_ATTACKS_WEST[64] = {
    0,    0,       0,       0,       0,       0,       0,       0,

    1LL<<G3, 1LL<<F3, 1LL<<E3, 1LL<<D3, 1LL<<C3, 1LL<<B3, 1LL<<A3, 0,
    1LL<<G4, 1LL<<F4, 1LL<<E4, 1LL<<D4, 1LL<<C4, 1LL<<B4, 1LL<<A4, 0,
    1LL<<G5, 1LL<<F5, 1LL<<E5, 1LL<<D5, 1LL<<C5, 1LL<<B5, 1LL<<A5, 0,
    1LL<<G6, 1LL<<F6, 1LL<<E6, 1LL<<D6, 1LL<<C6, 1LL<<B6, 1LL<<A6, 0,
    1LL<<G7, 1LL<<F7, 1LL<<E7, 1LL<<D7, 1LL<<C7, 1LL<<B7, 1LL<<A7, 0,
    1LL<<G8, 1LL<<F8, 1LL<<E8, 1LL<<D8, 1LL<<C8, 1LL<<B8, 1LL<<A8, 0,

    0,    0,       0,       0,       0,       0,       0,       0,
};
Bitboard PAWN_B_ATTACKS_EAST[64] = {
    0,    0,       0,       0,       0,       0,       0,       0,

    1LL<<G1, 1LL<<F1, 1LL<<E1, 1LL<<D1, 1LL<<C1, 1LL<<B1, 1LL<<A1, 0,
    1LL<<G2, 1LL<<F2, 1LL<<E2, 1LL<<D2, 1LL<<C2, 1LL<<B2, 1LL<<A2, 0,
    1LL<<G3, 1LL<<F3, 1LL<<E3, 1LL<<D3, 1LL<<C3, 1LL<<B3, 1LL<<A3, 0,
    1LL<<G4, 1LL<<F4, 1LL<<E4, 1LL<<D4, 1LL<<C4, 1LL<<B4, 1LL<<A4, 0,
    1LL<<G5, 1LL<<F5, 1LL<<E5, 1LL<<D5, 1LL<<C5, 1LL<<B5, 1LL<<A5, 0,
    1LL<<G6, 1LL<<F6, 1LL<<E6, 1LL<<D6, 1LL<<C6, 1LL<<B6, 1LL<<A6, 0,

    0,    0,       0,       0,       0,       0,       0,        0,
};
Bitboard PAWN_B_ATTACKS_WEST[64] = {
    0,       0,       0,       0,       0,       0,       0,       0,

    0, 1LL<<H1, 1LL<<G1, 1LL<<F1, 1LL<<E1, 1LL<<D1, 1LL<<C1, 1LL<<B1,
    0, 1LL<<H2, 1LL<<G2, 1LL<<F2, 1LL<<E2, 1LL<<D2, 1LL<<C2, 1LL<<B2,
    0, 1LL<<H3, 1LL<<G3, 1LL<<F3, 1LL<<E3, 1LL<<D3, 1LL<<C3, 1LL<<B3,
    0, 1LL<<H4, 1LL<<G4, 1LL<<F4, 1LL<<E4, 1LL<<D4, 1LL<<C4, 1LL<<B4,
    0, 1LL<<H5, 1LL<<G5, 1LL<<F5, 1LL<<E5, 1LL<<D5, 1LL<<C5, 1LL<<B5,
    0, 1LL<<H6, 1LL<<G6, 1LL<<F6, 1LL<<E6, 1LL<<D6, 1LL<<C6, 1LL<<B6,

    0,       0,       0,       0,       0,       0,       0,       0,
};

Move* legalMoves(Board board, int* length) {
    Move* first = malloc(sizeof(Move));
    Move* move = first;

    Bitboard occupancyWhite = 0;
    Bitboard occupancyBlack = 0;
    for (int i = 0; i < 6;i++) {
        occupancyWhite |= *(&(board.pawn_W)+i);
        occupancyBlack |= *(&(board.pawn_B)+i);
    }
    Bitboard occupancy = occupancyBlack | occupancyWhite;

    if (board.turn) {
        // Pawn pushes
        Bitboard singlePush = board.pawn_W << 8;
        singlePush ^= singlePush & occupancy;

        Bitboard doublePush = (board.pawn_W & pawnStartWhite) << 8*2;
        doublePush ^= doublePush & occupancy;
        doublePush = doublePush >> 8;
        doublePush &= singlePush;
        doublePush = doublePush << 8;

        for (int i = 0; i < 64;i++) {
            if (getBit(singlePush, i)) {
                move->fromSquare = i-8;
                move->toSquare = i;
                move++;
                (*length)++;
            }
            if (getBit(doublePush, i)) {
                move->fromSquare = i-8*2;
                move->toSquare = i;
                move++;
                (*length)++;
            }
        }

        // Pawn attacks
        for (int i = 0; i < 64;i++) {
            if (getBit(board.pawn_W, i)) {
                if (PAWN_W_ATTACKS_EAST[i] & occupancyBlack) {
                    move->fromSquare = i;
                    move->toSquare = i+7;
                    move++;
                    (*length)++;
                }
                
                if (PAWN_W_ATTACKS_WEST[i] & occupancyBlack) {
                    move->fromSquare = i;
                    move->toSquare = i+9;
                    move++;
                    (*length)++;
                }
            }
        }

    } else {
        // Pawn pushes
        Bitboard singlePush = board.pawn_B >> 8;
        singlePush ^= singlePush & occupancy;

        Bitboard doublePush = (board.pawn_B & pawnStartBlack) >> 8*2;
        doublePush ^= doublePush & occupancy;
        doublePush = doublePush << 8;
        doublePush &= singlePush;
        doublePush = doublePush >> 8;

        for (int i = 0; i < 64;i++) {
            if (getBit(singlePush, i)) {
                move->fromSquare = i+8;
                move->toSquare = i;
                move++;
                (*length)++;
            }
            if (getBit(doublePush, i)) {
                move->fromSquare = i+8*2;
                move->toSquare = i;
                move++;
                (*length)++;
            }
        }

        // Pawn attacks
        for (int i = 0; i < 64;i++) {
            if (getBit(board.pawn_B, i)) {
                if (PAWN_B_ATTACKS_EAST[i] & occupancyWhite) {
                    move->fromSquare = i;
                    move->toSquare = i-7;
                    move++;
                    (*length)++;
                }
                
                if (PAWN_B_ATTACKS_WEST[i] & occupancyWhite) {
                    move->fromSquare = i;
                    move->toSquare = i-9;
                    move++;
                    (*length)++;
                }
            }
        }
    }

    return first;
}

void sanToMove(Move* move, char* san) {
    int fromFile = 'h' - san[0];
    int fromRank = atoi(&san[1]) - 1;
    int toFile = 'h' - san[2];
    int toRank = atoi(&san[3]) - 1;
    move->fromSquare = 8 * (fromRank) + fromFile;
    move->toSquare = 8 * (toRank) + toFile;
}

void moveToSan(Move move, char* san) {
    san[0] = SQUARE_NAMES[move.fromSquare][0];
    san[1] = SQUARE_NAMES[move.fromSquare][1];
    san[2] = SQUARE_NAMES[move.toSquare][0];
    san[3] = SQUARE_NAMES[move.toSquare][1];
}