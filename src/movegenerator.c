#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include "movegenerator.h"
#include "utils.h"
#include "board.h"

#define bit(a) (1LL<<a)
#define pop_bit(bitboard, square) (get_bit(bitboard, square) ? (bitboard ^= (1ULL << square)) : 0)
#define get_bit(bitboard, square) (bitboard & (1ULL << square))
#define set_bit(bitboard, square) (bitboard |= (1ULL << square))
#define getMove(from, to, promo, castling) {.fromSquare=from, .toSquare=to, .promotion=promo, .castle=castling}

Bitboard PAWN_START_WHITE = 0xFF00;
Bitboard PAWN_START_BLACK = 0x00FF000000000000;
int WHITE_PROMOTIONS[4] = {QUEEN_W, BISHOP_W, KNIGHT_W, ROOK_W};
int BLACK_PROMOTIONS[4] = {QUEEN_B, BISHOP_B, KNIGHT_B, ROOK_B};
int NO_PROMOTION = -1;
int NOT_CASTLE = 0;

Bitboard PAWN_W_ATTACKS_EAST[64];
Bitboard PAWN_W_ATTACKS_WEST[64];
Bitboard PAWN_B_ATTACKS_EAST[64];
Bitboard PAWN_B_ATTACKS_WEST[64];
Bitboard KNIGHT_MOVEMENT[64];
Bitboard BISHOP_MOVEMENT[64];
Bitboard ROOK_MOVEMENT[64];
Bitboard KING_MOVEMENT[64];
Bitboard BISHOP_ATTACKS[64][512];
Bitboard ROOK_ATTACKS[64][4096];

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

void initKingMovementTable(void) {
    for (int sq = 0; sq < 64; sq++) {
        Bitboard moves = 0;
        int file = sq%8;

        if (sq <= A7) moves |= 1LL << (sq+8); // UP
        if (sq >= H2) moves |= 1LL << (sq-8); // DOWN
        if (file < A) moves |= 1LL << (sq+1); // LEFT
        if (file > H) moves |= 1LL << (sq-1); // RIGHT
        if (file < A && sq <= A7) moves |= 1LL << (sq+9); // UP LEFT
        if (file > H && sq <= A7) moves |= 1LL << (sq+7); // UP RIGHT
        if (file < A && sq >= H2) moves |= 1LL << (sq-7); // DOWN LEFT
        if (file > H && sq >= H2) moves |= 1LL << (sq-9); // DOWN RIGHT

        KING_MOVEMENT[sq] = moves;
    }
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

void initPawnAttackTables(void) {
    for (int sq = 0; sq < 64;sq++) {
        int file = sq%8;

        Bitboard attack = 0;
        if (file != A && sq <= H8) attack |= 1LL << (sq+9); // White west
        PAWN_W_ATTACKS_WEST[sq] = attack;

        attack = 0;
        if (file != H && sq <= H8) attack |= 1LL << (sq+7); // White east
        PAWN_W_ATTACKS_EAST[sq] = attack;

        attack = 0;
        if (file != H && sq >= H2) attack |= 1LL << (sq-9); // Black west
        PAWN_B_ATTACKS_WEST[sq] = attack;

        attack = 0;
        if (file != A && sq >= H2) attack |= 1LL << (sq-7); // Black east
        PAWN_B_ATTACKS_EAST[sq] = attack;
    }
}

void initBishopRookAttackTables() {
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

void initMoveGenerationTables(void) {
    initKnightMovementTable();
    initKingMovementTable();
    initPawnAttackTables();
    initBishopRookAttackTables();
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


bool isSquareAttacked(Board board, int square) {
    Bitboard occ = 0;
    for (int i = 0; i < 12; i++) occ |= *(&(board.pawn_W)+i);
    Bitboard sqBb = 1LL << square;

    for (int i = 0; i < 64; i++) {
        // Pawn
        Bitboard pawn = board.turn ? board.pawn_B : board.pawn_W;
        if (getBit(pawn, i)) {
            if (board.turn) {
                if (PAWN_B_ATTACKS_EAST[i] & sqBb) return true;
                if (PAWN_B_ATTACKS_WEST[i] & sqBb) return true;
            } else {
                if (PAWN_W_ATTACKS_EAST[i] & sqBb) return true;
                if (PAWN_W_ATTACKS_WEST[i] & sqBb) return true;
            }
        }

        // King
        Bitboard king = board.turn ? board.king_B : board.king_W;
        if (getBit(king, i)) {
            if (KING_MOVEMENT[i] & sqBb) return true;
        }
        // Knight
        Bitboard knight = board.turn ? board.knight_B : board.knight_W;
        if (getBit(knight, i)) {
            if (KNIGHT_MOVEMENT[i] & sqBb) return true;
        }
        // Bishop
        Bitboard bishop = board.turn ? board.bishop_B : board.bishop_W;
        if (getBit(bishop, i)) {
            Bitboard attacks = getBishopAttacks(i, occ);
            if (attacks & sqBb) return true;
        }
        // Rook
        Bitboard rook = board.turn ? board.rook_B : board.rook_W;
        if (getBit(rook, i)) {
            Bitboard attacks = getRookAttacks(i, occ);
            if (attacks & sqBb) return true;
        }
        // Queen
        Bitboard queen = board.turn ? board.queen_B : board.queen_W;
        if (getBit(queen, i)) {
            Bitboard attacks = getBishopAttacks(i, occ);
            attacks |= getRookAttacks(i, occ);
            if (attacks & sqBb) return true;
        }
    }

    return false;
}

bool isMoveLegal(Board board, Move move) {
    Board cpy = board;
    pushMove(&cpy, move);

    Bitboard kingMask = cpy.turn ? cpy.king_B : cpy.king_W;
    int kingSquare;
    for (int i = 0; i < 64;i++) {
        if (getBit(kingMask, i)) {
            kingSquare = i;
            break;
        }
    }
    cpy.turn ^= 1;

    return ! isSquareAttacked(cpy, kingSquare);
}

void addMove(Board board, Move move, Move moves[], int* indx) {
    if (isMoveLegal(board, move)) {
        moves[*indx] = move;
        *indx += 1;
    }
}

Bitboard getKingMask(Board board, Bitboard occWhite, Bitboard occBlack, int* kingSquare) {
    Bitboard kingPosMask = board.turn ? board.king_W : board.king_B;

    for (int i = 0; i < 64;i++) {
        if (getBit(kingPosMask, i)) {
            *kingSquare = i;
            break;
        }
    }

    Bitboard kingMoves = KING_MOVEMENT[*kingSquare];
    Bitboard occ = board.turn ? occWhite : occBlack;
    return kingMoves ^ (kingMoves & occ);
}

void pawnSingleAndDblPushes(Board board, Bitboard occ, Bitboard* singlePush, Bitboard* doublePush) {
    Bitboard single;
    Bitboard dbl;

    if (board.turn) {
        single = board.pawn_W << 8;
    } else {
        single = board.pawn_B >> 8;
    }
    single ^= single & occ;

    if (board.turn) {
        dbl = (board.pawn_W & PAWN_START_WHITE) << 16;
    } else {
        dbl = (board.pawn_B & PAWN_START_BLACK) >> 16;
    }
    dbl ^= dbl & occ;
    dbl = board.turn ? dbl >> 8 : dbl << 8;
    dbl &= single;
    dbl = board.turn ? dbl << 8 : dbl >> 8;

    *singlePush = single;
    *doublePush = dbl;
}

void addPawnAdvanceWithPossiblePromos(Board board, bool isPromoting, int turn, int from, int to, Move moves[], int* indx) {
    if (isPromoting) {
        for (int i = 0; i < 4; i++) {
            Move move = getMove(from, to, turn ? WHITE_PROMOTIONS[i] : BLACK_PROMOTIONS[i], NOT_CASTLE);
            addMove(board, move, moves, indx);
        }
    } else {
        Move move = getMove(from, to, NO_PROMOTION, NOT_CASTLE);
        addMove(board, move, moves, indx);
    }
}

int legalMoves(Board board, Move moves[]) {

    // Occupancy calculations
    Bitboard occupancyWhite = 0;
    Bitboard occupancyBlack = 0;
    for (int i = 0; i < 6;i++) {
        occupancyWhite |= *(&(board.pawn_W)+i);
        occupancyBlack |= *(&(board.pawn_B)+i);
    }
    Bitboard occupancy = occupancyBlack | occupancyWhite;
    Bitboard friendlyOccupancy = board.turn ? occupancyWhite : occupancyBlack;
    int length = 0;

    // King moves
    int kingSquare;
    Bitboard kingMovesMask = getKingMask(board, occupancyWhite, occupancyBlack, &kingSquare);

    // Pawn single and double pushes
    Bitboard singlePush;
    Bitboard doublePush;
    pawnSingleAndDblPushes(board, occupancy, &singlePush, &doublePush);

    Bitboard bishopBitboard = board.turn ? board.bishop_W : board.bishop_B;
    Bitboard rookBitboard = board.turn ? board.rook_W : board.rook_B;
    Bitboard queenBitboard = board.turn ? board.queen_W : board.queen_B;
    Bitboard knightBitboard = board.turn ? board.knight_W : board.knight_B;
    Bitboard pawnMask = board.turn ? board.pawn_W : board.pawn_B;
    Bitboard epSquare = board.epSquare == -1 ? 0LL : 1LL << board.epSquare;

    // Iterate over squares
    for (int sq = 0; sq < 64; sq++) {

        if (getBit(singlePush, sq)) {
            int fromSquare = board.turn ? sq-8 : sq+8;
            bool isPromoting = board.turn ? (fromSquare >= H7 && fromSquare <= A7) : (fromSquare >= H2 && fromSquare <= A2);

            // Pawn single push
            addPawnAdvanceWithPossiblePromos(board, isPromoting, board.turn, fromSquare, sq, moves, &length);
        }

        if (getBit(pawnMask, sq)) {
            bool isPromoting = board.turn ? (sq >= H7 && sq <= A7) : (sq >= H2 && sq <= A2);
            if (board.turn) {
                // Pawn east attack
                if (PAWN_W_ATTACKS_EAST[sq] & (occupancyBlack | epSquare)) {
                    int toSquare = sq + 7;
                    addPawnAdvanceWithPossiblePromos(board, isPromoting, board.turn, sq, toSquare, moves, &length);
                }
                
                // Pawn west attack
                if (PAWN_W_ATTACKS_WEST[sq] & (occupancyBlack | epSquare)) {
                    int toSquare = sq + 9;
                    addPawnAdvanceWithPossiblePromos(board, isPromoting, board.turn, sq, toSquare, moves, &length);
                }
            } else {
                // Pawn east attack
                if (PAWN_B_ATTACKS_EAST[sq] & (occupancyWhite | epSquare)) {
                    int toSquare = sq - 7;
                    addPawnAdvanceWithPossiblePromos(board, isPromoting, board.turn, sq, toSquare, moves, &length);
                }
                
                // Pawn west attack
                if (PAWN_B_ATTACKS_WEST[sq] & (occupancyWhite | epSquare)) {
                    int toSquare = sq - 9;
                    addPawnAdvanceWithPossiblePromos(board, isPromoting, board.turn, sq, toSquare, moves, &length);
                }
            }
        }

        // Pawn double pushes
        if (getBit(doublePush, sq)) {
            int fromSquare = board.turn ? sq-8*2 : sq+8*2;
            Move move = getMove(fromSquare, sq, NO_PROMOTION, NOT_CASTLE);
            addMove(board, move, moves, &length);
        }

        // King
        if (getBit(kingMovesMask, sq)) {
            Move move = getMove(kingSquare, sq, NO_PROMOTION, NOT_CASTLE);
            isMoveLegal(board, move);
            addMove(board, move, moves, &length);
        }

        // Bishop
        if (getBit(bishopBitboard, sq)) {
            Bitboard target = getBishopAttacks(sq, occupancy);
            target ^= target & friendlyOccupancy;

            for (int i = 0; i < 64; i++) {
                if (getBit(target, i)) {
                    Move move = getMove(sq, i, NO_PROMOTION, NOT_CASTLE);
                    addMove(board, move, moves, &length);
                }
            }
        }

        // Rook
        if (getBit(rookBitboard, sq)) {
            Bitboard target = getRookAttacks(sq, occupancy);
            target ^= target & friendlyOccupancy;

            for (int i = 0; i < 64; i++) {
                if (getBit(target, i)) {
                    Move move = getMove(sq, i, NO_PROMOTION, NOT_CASTLE);
                    addMove(board, move, moves, &length);
                }
            }
        }

        // Queen
        if (getBit(queenBitboard, sq)) {
            Bitboard rookAttacks = getRookAttacks(sq, occupancy);
            rookAttacks ^= rookAttacks & friendlyOccupancy;

            Bitboard bishopAttacks = getBishopAttacks(sq, occupancy);
            bishopAttacks ^= bishopAttacks & friendlyOccupancy;

            Bitboard target = bishopAttacks | rookAttacks;

            for (int i = 0; i < 64; i++) {
                if (getBit(target, i)) {
                    Move move = getMove(sq, i, NO_PROMOTION, NOT_CASTLE);
                    addMove(board, move, moves, &length);
                }
            }
        }

        // Knight
        if (getBit(knightBitboard, sq)) {
            Bitboard target = KNIGHT_MOVEMENT[sq] ^ (KNIGHT_MOVEMENT[sq] & friendlyOccupancy);
                for (int i = 0; i < 64; i++) {
                    if (getBit(target, i)) {
                        Move move = getMove(sq, i, NO_PROMOTION, NOT_CASTLE);
                        addMove(board, move, moves, &length);
                    }
                }
        }
    }

    // Castling
    if (board.turn) {
        if (board.castling & K) {
            bool isNotInCheck = ! isSquareAttacked(board, E1);
            bool pathClear = getBit(occupancy, F1) == 0 && getBit(occupancy, G1) == 0;
            bool noAttacks = ! isSquareAttacked(board, F1);
            pathClear = pathClear && noAttacks;

            if (isNotInCheck && pathClear) {
                Move move = getMove(0, 0, NO_PROMOTION, K);
                addMove(board, move, moves, &length);
            }
        }
        if (board.castling & Q) {
            bool isNotInCheck = ! isSquareAttacked(board, E1);
            bool pathClear = getBit(occupancy, B1) == 0 && getBit(occupancy, C1) == 0 && getBit(occupancy, D1) == 0;
            bool noAttacks = ! isSquareAttacked(board, D1);
            pathClear = pathClear && noAttacks;

            if (isNotInCheck && pathClear) {
                Move move = getMove(0, 0, NO_PROMOTION, Q);
                addMove(board, move, moves, &length);
            }
        }
    } else {
        if (board.castling & k) {
            bool isNotInCheck = ! isSquareAttacked(board, E8);
            bool pathClear = getBit(occupancy, F8) == 0 && getBit(occupancy, G8) == 0;
            bool noAttacks = ! isSquareAttacked(board, F8);
            pathClear = pathClear && noAttacks;

            if (isNotInCheck && pathClear) {
                Move move = getMove(0, 0, NO_PROMOTION, k);
                addMove(board, move, moves, &length);
            }
        }
        if (board.castling & q) {
            bool isNotInCheck = ! isSquareAttacked(board, E8);
            bool pathClear = getBit(occupancy, B8) == 0 && getBit(occupancy, C8) == 0 && getBit(occupancy, D8) == 0;
            bool noAttacks = ! isSquareAttacked(board, D8);
            pathClear = pathClear && noAttacks;

            if (isNotInCheck && pathClear) {
                Move move = getMove(0, 0, NO_PROMOTION, q);
                addMove(board, move, moves, &length);
            }
        }
    }

    return length;
}

void printMoves(Move moves[], int length) {
    printf("Moves: %d\n", length);
    for(int i = 0;i<length;i++) {
        char san[6];
        moveToSan(moves[i], san);
        printf("%s\n", san);
        memset(san, 0, sizeof(san));
    }
}

void sanToMove(Board board, Move* move, char* san) {
    if (san[0] == 'O' && san[3] != '-') {
        if (board.turn) move->castle = K;
        else            move->castle = k;
    } else if (san[4] == 'O') {
        if (board.turn) move->castle = Q;
        else            move->castle = q;
    } else {

        int fromFile = 'h' - san[0];
        int fromRank = atoi(&san[1]) - 1;
        int toFile = 'h' - san[2];
        int toRank = atoi(&san[3]) - 1;
        move->fromSquare = 8 * (fromRank) + fromFile;
        move->toSquare = 8 * (toRank) + toFile;
        move->castle = 0;
    }
}

void moveToSan(Move move, char san[]) {
    if (move.castle) {
        if (move.castle == K) { san = strcpy(san, "e1g1"); }
        else if (move.castle == k) { san = strcpy(san, "e8g8"); }
        else if (move.castle == Q) { san = strcpy(san, "e1c1"); }
        else if (move.castle == q) { san = strcpy(san, "e8c8"); }

    } else {
        san[0] = SQUARE_NAMES[move.fromSquare][0];
        san[1] = SQUARE_NAMES[move.fromSquare][1];
        san[2] = SQUARE_NAMES[move.toSquare][0];
        san[3] = SQUARE_NAMES[move.toSquare][1];

        switch (move.promotion) {
            case QUEEN_W:
                san[4] = 'Q';
                break;
            case QUEEN_B:
                san[4] = 'q';
                break;
            case ROOK_W:
                san[4] = 'R';
                break;
            case ROOK_B:
                san[4] = 'r';
                break;
            case BISHOP_W:
                san[4] = 'B';
                break;
            case BISHOP_B:
                san[4] = 'b';
                break;
            case KNIGHT_W:
                san[4] = 'N';
                break;
            case KNIGHT_B:
                san[4] = 'n';
                break;
        }
    }
}