#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include "movegen.h"
#include "board.h"
#include "utils.h"

#define getMove(from, to, promo, castling, pType) {.fromSquare=from, .toSquare=to, .promotion=promo, .castle=castling, .pieceType=pType}
#define addMove moves[length] = move; length++;

Bitboard PAWN_START_WHITE = 0xFF00;
Bitboard PAWN_START_BLACK = 0x00FF000000000000;
Bitboard WHITE_CASTLE_K_PATH = 0b110;
Bitboard WHITE_CASTLE_Q_PATH = 0b1110000;
Bitboard BLACK_CASTLE_K_PATH = 0b11LL << G8;
Bitboard BLACK_CASTLE_Q_PATH = 0b111LL << D8;
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

Bitboard occupancyMask(int index, int bits, Bitboard attackMask) {
    Bitboard occupancy = 0ULL;
    
    for (int i = 0; i < bits; i++) {
        int square = __builtin_ctzll(attackMask);
        attackMask = toggleBit(attackMask, square);
        
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

Bitboard getKingMask(Board board) {
    int kingSquare = board.turn ? board.whiteKingSq : board.blackKingSq;
    Bitboard opponentOccupancy = board.turn ? board.occupancyWhite : board.occupancyBlack;
    return KING_MOVEMENT[kingSquare] & ~opponentOccupancy;
}

void validateMove(Board board, Move* move) {
    if (move->castle) {
        // Does king travel over attacked squares?
        int sq;
        if (move->castle == K) sq = F1;
        if (move->castle == Q) sq = D1;
        if (move->castle == k) sq = F8;
        if (move->castle == q) sq = D8;
        bool attackedTravel = isSquareAttacked(board, sq);
        if (attackedTravel) {
            move->validation = ILLEGAL;
            return;
        }

        // Is king in check?
        bool isInCheck = isSquareAttacked(board, board.turn ? board.whiteKingSq : board.blackKingSq);
        if (isInCheck) {
            move->validation = ILLEGAL;
            return;
        }
    }

    Board cpy = board;
    pushMove(&cpy, *move);

    int kingSquare = cpy.turn ? cpy.blackKingSq : cpy.whiteKingSq;
    cpy.turn = cpy.turn ? 0 : 1;
    bool isInCheckAfterMove = isSquareAttacked(cpy, kingSquare);

    move->validation = isInCheckAfterMove ? ILLEGAL : LEGAL;
}

/**
 * Computes pawn moves, single and double pushes
*/
void pawnSingleAndDblPushes(Board board, Bitboard* single, Bitboard* dbl) {
    *single = board.turn ? board.pawn_W << 8 : board.pawn_B >> 8;
    *single = *single & ~board.occupancy;

    *dbl = board.turn ? (board.pawn_W & PAWN_START_WHITE) << 16 : (board.pawn_B & PAWN_START_BLACK) >> 16;
    *dbl = *dbl & ~board.occupancy;

    // Double pushes can't jump over pieces
    *dbl = board.turn ? *dbl >> 8 : *dbl << 8;
    *dbl &= *single;
    *dbl = board.turn ? *dbl << 8 : *dbl >> 8;
}

void addPawnAdvanceWithPossiblePromos(Board board, bool isPromoting, int turn, int from, int to, Move moves[], int* indx) {
    if (isPromoting) {
        for (int i = 0; i < 4; i++) {
            Move move = getMove(from, to, turn ? WHITE_PROMOTIONS[i] : BLACK_PROMOTIONS[i], NOT_CASTLE, board.turn ? PAWN_W : PAWN_B);
            validateMove(board, &move);
            if (move.validation == ILLEGAL) continue;

            moves[*indx] = move;
            *indx += 1;
        }
        return;
    }

    Move move = getMove(from, to, NO_PROMOTION, NOT_CASTLE, board.turn ? PAWN_W : PAWN_B);
    validateMove(board, &move);
    if (move.validation == ILLEGAL) return;
    moves[*indx] = move;
    *indx += 1;
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

/**
 * Whether the given square is attacked by the opponent
*/
bool isSquareAttacked(Board board, int square) {
    Bitboard sqBb = SQUARE_BITBOARDS[square];
    Bitboard pawn = board.turn ? board.pawn_B : board.pawn_W;
    Bitboard king = board.turn ? board.king_B : board.king_W;
    Bitboard knight = board.turn ? board.knight_B : board.knight_W;
    Bitboard bishop = board.turn ? board.bishop_B : board.bishop_W;
    Bitboard rook = board.turn ? board.rook_B : board.rook_W;
    Bitboard queen = board.turn ? board.queen_B : board.queen_W;

    while (queen) {
        int sq = __builtin_ctzl(queen);

        Bitboard attacks = getBishopAttacks(sq, board.occupancy);
        attacks |= getRookAttacks(sq, board.occupancy);
        if (attacks & sqBb) return true;

        queen &= queen - 1;
    }
    while (bishop) {
        int sq = __builtin_ctzl(bishop);
        Bitboard attacks = getBishopAttacks(sq, board.occupancy);
        if (attacks & sqBb) return true;
        bishop &= bishop - 1;
    }
    while (rook) {
        int sq = __builtin_ctzl(rook);
        Bitboard attacks = getRookAttacks(sq, board.occupancy);
        if (attacks & sqBb) return true;
        rook &= rook - 1;
    }
    while (knight) {
        int sq = __builtin_ctzl(knight);
        if (KNIGHT_MOVEMENT[sq] & sqBb) return true;
        knight &= knight - 1;
    }
    while (pawn) {
        int sq = __builtin_ctzl(pawn);

        if (board.turn) {
            if (PAWN_B_ATTACKS_EAST[sq] & sqBb) return true;
            if (PAWN_B_ATTACKS_WEST[sq] & sqBb) return true;
        } else {
            if (PAWN_W_ATTACKS_EAST[sq] & sqBb) return true;
            if (PAWN_W_ATTACKS_WEST[sq] & sqBb) return true;
        }
        pawn &= pawn - 1;
    }
    while (king) {
        int sq = __builtin_ctzll(king);
        if (KING_MOVEMENT[sq] & sqBb) return true;
        king &= king - 1;
    }

    return false;
}

/**
* Generates all legal moves for the given board
*/
int legalMoves(Board* board, Move moves[]) {
    int length = 0;

    // Already known
    int kingSquare = board->turn ? board->whiteKingSq : board->blackKingSq;
    Bitboard friendlyOccupancy = board->turn ? board->occupancyWhite : board->occupancyBlack;
    Bitboard bishopBitboard = board->turn ? board->bishop_W : board->bishop_B;
    Bitboard rookBitboard = board->turn ? board->rook_W : board->rook_B;
    Bitboard queenBitboard = board->turn ? board->queen_W : board->queen_B;
    Bitboard knightBitboard = board->turn ? board->knight_W : board->knight_B;
    Bitboard pawnMask = board->turn ? board->pawn_W : board->pawn_B;

    Bitboard attackMask = 0;
 
    // Generate all pieces movement, starting with pawns
    Bitboard singlePush;
    Bitboard doublePush;
    pawnSingleAndDblPushes(*board, &singlePush, &doublePush); // Single and double pawn pushes

    // Captures
    Bitboard epSquare = board->epSquare == -1 ? 0LL : SQUARE_BITBOARDS[board->epSquare];
    while (pawnMask) {
        int sq = __builtin_ctzll(pawnMask);
        bool isPromoting = board->turn ? (SQUARE_BITBOARDS[sq] & RANK_7) : (SQUARE_BITBOARDS[sq] & RANK_1);

        Bitboard occ = epSquare | (board->turn ? board->occupancyBlack : board->occupancyWhite);
        if (board->turn) {
            Bitboard eastAttacks = PAWN_W_ATTACKS_EAST[sq] & occ;
            attackMask |= PAWN_W_ATTACKS_EAST[sq];
            attackMask |= PAWN_W_ATTACKS_WEST[sq];
            if (eastAttacks) {
                int toSquare = sq + 7;
                addPawnAdvanceWithPossiblePromos(*board, isPromoting, board->turn, sq, toSquare, moves, &length);
            }
            Bitboard westAttacks = PAWN_W_ATTACKS_WEST[sq] & occ;
            if (westAttacks) {
                int toSquare = sq + 9;
                addPawnAdvanceWithPossiblePromos(*board, isPromoting, board->turn, sq, toSquare, moves, &length);
            }
        } else {
            Bitboard eastAttacks = PAWN_B_ATTACKS_EAST[sq] & occ;
            attackMask |= PAWN_B_ATTACKS_EAST[sq];
            attackMask |= PAWN_B_ATTACKS_WEST[sq];
            if (eastAttacks) {
                int toSquare = sq - 7;
                addPawnAdvanceWithPossiblePromos(*board, isPromoting, board->turn, sq, toSquare, moves, &length);
            }
            Bitboard westAttacks = PAWN_B_ATTACKS_WEST[sq] & occ;
            if (westAttacks) {
                int toSquare = sq - 9;
                addPawnAdvanceWithPossiblePromos(*board, isPromoting, board->turn, sq, toSquare, moves, &length);
            }
        }
        pawnMask &= pawnMask - 1;
    }

    Bitboard kingMovesMask = getKingMask(*board);
    attackMask |= KING_MOVEMENT[kingSquare];

    while (singlePush) {
        int sq = __builtin_ctzll(singlePush);
        int fromSquare = board->turn ? sq-8 : sq+8;
        bool isPromoting = board->turn ? (fromSquare >= H7 && fromSquare <= A7) : (fromSquare >= H2 && fromSquare <= A2);
        addPawnAdvanceWithPossiblePromos(*board, isPromoting, board->turn, fromSquare, sq, moves, &length);
        singlePush &= singlePush - 1;
    }

    int startPieceType = board->turn ? PAWN_W : PAWN_B;
    while (doublePush) {
        int sq = __builtin_ctzll(doublePush);
        int fromSquare = board->turn ? sq-8*2 : sq+8*2;
        Move move = getMove(fromSquare, sq, NO_PROMOTION, NOT_CASTLE, startPieceType + PAWN_W);
        validateMove(*board, &move);
        if (move.validation == LEGAL) {
            addMove
        }
        doublePush &= doublePush - 1;
    }

    while (kingMovesMask) {
        int sq = __builtin_ctzll(kingMovesMask);
        Move move = getMove(kingSquare, sq, NO_PROMOTION, NOT_CASTLE, startPieceType + KING_W);
        validateMove(*board, &move);
        if (move.validation == LEGAL) {
            addMove
        }
        kingMovesMask &= kingMovesMask - 1;
    }
    while (bishopBitboard) {
        int sq = __builtin_ctzll(bishopBitboard);
        Bitboard attacks = getBishopAttacks(sq, board->occupancy);
        attackMask |= attacks;
        attacks = attacks & ~friendlyOccupancy;

        while (attacks) {
            int indx = __builtin_ctzll(attacks);
            Move move = getMove(sq, indx, NO_PROMOTION, NOT_CASTLE, startPieceType + BISHOP_W);
            validateMove(*board, &move);
            if (move.validation == LEGAL) {
                addMove
            }
            attacks &= attacks - 1;
        }
        bishopBitboard &= bishopBitboard - 1;
    }
    while (rookBitboard) {
        int sq = __builtin_ctzll(rookBitboard);
        Bitboard attacks = getRookAttacks(sq, board->occupancy);
        attackMask |= attacks;
        attacks = attacks & ~friendlyOccupancy;

        while (attacks) {
            int indx = __builtin_ctzll(attacks);
            Move move = getMove(sq, indx, NO_PROMOTION, NOT_CASTLE, startPieceType + ROOK_W);
            validateMove(*board, &move);
            if (move.validation == LEGAL) {
                addMove
            }
            attacks &= attacks - 1;
        }

        rookBitboard &= rookBitboard - 1;
    }
    while (queenBitboard) {
        int sq = __builtin_ctzll(queenBitboard);
        Bitboard rookAttacks = getRookAttacks(sq, board->occupancy);
        Bitboard bishopAttacks = getBishopAttacks(sq, board->occupancy);
        Bitboard attacks = bishopAttacks | rookAttacks;
        attackMask |= attacks;
        attacks = attacks & ~friendlyOccupancy;

        while (attacks) {
            int indx = __builtin_ctzll(attacks);
            Move move = getMove(sq, indx, NO_PROMOTION, NOT_CASTLE, startPieceType + QUEEN_W);
            validateMove(*board, &move);
            if (move.validation == LEGAL) {
                addMove
            }
            attacks &= attacks - 1;
        }
        queenBitboard &= queenBitboard - 1;

    }
    while (knightBitboard) {
        int sq = __builtin_ctzll(knightBitboard);
        attackMask |= KNIGHT_MOVEMENT[sq];
        Bitboard target = KNIGHT_MOVEMENT[sq] & ~friendlyOccupancy;

        while (target) {
            int indx = __builtin_ctzll(target);
            Move move = getMove(sq, indx, NO_PROMOTION, NOT_CASTLE, startPieceType + KNIGHT_W);
            validateMove(*board, &move);
            if (move.validation == LEGAL) {
                addMove
            }
            target &= target - 1;
        }
        knightBitboard &= knightBitboard - 1;
    }

    // Castling
    if (board->turn) {
        if (board->castling & K) {
            bool pathClear = (board->occupancy & WHITE_CASTLE_K_PATH) == 0;
            if (pathClear) {
                Move move = getMove(E1, G1, NO_PROMOTION, K, -1);
                validateMove(*board, &move);
                if (move.validation == LEGAL) {
                    addMove
                }
            }
        }
        if (board->castling & Q) {
            bool pathClear = (board->occupancy & WHITE_CASTLE_Q_PATH) == 0;
            if (pathClear) {
                Move move = getMove(E1, C1, NO_PROMOTION, Q, -1);
                validateMove(*board, &move);
                if (move.validation == LEGAL) {
                    addMove
                }
            }
        }
    } else {
        if (board->castling & k) {
            bool pathClear = (board->occupancy & BLACK_CASTLE_K_PATH) == 0;
            if (pathClear) {
                Move move = getMove(E8, G8, NO_PROMOTION, k, -1);
                validateMove(*board, &move);
                if (move.validation == LEGAL) {
                    addMove
                }
            }
        }
        if (board->castling & q) {
            bool pathClear = (board->occupancy & BLACK_CASTLE_Q_PATH) == 0;

            if (pathClear) {
                Move move = getMove(E8, C8, NO_PROMOTION, q, -1);
                validateMove(*board, &move);
                if (move.validation == LEGAL) {
                    addMove
                }
            }
        }
    }

    board->attacks = attackMask;
    return length;
}
