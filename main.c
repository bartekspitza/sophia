#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <inttypes.h>

#define ROWS 8
#define COLS 8
#define H_FILE 'h'
#define pieceSymbols "PNBRQKpnbrqk"

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
} Board;

typedef struct {
    Bitboard fromSquare;
    Bitboard toSquare;
} Move;

Bitboard pawnStartWhite = 0xFF00;
Bitboard pawnStartBlack = 0x00FF000000000000;

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

char SQUARE_NAMES[64][3] = {
    "h1", "g1", "f1", "e1", "d1", "c1", "b1", "a1",
    "h2", "g2", "f2", "e2", "d2", "c2", "b2", "a2",
    "h3", "g3", "f3", "e3", "d3", "c3", "b3", "a3",
    "h4", "g4", "f4", "e4", "d4", "c4", "b4", "a4",
    "h5", "g5", "f5", "e5", "d5", "c5", "b5", "a5",
    "h6", "g6", "f6", "e6", "d6", "c6", "b6", "a6",
    "h7", "g7", "f7", "e7", "d7", "c7", "b7", "a7",
    "h8", "g8", "f8", "e8", "d8", "c8", "b8", "a8"
};

Board initBoard();
void printBitboard(Bitboard bb);
void printBits(Bitboard bb);
void printBoard(Board board);
int getBit(Bitboard bb, int bit);
void clearBit(Bitboard* bb, int bit);
void setBit(Bitboard* bb, int bit);
void pushSan(Board* board, char* move);
void moveToSan(Move move, char* san);
void pushMove(Board* board, Move move);
void pushMove(Board* board, Move move);
Move* legalMoves(Board board, int* length);

int main() {
    Board board = initBoard();
    printBoard(board);

    int length;
    Move* first = legalMoves(board, &length);
    Move* move = first;
    for (int i = 0; i < length;i++) {
        char san[6];
        moveToSan(*move, (char*) &san);
        printf("%s\n", (char*) &san);
        move++;
    }

    return 0;
}

void moveToSan(Move move, char* san) {
    *san = SQUARE_NAMES[move.fromSquare][0];
    *(san+1) = SQUARE_NAMES[move.fromSquare][1];
    *(san+2) = SQUARE_NAMES[move.toSquare][0];
    *(san+3) = SQUARE_NAMES[move.toSquare][1];
}

Move* legalMoves(Board board, int* length) {
    Move* first = (Move*) malloc(sizeof(Move));
    Move* move = first;

    Bitboard occupancy = board.pawn_W;
    for (int i = 0; i < 12;i++) {
        occupancy |= *(&(board.pawn_W)+i);
    }

    // Pawn pushes
    if (board.turn) {
        Bitboard singlePush = board.pawn_W << ROWS;
        singlePush ^= singlePush & occupancy;

        Bitboard doublePush = (board.pawn_W & pawnStartWhite) << ROWS*2;
        doublePush ^= doublePush & occupancy;
        doublePush = doublePush >> 8;
        doublePush &= singlePush;
        doublePush = doublePush << 8;

        for (int i = 0; i < 64;i++) {
            if (getBit(singlePush, i)) {
                move->fromSquare = i-ROWS;
                move->toSquare = i;
                move++;
                (*length)++;
            }
            if (getBit(doublePush, i)) {
                move->fromSquare = i-ROWS*2;
                move->toSquare = i;
                move++;
                (*length)++;
            }
        }
    } else {
        Bitboard singlePush = board.pawn_B >> ROWS;
        singlePush ^= singlePush & occupancy;

        Bitboard doublePush = (board.pawn_B & pawnStartBlack) >> ROWS*2;
        doublePush ^= doublePush & occupancy;
        doublePush = doublePush << 8;
        doublePush &= singlePush;
        doublePush = doublePush >> 8;

        for (int i = 0; i < 64;i++) {
            if (getBit(singlePush, i)) {
                move->fromSquare = i+ROWS;
                move->toSquare = i;
                move++;
                (*length)++;
            }
            if (getBit(doublePush, i)) {
                move->fromSquare = i+ROWS*2;
                move->toSquare = i;
                move++;
                (*length)++;
            }
        }
    }

    return first;
}

void pushMove(Board* board, Move move) {
    Bitboard* bb = board->turn ? &(board->pawn_W) : &(board->pawn_W);

    for (int i = 0; i < 12; i++) {
        if (getBit(*bb, move.fromSquare)) {
            clearBit(bb, move.fromSquare);
            setBit(bb, move.toSquare);
        } else if (getBit(*bb, move.toSquare)) {
            clearBit(bb, move.toSquare);
        }
        ++bb;
    }
    // Toggle the turn
    board->turn ^= 1;
}

void pushSan(Board* board, char* san) {
    int fromFile = H_FILE - san[0];
    int fromRank = atoi(&san[1]) - 1;
    int toFile = H_FILE - san[2];
    int toRank = atoi(&san[3]) - 1;
    Move move = {
        .fromSquare = ROWS * (fromRank) + (fromFile),
        .toSquare = ROWS * (toRank) + (toFile)
    };
    pushMove(board, move);
}

Board initBoard() {
    Board board = {
        .pawn_W   = pawnStartWhite,
        .knight_W = 0b01000010,
        .bishop_W = 0b00100100,
        .rook_W   = 0b10000001,
        .queen_W  = 0b00010000,
        .king_W   = 0b00001000,
        .pawn_B   = pawnStartBlack,
        .knight_B = 0b0010010000000000000000000000000000000000000000000000000000000000,
        .bishop_B = 0b0100001000000000000000000000000000000000000000000000000000000000,
        .rook_B   = 0b1000000100000000000000000000000000000000000000000000000000000000,
        .queen_B  = 0b0001000000000000000000000000000000000000000000000000000000000000,
        .king_B   = 0b0000100000000000000000000000000000000000000000000000000000000000,
        .turn = 1
    };
    return board;
}

void setBit(Bitboard* bb, int bit) {
    *bb |= (1LL << bit);
}
void clearBit(Bitboard* bb, int bit) {
    *bb &= ~(1LL << bit);
}

int getBit(Bitboard bb, int bit) {
    Bitboard val = bb & (1LL << bit);
    val = val >> bit;
    return (int) val;
}

void printBoard(Board board) {
    printf("\n");
    for (int y = 0; y < 8; y++) {
        for (int x = 0; x < 8; x++) {

            int loc = 63 - ((y*8) + x);
            bool printed = false;

            for (int i = 0; i < 12; i++) {
                Bitboard* bb = ((Bitboard*) &board) + i;                    

                if (getBit(*bb, loc)) {
                    printf("%c", pieceSymbols[i]);
                    printed = true;
                    break;
                }
            }

            if (! printed) putchar('.');
            printf(" ");
        }
        printf("\n");
    }
    printf("\n");
}

void printBitboard(Bitboard bb) {
    printf("\n");
    for (int y = 0; y < 8; y++) {
        for (int x = 0; x < 8; x++) {

            int loc = 63 - ((y*8) + x);

            if (getBit(bb, loc)) {
                putchar('x');
            } else {
                putchar('.');
            }

            printf(" ");
        }
        printf("\n");
    }
    printf("\n");
}

void printBits(Bitboard bb) {
    for (int i = 63; i >= 0; i--) {
        int bit = getBit(bb, i);
        if (bit == 0) {
            printf("0");
        } else if (bit == 1) {
            printf("1");
        }
    }
    printf("\n");
}
