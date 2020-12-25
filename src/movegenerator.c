#include <stdlib.h>
#include <stdio.h>
#include "movegenerator.h"
#include "utils.h"

#define bit(a) (1LL<<a)
#define _3(a,b,c)           (1LL<<a)|(1LL<<b)|(1LL<<c)
#define _4(a,b,c,d)         (1LL<<a)|(1LL<<b)|(1LL<<c)|(1LL<<d)
#define _5(a,b,c,d,e)       (1LL<<a)|(1LL<<b)|(1LL<<c)|(1LL<<d)|(1LL<<e)
#define _6(a,b,c,d,e,f)     (1LL<<a)|(1LL<<b)|(1LL<<c)|(1LL<<d)|(1LL<<e)|(1LL<<f)
#define _7(a,b,c,d,e,f,g)   (1LL<<a)|(1LL<<b)|(1LL<<c)|(1LL<<d)|(1LL<<e)|(1LL<<f)|(1LL<<g)
#define _8(a,b,c,d,e,f,g,h) (1LL<<a)|(1LL<<b)|(1LL<<c)|(1LL<<d)|(1LL<<e)|(1LL<<f)|(1LL<<g)|(1LL<<h)
#define pop_bit(bitboard, square) (get_bit(bitboard, square) ? (bitboard ^= (1ULL << square)) : 0)
#define get_bit(bitboard, square) (bitboard & (1ULL << square))
#define set_bit(bitboard, square) (bitboard |= (1ULL << square))


Bitboard PAWN_START_WHITE = 0xFF00;
Bitboard PAWN_START_BLACK = 0x00FF000000000000;

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

Bitboard KNIGHT_MOVEMENT[64];

int ROOK_RELEVANT_BITS[64] = {
    12, 11, 11, 11, 11, 11, 11, 12,
    11, 10, 10, 10, 10, 10, 10, 11,
    11, 10, 10, 10, 10, 10, 10, 11,
    11, 10, 10, 10, 10, 10, 10, 11,
    11, 10, 10, 10, 10, 10, 10, 11,
    11, 10, 10, 10, 10, 10, 10, 11,
    11, 10, 10, 10, 10, 10, 10, 11,
    12, 11, 11, 11, 11, 11, 11, 12
};

// bishop rellevant occupancy bits
int BISHOP_RELEVANT_BITS[64] = {
    6, 5, 5, 5, 5, 5, 5, 6,
    5, 5, 5, 5, 5, 5, 5, 5,
    5, 5, 7, 7, 7, 7, 5, 5,
    5, 5, 7, 9, 9, 7, 5, 5,
    5, 5, 7, 9, 9, 7, 5, 5,
    5, 5, 7, 7, 7, 7, 5, 5,
    5, 5, 5, 5, 5, 5, 5, 5,
    6, 5, 5, 5, 5, 5, 5, 6
};


// Movement masks
Bitboard BISHOP_MOVEMENT[64];
Bitboard ROOK_MOVEMENT[64];
// All the different attack masks combinations (decided by blockers) for each square
Bitboard BISHOP_ATTACKS[64][512];
Bitboard ROOK_ATTACKS[64][4096];

int countBits(Bitboard bitboard) {
  int count = 0;
  
  while (bitboard) {
      count++;
      bitboard &= bitboard - 1;
  }
  
  return count;
}

int getLs1bIndex(Bitboard bitboard) {

    if (bitboard != 0)
        // Convert trailing zeros before LS1B to ones and count them
        return countBits((bitboard & -bitboard) - 1);
    
    return -1;
}

Bitboard genOccupancyMask(int index, int bits_in_mask, Bitboard attack_mask) {
    Bitboard occupancy = 0ULL;
    
    // loop over the range of bits within attack mask
    for (int i = 0; i < bits_in_mask; i++) {
        int square = getLs1bIndex(attack_mask);
        pop_bit(attack_mask, square);
        
        if (index & (1 << i)) {
            occupancy |= (1ULL << square);
        }
    }
    
    return occupancy;
}

Bitboard genBishopMovement(int square) {
    Bitboard movement = 0ULL;
    
    // File and rank
    int f, r;
    
    // Target file and rank
    int tf = square % 8;
    int tr = square / 8;
    
    for (r = tr + 1, f = tf + 1; r <= 6 && f <= 6; r++, f++) movement |= (1ULL << (r * 8 + f));
    for (r = tr + 1, f = tf - 1; r <= 6 && f >= 1; r++, f--) movement |= (1ULL << (r * 8 + f));
    for (r = tr - 1, f = tf + 1; r >= 1 && f <= 6; r--, f++) movement |= (1ULL << (r * 8 + f));
    for (r = tr - 1, f = tf - 1; r >= 1 && f >= 1; r--, f--) movement |= (1ULL << (r * 8 + f));
    
    return movement;
}

Bitboard genRookMovement(int square) {
    Bitboard movement = 0ULL;
    
    // File and rank
    int f, r;
    
    // Target file and rank
    int tf = square % 8;
    int tr = square / 8;
    
    // generate attacks
    for (r = tr + 1; r <= 6; r++) movement |= (1ULL << (r * 8 + tf));
    for (r = tr - 1; r >= 1; r--) movement |= (1ULL << (r * 8 + tf));
    for (f = tf + 1; f <= 6; f++) movement |= (1ULL << (tr * 8 + f));
    for (f = tf - 1; f >= 1; f--) movement |= (1ULL << (tr * 8 + f));
    
    return movement;
}

Bitboard bishopAttacksOnTheFly(int square, Bitboard block) {
    Bitboard attacks = 0ULL;
    
    // init files & ranks
    int f, r;
    // init target files & ranks
    int tr = square / 8;
    int tf = square % 8;
    
    for (r = tr + 1, f = tf + 1; r <= 7 && f <= 7; r++, f++) {
        attacks |= (1ULL << (r * 8 + f));
        if (block & (1ULL << (r * 8 + f))) break;
    }
    
    for (r = tr + 1, f = tf - 1; r <= 7 && f >= 0; r++, f--) {
        attacks |= (1ULL << (r * 8 + f));
        if (block & (1ULL << (r * 8 + f))) break;
    }
    
    for (r = tr - 1, f = tf + 1; r >= 0 && f <= 7; r--, f++) {
        attacks |= (1ULL << (r * 8 + f));
        if (block & (1ULL << (r * 8 + f))) break;
    }
    
    for (r = tr - 1, f = tf - 1; r >= 0 && f >= 0; r--, f--) {
        attacks |= (1ULL << (r * 8 + f));
        if (block & (1ULL << (r * 8 + f))) break;
    }
    
    return attacks;
}

Bitboard rookAttacksOnTheFly(int square, Bitboard block) {
    Bitboard attacks = 0ULL;
    
    // init files & ranks
    int f, r;
    // init target files & ranks
    int tr = square / 8;
    int tf = square % 8;
    
    for (r = tr + 1; r <= 7; r++) {
        attacks |= (1ULL << (r * 8 + tf));
        if (block & (1ULL << (r * 8 + tf))) break;
    }
    
    for (r = tr - 1; r >= 0; r--) {
        attacks |= (1ULL << (r * 8 + tf));
        if (block & (1ULL << (r * 8 + tf))) break;
    }
    
    for (f = tf + 1; f <= 7; f++) {
        attacks |= (1ULL << (tr * 8 + f));
        if (block & (1ULL << (tr * 8 + f))) break;
    }
    
    for (f = tf - 1; f >= 0; f--) {
        attacks |= (1ULL << (tr * 8 + f));
        if (block & (1ULL << (tr * 8 + f))) break;
    }
    
    return attacks;
}


void initKnightMovementTable(void) {
    for (int i = 0; i < 64; i++) {
        Bitboard sqBb = 1LL << (63-i);
        Bitboard bb = 0LL;
        int file = i%8;

        if (file != H && i < H7) { bb |= sqBb >> 15;} // UP RIGHT
        if (file != A && i < H7) { bb |= sqBb >> 17;} // UP LEFT
        if (file != H && i > A2) { bb |= sqBb << 17;} // DOWN RIGHT
        if (file != A && i > A2) { bb |= sqBb << 15;} // DOWN RIGHT
        if (file < B && i < A7) { bb |= sqBb >> 10;} // LEFT UP
        if (file < B && i > H2) { bb |= sqBb << 6;} // LEFT DOWN
        if (file > G && i < H8) { bb |= sqBb >> 6;} // RIGHT UP
        if (file > G && i > A1) { bb |= sqBb << 10;} // RIGHT DOWN

        KNIGHT_MOVEMENT[63-i] = bb;
    }
}

void initMoveGenerationTables(void) {
    initKnightMovementTable();
    for (int square = 0; square < 64; square++) {

        // Fill movement masks
        BISHOP_MOVEMENT[square] = genBishopMovement(square);
        ROOK_MOVEMENT[square] = genRookMovement(square);
        
        Bitboard bishopMask = BISHOP_MOVEMENT[square];
        Bitboard rookMask = ROOK_MOVEMENT[square];
        int bishopRelevantBits = BISHOP_RELEVANT_BITS[square];
        int rookRelevantBits = ROOK_RELEVANT_BITS[square];
        int bishopOccupancyVariations = 1 << bishopRelevantBits;
        int rookOccupancyVariations = 1 << rookRelevantBits;

        for (int i = 0; i < bishopOccupancyVariations; i++) {
            Bitboard occupancy = genOccupancyMask(i, bishopRelevantBits, bishopMask);
            Bitboard magic_index = occupancy * BISHOP_MAGICS[square] >> 64 - bishopRelevantBits;
            BISHOP_ATTACKS[square][magic_index] = bishopAttacksOnTheFly(square, occupancy);                
        }

        for (int i = 0; i < rookOccupancyVariations; i++) {
            Bitboard occupancy = genOccupancyMask(i, rookRelevantBits, rookMask);
            Bitboard magic_index = occupancy * ROOK_MAGICS[square] >> 64 - rookRelevantBits;
            ROOK_ATTACKS[square][magic_index] = rookAttacksOnTheFly(square, occupancy);                
        }
    }
}

Bitboard getBishopAttacks(int square, Bitboard occupancy) {
	occupancy &= BISHOP_MOVEMENT[square];
	occupancy *=  BISHOP_MAGICS[square];
	occupancy >>= 64 - BISHOP_RELEVANT_BITS[square];
	return BISHOP_ATTACKS[square][occupancy];
}

Bitboard getRookAttacks(int square, Bitboard occupancy) {
	occupancy &= ROOK_MOVEMENT[square];
	occupancy *=  ROOK_MAGICS[square];
	occupancy >>= 64 - ROOK_RELEVANT_BITS[square];
	return ROOK_ATTACKS[square][occupancy];
}

int legalMoves(Board board, Move moves[]) {
    Bitboard occupancyWhite = 0;
    Bitboard occupancyBlack = 0;
    for (int i = 0; i < 6;i++) {
        occupancyWhite |= *(&(board.pawn_W)+i);
        occupancyBlack |= *(&(board.pawn_B)+i);
    }
    Bitboard occupancy = occupancyBlack | occupancyWhite;
    int length = 0;
    if (board.turn) {
        // Pawn pushes
        Bitboard singlePush = board.pawn_W << 8;
        singlePush ^= singlePush & occupancy;

        Bitboard doublePush = (board.pawn_W & PAWN_START_WHITE) << 8*2;
        doublePush ^= doublePush & occupancy;
        doublePush = doublePush >> 8;
        doublePush &= singlePush;
        doublePush = doublePush << 8;

        for (int i = 0; i < 64;i++) {
            if (getBit(singlePush, i)) {
                Move move;
                move.fromSquare = i-8;
                move.toSquare = i;
                moves[length] = move;
                length++;
            }
            if (getBit(doublePush, i)) {
                Move move;
                move.fromSquare = i-8*2;
                move.toSquare = i;
                moves[length] = move;
                length++;
            }
        }

        // Pawn attacks
        for (int i = 0; i < 64;i++) {
            if (getBit(board.pawn_W, i)) {
                if (PAWN_W_ATTACKS_EAST[i] & occupancyBlack) {
                    Move move;
                    move.fromSquare = i;
                    move.toSquare = i+7;
                    moves[length] = move;
                    length++;
                }
                
                if (PAWN_W_ATTACKS_WEST[i] & occupancyBlack) {
                    Move move;
                    move.fromSquare = i;
                    move.toSquare = i+9;
                    moves[length] = move;
                    length++;
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
        Bitboard kingMoves = KING_MOVEMENT[kingSquare];
        kingMoves ^= kingMoves & occupancyWhite;
        for (int i = 0; i < 64;i++) {
            if (getBit(kingMoves, i)) {
                Move move;
                move.fromSquare = kingSquare;
                move.toSquare = i;
                moves[length] = move;
                length++;
            }
        }

        // Bishop
        for (int i = 0; i < 64; i++) {
            if (getBit(board.bishop_W, i)) {
                Bitboard bishopAttacks = getBishopAttacks(i, occupancy);
                bishopAttacks ^= bishopAttacks & occupancyWhite;
                for (int j = 0; j < 64; j++) {
                    if (getBit(bishopAttacks, j)) {
                        Move move;
                        move.fromSquare = i;
                        move.toSquare = j;
                        moves[length] = move;
                        length++;
                    }
                }
            }
        }
        // Rook
        for (int i = 0; i < 64; i++) {
            if (getBit(board.rook_W, i)) {
                Bitboard rookAttacks = getRookAttacks(i, occupancy);
                rookAttacks ^= rookAttacks & occupancyWhite;
                for (int j = 0; j < 64; j++) {
                    if (getBit(rookAttacks, j)) {
                        Move move;
                        move.fromSquare = i;
                        move.toSquare = j;
                        moves[length] = move;
                        length++;
                    }
                }
            }
        }

        // Queen
        for (int i = 0; i < 64; i++) {
            if (getBit(board.queen_W, i)) {
                Bitboard rookAttacks = getRookAttacks(i, occupancy);
                rookAttacks ^= rookAttacks & occupancyWhite;

                Bitboard bishopAttacks = getBishopAttacks(i, occupancy);
                bishopAttacks ^= bishopAttacks & occupancyWhite;
                Bitboard queenAttacks = bishopAttacks | rookAttacks;

                for (int j = 0; j < 64; j++) {
                    if (getBit(queenAttacks, j)) {
                        Move move;
                        move.fromSquare = i;
                        move.toSquare = j;
                        moves[length] = move;
                        length++;
                    }
                }
            }
        }

        // Knight
        for (int i = 0; i < 64; i++) {
            if (getBit(board.knight_W, i)) {
                Bitboard attacks = KNIGHT_MOVEMENT[i] ^ (KNIGHT_MOVEMENT[i] & occupancyWhite);

                for (int j = 0; j < 64; j++) {
                    if (getBit(attacks, j)) {
                        Move move;
                        move.fromSquare = i;
                        move.toSquare = j;
                        moves[length] = move;
                        length++;
                    }
                }
            }
        }
    } else {
        // Pawn pushes
        Bitboard singlePush = board.pawn_B >> 8;
        singlePush ^= singlePush & occupancy;

        Bitboard doublePush = (board.pawn_B & PAWN_START_BLACK) >> 8*2;
        doublePush ^= doublePush & occupancy;
        doublePush = doublePush << 8;
        doublePush &= singlePush;
        doublePush = doublePush >> 8;

        for (int i = 0; i < 64;i++) {
            if (getBit(singlePush, i)) {
                Move move;
                move.fromSquare = i+8;
                move.toSquare = i;
                moves[length] = move;
                length++;
            }
            if (getBit(doublePush, i)) {
                Move move;
                move.fromSquare = i+8*2;
                move.toSquare = i;
                moves[length] = move;
                length++;
            }
        }

        // Pawn attacks
        for (int i = 0; i < 64;i++) {
            if (getBit(board.pawn_B, i)) {
                if (PAWN_B_ATTACKS_EAST[i] & occupancyWhite) {
                    Move move;
                    move.fromSquare = i;
                    move.toSquare = i-7;
                    moves[length] = move;
                    length++;
                }
                
                if (PAWN_B_ATTACKS_WEST[i] & occupancyWhite) {
                    Move move;
                    move.fromSquare = i;
                    move.toSquare = i-9;
                    moves[length] = move;
                    length++;
                }
            }
        }

        // King
        int kingSquare;
        for (int i = 0; i < 64;i++) {
            if (getBit(board.king_B, i)) {
                kingSquare = i;
                break;
            }
        }
        Bitboard kingMoves = KING_MOVEMENT[kingSquare];
        kingMoves ^= kingMoves & occupancyBlack;
        for (int i = 0; i < 64;i++) {
            if (getBit(kingMoves, i)) {
                Move move;
                move.fromSquare = kingSquare;
                move.toSquare = i;
                moves[length] = move;
                length++;
            }
        }

        // Bishop
        for (int i = 0; i < 64; i++) {
            if (getBit(board.bishop_B, i)) {
                Bitboard bishopAttacks = getBishopAttacks(i, occupancy);
                bishopAttacks ^= bishopAttacks & occupancyBlack;
                for (int j = 0; j < 64; j++) {
                    if (getBit(bishopAttacks, j)) {
                        Move move;
                        move.fromSquare = i;
                        move.toSquare = j;
                        moves[length] = move;
                        length++;
                    }
                }
            }
        }
        // Rook
        for (int i = 0; i < 64; i++) {
            if (getBit(board.rook_B, i)) {
                Bitboard rookAttacks = getRookAttacks(i, occupancy);
                rookAttacks ^= rookAttacks & occupancyBlack;
                for (int j = 0; j < 64; j++) {
                    if (getBit(rookAttacks, j)) {
                        Move move;
                        move.fromSquare = i;
                        move.toSquare = j;
                        moves[length] = move;
                        length++;
                    }
                }
            }
        }

        // Queen
        for (int i = 0; i < 64; i++) {
            if (getBit(board.queen_B, i)) {
                Bitboard rookAttacks = getRookAttacks(i, occupancy);
                rookAttacks ^= rookAttacks & occupancyBlack;

                Bitboard bishopAttacks = getBishopAttacks(i, occupancy);
                bishopAttacks ^= bishopAttacks & occupancyBlack;
                Bitboard queenAttacks = bishopAttacks | rookAttacks;

                for (int j = 0; j < 64; j++) {
                    if (getBit(queenAttacks, j)) {
                        Move move;
                        move.fromSquare = i;
                        move.toSquare = j;
                        moves[length] = move;
                        length++;
                    }
                }
            }
        }

        // Knight
        for (int i = 0; i < 64; i++) {
            if (getBit(board.knight_B, i)) {
                Bitboard attacks = KNIGHT_MOVEMENT[i] ^ (KNIGHT_MOVEMENT[i] & occupancyBlack);

                for (int j = 0; j < 64; j++) {
                    if (getBit(attacks, j)) {
                        Move move;
                        move.fromSquare = i;
                        move.toSquare = j;
                        moves[length] = move;
                        length++;
                    }
                }
            }
        }
    }

    return length;
}

void sanToMove(Move* move, char* san) {
    int fromFile = 'h' - san[0];
    int fromRank = atoi(&san[1]) - 1;
    int toFile = 'h' - san[2];
    int toRank = atoi(&san[3]) - 1;
    move->fromSquare = 8 * (fromRank) + fromFile;
    move->toSquare = 8 * (toRank) + toFile;
}

void moveToSan(Move move, char san[]) {
    san[0] = SQUARE_NAMES[move.fromSquare][0];
    san[1] = SQUARE_NAMES[move.fromSquare][1];
    san[2] = SQUARE_NAMES[move.toSquare][0];
    san[3] = SQUARE_NAMES[move.toSquare][1];
}