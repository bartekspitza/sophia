#include <stdio.h>
#include <stdint.h>
#include <inttypes.h>

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
    for (int y = 0; y < 8; y++) {
        char row[17];
        row[16] = '\0';

        for (int x = 0; x < 16; x++) {
            if (x % 2 != 0) {
                row[x] = ' ';
            } else {
                int loc = 63 - ((y*8) + x/2);
                Bitboard pw = getBit(board.pawn_W, loc);
                Bitboard nw = getBit(board.knight_W, loc);
                Bitboard bw = getBit(board.bishop_W, loc);
                Bitboard rw = getBit(board.rook_W, loc);
                Bitboard qw = getBit(board.queen_W, loc);
                Bitboard kw = getBit(board.king_W, loc);
                Bitboard pb = getBit(board.pawn_B, loc);
                Bitboard nb = getBit(board.knight_B, loc);
                Bitboard bb = getBit(board.bishop_B, loc);
                Bitboard rb = getBit(board.rook_B, loc);
                Bitboard qb = getBit(board.queen_B, loc);
                Bitboard kb = getBit(board.king_B, loc);
                
                if (pw == 1) {
                    row[x] = 'P';
                } else if (nw == 1) {
                    row[x] = 'N';
                } else if (bw == 1) {
                    row[x] = 'B';
                } else if (rw == 1) {
                    row[x] = 'R';
                } else if (qw == 1) {
                    row[x] = 'Q';
                } else if (kw == 1) {
                    row[x] = 'K';
                } else if (pb == 1) {
                    row[x] = 'p';
                } else if (nb == 1) {
                    row[x] = 'n';
                } else if (bb == 1) {
                    row[x] = 'b';
                } else if (rb == 1) {
                    row[x] = 'r';
                } else if (qb == 1) {
                    row[x] = 'q';
                } else if (kb == 1) {
                    row[x] = 'k';
                } else {
                    row[x] = ' ';
                }
            }
        }
        printf("%s", row);
        printf("\n");
    }
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
