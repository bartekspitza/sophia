#include <stdbool.h>
#include <inttypes.h>
#include <stdio.h>
#include <stdlib.h>
#include "board.h"
#include "utils.h"

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

void pushSan(Board* board, char* san) {
    Move move;
    sanToMove(*board, &move, san);
    pushMove(board, move);
}

void pushMove(Board* board, Move move) {
    // En passant
    if (move.toSquare == board->epSquare) {
        if (board->turn) {
            int capturedSquare = board->epSquare -8;
            clearBit(&(board->pawn_B), capturedSquare);
        } else {
            int capturedSquare = board->epSquare + 8;
            clearBit(&(board->pawn_W), capturedSquare);
        }
        board->epSquare = -1;
        board->turn ^= 1;
        return;
    } 
    
    // Castle
    if (move.castle) {
        if (move.castle == K) {
            board->king_W = board->king_W >> 2;
            clearBit(&(board->rook_W), H1);
            setBit(&(board->rook_W), F1);
        } else if (move.castle == Q) {
            board->king_W = board->king_W << 2;
            clearBit(&(board->rook_W), A1);
            setBit(&(board->rook_W), D1);
        } else if (move.castle == k) {
            board->king_B = board->king_B >> 2;
            clearBit(&(board->rook_B), H8);
            setBit(&(board->rook_B), F8);
        } else if (move.castle == q) {
            board->king_B = board->king_B << 2;
            clearBit(&(board->rook_B), A8);
            setBit(&(board->rook_B), D8);
        }
        board->epSquare = -1;
        board->turn ^= 1;
        return;
    }

    // Ep square
    board->epSquare = -1;

    Bitboard from = 1LL << move.fromSquare;
    bool starterPawnMoved = from & (board->turn ? PAWN_START_WHITE : PAWN_START_BLACK);
    int distanceCovered = abs(move.fromSquare - move.toSquare);
    int twoRanks = 16;

    if (starterPawnMoved && distanceCovered == twoRanks) {
        board->epSquare = board->turn ? move.fromSquare + 8 : move.fromSquare - 8;
    }

    // Make move
    Bitboard* bb = board->turn ? &(board->pawn_W) : &(board->pawn_B);
    for (int i = 0; i < 6; i++) {
        if (getBit(*bb, move.fromSquare)) {
            clearBit(bb, move.fromSquare);
            setBit(bb, move.toSquare);
        } else if (getBit(*bb, move.toSquare)) {
            clearBit(bb, move.toSquare);
        }
        ++bb;
    }

    // Toggle turn
    board->turn ^= 1;
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
    printf("\nEp square: %s\n", board.epSquare == -1 ? "None" : &SQUARE_NAMES[board.epSquare][0]);
    printf("\n");
}
