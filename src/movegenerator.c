#include <stdlib.h>
#include "movegenerator.h"
#include "utils.h"

#define bit(a) (1LL<<a)
#define _3(a,b,c)           (1LL<<a)|(1LL<<b)|(1LL<<c)
#define _4(a,b,c,d)         (1LL<<a)|(1LL<<b)|(1LL<<c)|(1LL<<d)
#define _5(a,b,c,d,e)       (1LL<<a)|(1LL<<b)|(1LL<<c)|(1LL<<d)|(1LL<<e)
#define _6(a,b,c,d,e,f)     (1LL<<a)|(1LL<<b)|(1LL<<c)|(1LL<<d)|(1LL<<e)|(1LL<<f)
#define _7(a,b,c,d,e,f,g)   (1LL<<a)|(1LL<<b)|(1LL<<c)|(1LL<<d)|(1LL<<e)|(1LL<<f)|(1LL<<g)
#define _8(a,b,c,d,e,f,g,h) (1LL<<a)|(1LL<<b)|(1LL<<c)|(1LL<<d)|(1LL<<e)|(1LL<<f)|(1LL<<g)|(1LL<<h)

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
    0, bit(H3), bit(G3), bit(F3), bit(E3), bit(D3), bit(C3), bit(B3),
    0, bit(H4), bit(G4), bit(F4), bit(E4), bit(D4), bit(C4), bit(B4),
    0, bit(H5), bit(G5), bit(F5), bit(E5), bit(D5), bit(C5), bit(B5),
    0, bit(H6), bit(G6), bit(F6), bit(E6), bit(D6), bit(C6), bit(B6),
    0, bit(H7), bit(G7), bit(F7), bit(E7), bit(D7), bit(C7), bit(B7),
    0, bit(H8), bit(G8), bit(F8), bit(E8), bit(D8), bit(C8), bit(B8),
    0,       0,       0,       0,       0,       0,       0,       0,
};
Bitboard PAWN_W_ATTACKS_WEST[64] = {
    0,    0,       0,       0,       0,       0,       0,       0,
    bit(G3), bit(F3), bit(E3), bit(D3), bit(C3), bit(B3), bit(A3), 0,
    bit(G4), bit(F4), bit(E4), bit(D4), bit(C4), bit(B4), bit(A4), 0,
    bit(G5), bit(F5), bit(E5), bit(D5), bit(C5), bit(B5), bit(A5), 0,
    bit(G6), bit(F6), bit(E6), bit(D6), bit(C6), bit(B6), bit(A6), 0,
    bit(G7), bit(F7), bit(E7), bit(D7), bit(C7), bit(B7), bit(A7), 0,
    bit(G8), bit(F8), bit(E8), bit(D8), bit(C8), bit(B8), bit(A8), 0,
    0,    0,       0,       0,       0,       0,       0,       0,
};
Bitboard PAWN_B_ATTACKS_EAST[64] = {
    0,    0,       0,       0,       0,       0,       0,       0,
    bit(G1), bit(F1), bit(E1), bit(D1), bit(C1), bit(B1), bit(A1), 0,
    bit(G2), bit(F2), bit(E2), bit(D2), bit(C2), bit(B2), bit(A2), 0,
    bit(G3), bit(F3), bit(E3), bit(D3), bit(C3), bit(B3), bit(A3), 0,
    bit(G4), bit(F4), bit(E4), bit(D4), bit(C4), bit(B4), bit(A4), 0,
    bit(G5), bit(F5), bit(E5), bit(D5), bit(C5), bit(B5), bit(A5), 0,
    bit(G6), bit(F6), bit(E6), bit(D6), bit(C6), bit(B6), bit(A6), 0,
    0,    0,       0,       0,       0,       0,       0,        0,
};
Bitboard PAWN_B_ATTACKS_WEST[64] = {
    0,       0,       0,       0,       0,       0,       0,       0,
    0, bit(H1), bit(G1), bit(F1), bit(E1), bit(D1), bit(C1), bit(B1),
    0, bit(H2), bit(G2), bit(F2), bit(E2), bit(D2), bit(C2), bit(B2),
    0, bit(H3), bit(G3), bit(F3), bit(E3), bit(D3), bit(C3), bit(B3),
    0, bit(H4), bit(G4), bit(F4), bit(E4), bit(D4), bit(C4), bit(B4),
    0, bit(H5), bit(G5), bit(F5), bit(E5), bit(D5), bit(C5), bit(B5),
    0, bit(H6), bit(G6), bit(F6), bit(E6), bit(D6), bit(C6), bit(B6),
    0,       0,       0,       0,       0,       0,       0,       0,
};
Bitboard KING_MOVEMENT[64] = {
_3(G1,G2,H2)      ,_5(F1,F2,G2,H2,H1)         ,_5(E1,E2,F2,G2,G1)         ,_5(D1,D2,E2,F2,F1)         ,_5(C1,C2,D2,E2,E1)         ,_5(B1,B2,C2,D2,D1)         ,_5(A1,A2,B2,C2,C1)         ,_3(B1,B2,A2),
_5(H1,G1,G2,G3,H3),_8(F1,F2,F3,G1,G3,H1,H2,H3),_8(E1,E2,E3,F1,F3,G1,G2,G3),_8(D1,D2,D3,E1,E3,F1,F2,F3),_8(C1,C2,C3,D1,D3,E1,E2,E3),_8(B1,B2,B3,C1,C3,D1,D2,D3),_8(A1,A2,A3,B1,B3,C1,C2,C3),_5(A1,B1,B2,B3,A3),
_5(H2,G2,G3,G4,H4),_8(F2,F3,F4,G2,G4,H2,H3,H4),_8(E2,E3,E4,F2,F4,G2,G3,G4),_8(D2,D3,D4,E2,E4,F2,F3,F4),_8(C2,C3,C4,D2,D4,E2,E3,E4),_8(B2,B3,B4,C2,C4,D2,D3,D4),_8(A2,A3,A4,B2,B4,C2,C3,C4),_5(A2,B2,B3,B4,A4),
_5(H3,G3,G4,G5,H5),_8(F3,F4,F5,G3,G5,H3,H4,H5),_8(E3,E4,E5,F3,F5,G3,G4,G5),_8(D3,D4,D5,E3,E5,F3,F4,F5),_8(C3,C4,C5,D3,D5,E3,E4,E5),_8(B3,B4,B5,C3,C5,D3,D4,D5),_8(A3,A4,A5,B3,B5,C3,C4,C5),_5(A3,B3,B4,B5,A5),
_5(H4,G4,G5,G6,H6),_8(F4,F5,F6,G4,G6,H4,H5,H6),_8(E4,E5,E6,F4,F6,G4,G5,G6),_8(D4,D5,D6,E4,E6,F4,F5,F6),_8(C4,C5,C6,D4,D6,E4,E5,E6),_8(B4,B5,B6,C4,C6,D4,D5,D6),_8(A4,A5,A6,B4,B6,C4,C5,C6),_5(A4,B4,B5,B6,A6),
_5(H5,G5,G6,G7,H7),_8(F5,F6,F7,G5,G7,H5,H6,H7),_8(E5,E6,E7,F5,F7,G5,G6,G7),_8(D5,D6,D7,E5,E7,F5,F6,F7),_8(C5,C6,C7,D5,D7,E5,E6,E7),_8(B5,B6,B7,C5,C7,D5,D6,D7),_8(A5,A6,A7,B5,B7,C5,C6,C7),_5(A5,B5,B6,B7,A7),
_5(H6,G6,G7,G8,H8),_8(F6,F7,F8,G6,G8,H6,H7,H8),_8(E6,E7,E8,F6,F8,G6,G7,G8),_8(D6,D7,D8,E6,E8,F6,F7,F8),_8(C6,C7,C8,D6,D8,E6,E7,E8),_8(B6,B7,B8,C6,C8,D6,D7,D8),_8(A6,A7,A8,B6,B8,C6,C7,C8),_5(A6,B6,B7,B8,A8),
_3(G8,G7,H7)      ,_5(F8,F7,G7,H8,H7)         ,_5(E8,E7,F7,G8,G7)         ,_5(D8,D7,E7,F8,F7)         ,_5(C8,C7,D7,E8,E7)         ,_5(B8,B7,C7,D8,D7)         ,_5(A8,A7,B7,C8,C7)         ,_3(B8,B7,A7)
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

        // King
        int kingSquare;
        for (int i = 0; i < 64;i++) {
            if (getBit(board.king_W, i)) {
                kingSquare = i;
                break;
            }
        }
        Bitboard tmp = board.king_W;
        tmp |= board.king_W << 1;
        tmp |= board.king_W >> 1;
        tmp |= board.king_W;

        Bitboard moves = 0;
        moves |= tmp; 
        moves |= tmp << 8; 
        moves |= tmp >> 8; 



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