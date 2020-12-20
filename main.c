#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <inttypes.h>

#define pieceSymbols "pnbrqkPNBRQK"

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
} Board;

Board initBoard();
void printBitboard(Bitboard bb);
void printBoard(Board board);
int getBit(Bitboard bb, int bit);

int main() {
    Board board = initBoard();
    printBoard(board);
    return 0;
}

void printBoard(Board board) {
    printf("\n");
    for (int y = 0; y < 8; y++) {
        char row[17];
        row[16] = '\0';

        for (int x = 0; x < 8; x++) {
            row[x*2+1] = ' ';

            int loc = 63 - ((y*8) + x);
            bool printed = false;

            for (int i = 0; i < 12; i++) {
                Bitboard* bb = ((Bitboard*) &board) + i;                    

                int bit = getBit(*bb, loc);
                if (bit == 1) {
                    row[x*2] = pieceSymbols[i];
                    printed = true;
                }
            }

            if (! printed) row[x*2] = ' ';
        }
        printf("%s\n", row);
    }
    printf("\n");
}

Board initBoard() {
    Board board = {
        .pawn_W   = 0b1111111100000000,
        .knight_W = 0b01000010,
        .bishop_W = 0b00100100,
        .rook_W   = 0b10000001,
        .queen_W  = 0b00010000,
        .king_W   = 0b00001000,
        .pawn_B   = 0b0000000011111111000000000000000000000000000000000000000000000000,
        .knight_B = 0b0010010000000000000000000000000000000000000000000000000000000000,
        .bishop_B = 0b0100001000000000000000000000000000000000000000000000000000000000,
        .rook_B   = 0b1000000100000000000000000000000000000000000000000000000000000000,
        .queen_B  = 0b0001000000000000000000000000000000000000000000000000000000000000,
        .king_B   = 0b0000100000000000000000000000000000000000000000000000000000000000,
    };
    return board;
}

int getBit(Bitboard bb, int bit) {
    Bitboard val = bb & (1LL << bit);
    val = val >> bit;
    return (int) val;
}

void printBitboard(Bitboard bb) {
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
