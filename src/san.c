#include <stdlib.h>
#include <string.h>
#include "san.h"
#include "board.h"

void sanToMove(Board board, Move* move, char* san);
void moveToSan(Move move, char san[]);


void pushSan(Board* board, char* san) {
    Move move;
    sanToMove(*board, &move, san);
    pushMove(board, move);
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


    // Calculate what piece moved
    for (int i = 0; i < 12; i++) {
        Bitboard bb = *(&board.pawn_W+i);
        if (getBit(bb, move->fromSquare)) {
            move->pieceType = i;
            break;
        }
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