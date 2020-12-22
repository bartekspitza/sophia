#ifndef TYPEDEFS_H
#define TYPEDEFS_H

#define BLACK 0
#define WHITE 1

#include <stdint.h>

typedef uint64_t Bitboard;
typedef struct {
    Bitboard pawn_W;
    Bitboard knight_W;
    Bitboard bishop_W;
    Bitboard rook_W;
    Bitboard queen_W;
    Bitboard king_W;
    Bitboard pawn_B;
    Bitboard knight_B;
    Bitboard bishop_B;
    Bitboard rook_B;
    Bitboard queen_B;
    Bitboard king_B;
    int turn;
    int castling;
    int epSquare;
    int halfmoves;
    int fullmoves;
} Board;

typedef struct {
    Bitboard fromSquare;
    Bitboard toSquare;
} Move;

enum PIECES {
    PAWN_W, KNIGHT_W, BISHOP_W, ROOK_W, QUEEN_W, KING_W,
    PAWN_B, KNIGHT_B, BISHOP_B, ROOK_B, QUEEN_B, KING_B
};

#endif