#ifndef TYPEDEFS_H
#define TYPEDEFS_H

#define BLACK 0
#define WHITE 1

#include <stdint.h>
#include <stdbool.h>

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
    int whiteKingSq;
    int blackKingSq;
    Bitboard occupancy;
    Bitboard occupancyWhite;
    Bitboard occupancyBlack;
    Bitboard hash;
    Bitboard attacks; // The attack mask of the other side, i.e. when it's white's turn, this is the black attack mask
} Board;

typedef struct {
    int fromSquare;
    int toSquare;
    int promotion;
    int castle;
    int validation;
    int pieceType;
    int score;
    bool exhausted;
} Move;

enum PIECES {
    PAWN_W, KNIGHT_W, BISHOP_W, ROOK_W, QUEEN_W, KING_W,
    PAWN_B, KNIGHT_B, BISHOP_B, ROOK_B, QUEEN_B, KING_B
};

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

enum MOVE_VALIDAITON {NOT_VALIDATED, LEGAL, ILLEGAL};

enum GAME_RESULT {UN_DETERMINED, WHITE_WIN, BLACK_WIN, DRAW};

enum CASTLING {K=1, Q=2, k=4, q=8};

enum FILES {H, G, F, E, D, C, B, A};

extern char SQUARE_NAMES[64][3];

#endif