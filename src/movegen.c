#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include "movegen.h"
#include "board.h"
#include "utils.h"

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


/*---------------------------------------------
                                             
 Initializes piece movement tables

---------------------------------------------*/

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
    for (int sq = 0; sq < 64; sq++) {
        Bitboard bb = 0LL;
        int file = sq%8;

        if (file != A && sq <= A6) { bb |= 1LL << sq+17;} // UP LEFT
        if (file != H && sq <= A6) { bb |= 1LL << sq+15;} // UP RIGHT
        if (file != A && sq >= A2) { bb |= 1LL << sq-15;} // DOWN LEFT
        if (file != H && sq > H3) { bb |= 1LL << sq-17;} // DOWN RIGHT
        if (file < B && sq <= A7) { bb |= 1LL << sq+10;} // LEFT UP
        if (file < B && sq >= H2) { bb |= 1LL << sq-6;} // LEFT DOWN
        if (file > G && sq >= H2) { bb |= 1LL << sq-10;} // RIGHT DOWN
        if (file > G && sq <= A7) { bb |= 1LL << sq+6;} // RIGHT UP

        KNIGHT_MOVEMENT[sq] = bb;
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

/*---------------------------------------------
                                             
Magic bitboard methods

---------------------------------------------*/

int bitScanForward(Bitboard* bb) {
    Bitboard tmp = *bb & -(*bb);
    int indx = __builtin_ctzl(*bb);
    *bb ^= tmp;

    return indx;
}

int countBits(Bitboard bitboard) {
  int count = 0;
  
  while (bitboard) {
      count++;
      bitboard &= bitboard - 1;
  }
  
  return count;
}

int indexOfLastSetBit(Bitboard bitboard) {
    if (bitboard != 0)
        return countBits((bitboard & -bitboard) - 1);
    
    return -1;
}

Bitboard occupancyMask(int index, int bits, Bitboard attackMask) {
    Bitboard occupancy = 0ULL;
    
    for (int i = 0; i < bits; i++) {
        int square = indexOfLastSetBit(attackMask);
        toggleBit(attackMask, square);
        
        if (index & (1 << i)) {
            occupancy |= (1ULL << square);
        }
    }
    
    return occupancy;
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

Bitboard bishopMovement(int square) {
    // File and rank
    int f, r;
    
    // Target file and rank
    int tf = square % 8;
    int tr = square / 8;
    
    Bitboard movement = 0ULL;
    for (r = tr + 1, f = tf + 1; r <= 6 && f <= 6; r++, f++) movement |= (1ULL << (r * 8 + f));
    for (r = tr + 1, f = tf - 1; r <= 6 && f >= 1; r++, f--) movement |= (1ULL << (r * 8 + f));
    for (r = tr - 1, f = tf + 1; r >= 1 && f <= 6; r--, f++) movement |= (1ULL << (r * 8 + f));
    for (r = tr - 1, f = tf - 1; r >= 1 && f >= 1; r--, f--) movement |= (1ULL << (r * 8 + f));
    
    return movement;
}

Bitboard rookMovement(int square) {
    // File and rank
    int f, r;
    
    // Target file and rank
    int tf = square % 8;
    int tr = square / 8;
    
    Bitboard movement = 0ULL;
    for (r = tr + 1; r <= 6; r++) movement |= (1ULL << (r * 8 + tf));
    for (r = tr - 1; r >= 1; r--) movement |= (1ULL << (r * 8 + tf));
    for (f = tf + 1; f <= 6; f++) movement |= (1ULL << (tr * 8 + f));
    for (f = tf - 1; f >= 1; f--) movement |= (1ULL << (tr * 8 + f));
    
    return movement;
}

void initBishopRookAttackTables() {
    for (int square = 0; square < 64; square++) {

        // Fill movement masks
        BISHOP_MOVEMENT[square] = bishopMovement(square);
        ROOK_MOVEMENT[square] = rookMovement(square);
        
        Bitboard bishopMask = BISHOP_MOVEMENT[square];
        Bitboard rookMask = ROOK_MOVEMENT[square];
        int bishopRelevantBits = BISHOP_RELEVANT_BITS[square];
        int rookRelevantBits = ROOK_RELEVANT_BITS[square];
        int bishopOccupancyVariations = 1 << bishopRelevantBits;
        int rookOccupancyVariations = 1 << rookRelevantBits;

        for (int i = 0; i < bishopOccupancyVariations; i++) {
            Bitboard occupancy = occupancyMask(i, bishopRelevantBits, bishopMask);
            Bitboard magic_index = occupancy * BISHOP_MAGICS[square] >> 64 - bishopRelevantBits;
            BISHOP_ATTACKS[square][magic_index] = bishopAttacksOnTheFly(square, occupancy);                
        }

        for (int i = 0; i < rookOccupancyVariations; i++) {
            Bitboard occupancy = occupancyMask(i, rookRelevantBits, rookMask);
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

bool isMoveLegal(Board board, Move move) {
    Board cpy = board;
    pushMove(&cpy, move);

    int kingSquare = cpy.turn ? cpy.blackKingSq : cpy.whiteKingSq;
    cpy.turn ^= 1;

    return ! isSquareAttacked(cpy, kingSquare);
}

void addMove(Board board, Move move, Move moves[], int* indx) {
    if (isMoveLegal(board, move)) {
        moves[*indx] = move;
        *indx += 1;
    }
}

Bitboard getKingMask(Board board) {
    int kingSquare = board.turn ? board.whiteKingSq : board.blackKingSq;

    Bitboard kingMoves = KING_MOVEMENT[kingSquare];
    Bitboard occ = board.turn ? board.occupancyWhite : board.occupancyBlack;
    return kingMoves ^ (kingMoves & occ);
}

void pawnSingleAndDblPushes(Board board, Bitboard* singlePush, Bitboard* doublePush) {
    Bitboard single;
    Bitboard dbl;

    if (board.turn) {
        single = board.pawn_W << 8;
    } else {
        single = board.pawn_B >> 8;
    }
    single ^= single & board.occupancy;

    if (board.turn) {
        dbl = (board.pawn_W & PAWN_START_WHITE) << 16;
    } else {
        dbl = (board.pawn_B & PAWN_START_BLACK) >> 16;
    }
    dbl ^= dbl & board.occupancy;
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


/*---------------------------------------------
                                             
Visible methods

---------------------------------------------*/

void initMoveGeneration(void) {
    initKnightMovementTable();
    initKingMovementTable();
    initPawnAttackTables();
    initBishopRookAttackTables();
}

bool isSquareAttacked(Board board, int square) {
    Bitboard sqBb = 1LL << square;
    Bitboard pawn = board.turn ? board.pawn_B : board.pawn_W;
    Bitboard king = board.turn ? board.king_B : board.king_W;
    Bitboard knight = board.turn ? board.knight_B : board.knight_W;
    Bitboard bishop = board.turn ? board.bishop_B : board.bishop_W;
    Bitboard rook = board.turn ? board.rook_B : board.rook_W;
    Bitboard queen = board.turn ? board.queen_B : board.queen_W;

    while (queen) {
        int sq = bitScanForward(&queen);

        Bitboard attacks = getBishopAttacks(sq, board.occupancy);
        attacks |= getRookAttacks(sq, board.occupancy);
        if (attacks & sqBb) return true;
    }
    while (bishop) {
        int sq = bitScanForward(&bishop);
        Bitboard attacks = getBishopAttacks(sq, board.occupancy);
        if (attacks & sqBb) return true;
    }
    while (rook) {
        int sq = bitScanForward(&rook);
        Bitboard attacks = getRookAttacks(sq, board.occupancy);
        if (attacks & sqBb) return true;
    }
    while (knight) {
        int sq = bitScanForward(&knight);
        if (KNIGHT_MOVEMENT[sq] & sqBb) return true;
    }
    while (pawn) {
        int sq = bitScanForward(&pawn);

        if (board.turn) {
            if (PAWN_B_ATTACKS_EAST[sq] & sqBb) return true;
            if (PAWN_B_ATTACKS_WEST[sq] & sqBb) return true;
        } else {
            if (PAWN_W_ATTACKS_EAST[sq] & sqBb) return true;
            if (PAWN_W_ATTACKS_WEST[sq] & sqBb) return true;
        }
    }
    while (king) {
        int sq = bitScanForward(&king);
        if (KING_MOVEMENT[sq] & sqBb) return true;
    }

    return false;
}

Bitboard WHITE_CASTLE_K_PATH = 0b110;
Bitboard WHITE_CASTLE_Q_PATH = 0b1110000;
Bitboard BLACK_CASTLE_K_PATH = 0b11LL << G8;
Bitboard BLACK_CASTLE_Q_PATH = 0b111LL << D8;

int legalMoves(Board board, Move moves[]) {
    int length = 0;

    // Color relative variables
    int kingSquare = board.turn ? board.whiteKingSq : board.blackKingSq;
    Bitboard friendlyOccupancy = board.turn ? board.occupancyWhite : board.occupancyBlack;
    Bitboard bishopBitboard = board.turn ? board.bishop_W : board.bishop_B;
    Bitboard rookBitboard = board.turn ? board.rook_W : board.rook_B;
    Bitboard queenBitboard = board.turn ? board.queen_W : board.queen_B;
    Bitboard knightBitboard = board.turn ? board.knight_W : board.knight_B;
    Bitboard pawnMask = board.turn ? board.pawn_W : board.pawn_B;
 
    Bitboard singlePush;
    Bitboard doublePush;
    pawnSingleAndDblPushes(board, &singlePush, &doublePush);
    Bitboard kingMovesMask = getKingMask(board);

    Bitboard epSquare = board.epSquare == -1 ? 0LL : 1LL << board.epSquare;

    // Pawn attacks
    while (pawnMask) {
        int sq = bitScanForward(&pawnMask);
        bool isPromoting = board.turn ? (sq >= H7 && sq <= A7) : (sq >= H2 && sq <= A2);
        if (board.turn) {
            // Pawn east attack
            if (PAWN_W_ATTACKS_EAST[sq] & (board.occupancyBlack | epSquare)) {
                int toSquare = sq + 7;
                addPawnAdvanceWithPossiblePromos(board, isPromoting, board.turn, sq, toSquare, moves, &length);
            }
            
            // Pawn west attack
            if (PAWN_W_ATTACKS_WEST[sq] & (board.occupancyBlack | epSquare)) {
                int toSquare = sq + 9;
                addPawnAdvanceWithPossiblePromos(board, isPromoting, board.turn, sq, toSquare, moves, &length);
            }
        } else {
            // Pawn east attack
            if (PAWN_B_ATTACKS_EAST[sq] & (board.occupancyWhite | epSquare)) {
                int toSquare = sq - 7;
                addPawnAdvanceWithPossiblePromos(board, isPromoting, board.turn, sq, toSquare, moves, &length);
            }
            
            // Pawn west attack
            if (PAWN_B_ATTACKS_WEST[sq] & (board.occupancyWhite | epSquare)) {
                int toSquare = sq - 9;
                addPawnAdvanceWithPossiblePromos(board, isPromoting, board.turn, sq, toSquare, moves, &length);
            }
        }
    }

    while (singlePush) {
        int sq = bitScanForward(&singlePush);
        int fromSquare = board.turn ? sq-8 : sq+8;
        bool isPromoting = board.turn ? (fromSquare >= H7 && fromSquare <= A7) : (fromSquare >= H2 && fromSquare <= A2);
        addPawnAdvanceWithPossiblePromos(board, isPromoting, board.turn, fromSquare, sq, moves, &length);
    }

    while (doublePush) {
        int sq = bitScanForward(&doublePush);
        int fromSquare = board.turn ? sq-8*2 : sq+8*2;
        Move move = getMove(fromSquare, sq, NO_PROMOTION, NOT_CASTLE);
        addMove(board, move, moves, &length);
    }

    while (kingMovesMask) {
        int sq = bitScanForward(&kingMovesMask);
        Move move = getMove(kingSquare, sq, NO_PROMOTION, NOT_CASTLE);
        isMoveLegal(board, move);
        addMove(board, move, moves, &length);
    }
    while (bishopBitboard) {
        int sq = bitScanForward(&bishopBitboard);
        Bitboard target = getBishopAttacks(sq, board.occupancy);
        target ^= target & friendlyOccupancy;

        while (target) {
            int indx = bitScanForward(&target);
            Move move = getMove(sq, indx, NO_PROMOTION, NOT_CASTLE);
            addMove(board, move, moves, &length);
        }
    }
    while (rookBitboard) {
        int sq = bitScanForward(&rookBitboard);
            Bitboard target = getRookAttacks(sq, board.occupancy);
            target ^= target & friendlyOccupancy;

            while (target) {
                int indx = bitScanForward(&target);
                Move move = getMove(sq, indx, NO_PROMOTION, NOT_CASTLE);
                addMove(board, move, moves, &length);
            }
 
    }
    while (queenBitboard) {
        int sq = bitScanForward(&queenBitboard);
        Bitboard rookAttacks = getRookAttacks(sq, board.occupancy);
        Bitboard bishopAttacks = getBishopAttacks(sq, board.occupancy);
        Bitboard target = bishopAttacks | rookAttacks;
        target ^= target & friendlyOccupancy;

        while (target) {
            int indx = bitScanForward(&target);
            Move move = getMove(sq, indx, NO_PROMOTION, NOT_CASTLE);
            addMove(board, move, moves, &length);
        }

    }
    while (knightBitboard) {
        int sq = bitScanForward(&knightBitboard);
        Bitboard target = KNIGHT_MOVEMENT[sq] ^ (KNIGHT_MOVEMENT[sq] & friendlyOccupancy);

        while (target) {
            int indx = bitScanForward(&target);
            Move move = getMove(sq, indx, NO_PROMOTION, NOT_CASTLE);
            addMove(board, move, moves, &length);
        }
    }

    // Castling
    if (board.turn) {
        if (board.castling & K) {
            bool isNotInCheck = ! isSquareAttacked(board, E1);
            bool pathClear = (board.occupancy & WHITE_CASTLE_K_PATH) == 0;
            bool noAttacks = ! isSquareAttacked(board, F1);
            pathClear = pathClear && noAttacks;

            if (isNotInCheck && pathClear) {
                Move move = getMove(0, 0, NO_PROMOTION, K);
                addMove(board, move, moves, &length);
            }
        }
        if (board.castling & Q) {
            bool isNotInCheck = ! isSquareAttacked(board, E1);
            bool pathClear = (board.occupancy & WHITE_CASTLE_Q_PATH) == 0;
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
            bool pathClear = (board.occupancy & BLACK_CASTLE_K_PATH) == 0;
            bool noAttacks = ! isSquareAttacked(board, F8);
            pathClear = pathClear && noAttacks;

            if (isNotInCheck && pathClear) {
                Move move = getMove(0, 0, NO_PROMOTION, k);
                addMove(board, move, moves, &length);
            }
        }
        if (board.castling & q) {
            bool isNotInCheck = ! isSquareAttacked(board, E8);
            bool pathClear = (board.occupancy & BLACK_CASTLE_Q_PATH) == 0;
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
    int fromFile = 'h' - san[0];
    int fromRank = atoi(&san[1]) - 1;
    int toFile = 'h' - san[2];
    int toRank = atoi(&san[3]) - 1;
    move->fromSquare = 8 * (fromRank) + fromFile;
    move->toSquare = 8 * (toRank) + toFile;
    move->castle = 0;
    move->promotion = -1;

    if (move->fromSquare == E1 && move->toSquare == G1 && (board.castling & K)) {
        move->castle = K;
    }
    else if (move->fromSquare == E1 && move->toSquare == C1 && (board.castling & Q)) {
        move->castle = Q;
    }
    else if (move->fromSquare == E8 && move->toSquare == G8 && (board.castling & k)) {
        move->castle = k;
    }
    else if (move->fromSquare == E8 && move->toSquare == C8 && (board.castling & q)) {
        move->castle = q;
    }

    if (san[4] == 'q') {
        move->promotion = board.turn ? QUEEN_W : QUEEN_B;
    }
    else if (san[4] == 'r') {
        move->promotion = board.turn ? ROOK_W : ROOK_B;
    }
    else if (san[4] == 'n') {
        move->promotion = board.turn ? KNIGHT_W : KNIGHT_B;
    }
    else if (san[4] == 'b') {
        move->promotion = board.turn ? BISHOP_W : BISHOP_B;
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
            case QUEEN_B:
                san[4] = 'q';
                break;
            case ROOK_W:
            case ROOK_B:
                san[4] = 'r';
                break;
            case BISHOP_W:
            case BISHOP_B:
                san[4] = 'b';
                break;
            case KNIGHT_W:
            case KNIGHT_B:
                san[4] = 'n';
                break;
        }
    }
}