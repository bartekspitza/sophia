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

Board initBoard();
void printBitboard(Bitboard bb);
void printBoard(Board board);
int getBit(Bitboard bb, int bit);
void clearBit(Bitboard* bb, int bit);
void setBit(Bitboard* bb, int bit);
void makeMove(Board* board, char* move);

int main() {
    Board board = initBoard();
    makeMove(&board, "e2e4");
    makeMove(&board, "d7d6");
    makeMove(&board, "g1f3");
    printBoard(board);
    return 0;
}

void makeMove(Board* board, char* move) {
    int from_file = H_FILE - move[0];
    int from_rank = atoi(&move[1]) - 1;
    int to_file = H_FILE - move[2];
    int to_rank = atoi(&move[3]) - 1;
    int from_square = ROWS * (from_rank) + (from_file);
    int to_square = ROWS * (to_rank) + (to_file);

    Bitboard* bb = board->turn ? &(board->pawn_W) : &(board->pawn_B);

    for (int i = 0; i < 6; i++) {
        bb += i;
        if (getBit(*bb, from_square)) {
            clearBit(bb, from_square);
            setBit(bb, to_square);
            break;
        }
    }
    // Toggle the turn
    board->turn ^= 1;
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
