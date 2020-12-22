#include "board.h"
#include <stdbool.h>
#include <inttypes.h>
#include <stdio.h>
#include <stdlib.h>
#include "fen.h"

Bitboard pawnStartWhite = 0xFF00;
Bitboard pawnStartBlack = 0x00FF000000000000;

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

char CASTLING_RIGHTS[4][2] = { "K", "Q", "k", "q" };

Bitboard* pieceBitboard(Board* board, int pieceType) {
    return (Bitboard*) board + pieceType;
}

void reset(Board* board) {
    Bitboard* bb = (Bitboard*) (board);
    for (int i = 0; i < 12; i++) {
        *bb &= 0;
        bb++;
    }
    board->turn = WHITE;
    board->castling = 0;
    board->epSquare = -1;
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

void initBoard(Board* board) {
    setFen(board, START_FEN);
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
    char pieceSymbols[] = "PNBRQKpnbrqk";
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

    printf("Turn: %s\n", board.turn ? "White" : "Black");
    printf("Castling: ");
    for (int i = 0; i < 4; i++) {
        if ((board.castling & (1 << i)) >> i) {
            putchar(CASTLING_RIGHTS[i][0]);
        }
    }
    printf("\nEp square: %s (%d)\n", board.epSquare == -1 ? "None" : &SQUARE_NAMES[board.epSquare][0], board.epSquare);
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