#include <stdbool.h>
#include <inttypes.h>
#include <stdio.h>
#include <stdlib.h>
#include "board.h"
#include "movegen.h"

#define TWO_RANKS 16

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

int ALL_CASTLE_W = 0b0011;
int ALL_CASTLE_B = 0b1100;

bool isInsufficientMaterial(Board board) {
    if (board.pawn_W || board.pawn_B || board.rook_W || board.rook_B || board.queen_W || board.queen_B) {
        return false;
    }

    int knights = 0;
    int bishops = 0;
    Bitboard knightsBB = board.knight_W | board.knight_B;
    Bitboard bishopsBB = board.bishop_W | board.bishop_B;
    while (knightsBB) {
        knightsBB &= knightsBB - 1;
        knights++;
    }
    while (bishopsBB) {
        bishopsBB &= bishopsBB - 1;
        bishops++;
    }

    if (!bishops && knights) return true;       // KN or KNN vs K
    if (!knights && bishops == 1) return true;  // KB vs K

    return false;
}

int result(Board board, Move pseudoLegal[], int length) {
    if (isInsufficientMaterial(board)) {
        return DRAW;
    }

    bool noLegalMoves = true;

    for (int i = 0; i < length;i++) {
        if (pseudoLegal[i].validation == NOT_VALIDATED) 
            validateMove(board, &(pseudoLegal[i]));

        if (pseudoLegal[i].validation == LEGAL) {
            noLegalMoves = false;
            break;
        }
    }

    if (noLegalMoves) {
        bool kingInCheck = isSquareAttacked(board, board.turn ? board.whiteKingSq : board.blackKingSq);
        if (kingInCheck) {
            return board.turn ? BLACK_WIN : WHITE_WIN;
        }

        return DRAW;
    }

    return UN_DETERMINED;
}

void computeOccupancyMasks(Board* board) {
    board->occupancyWhite = board->pawn_W | board->knight_W | board->bishop_W | board->rook_W | board->queen_W | board->king_W;
    board->occupancyBlack = board->pawn_B | board->knight_B | board->bishop_B | board->rook_B | board->queen_B | board->king_B;
    board->occupancy = board->occupancyBlack | board->occupancyWhite;
}

void makeEnPassantMove(Board* board, Move move) {
    int capturedSquare = board->epSquare + (board->turn ? -8 : 8);

    Bitboard* friendlyPawns = board->turn ? &(board->pawn_W) : &(board->pawn_B);
    Bitboard* opponentPawns = board->turn ? &(board->pawn_B) : &(board->pawn_W);

    // Capture the pawn
    *opponentPawns = toggleBit(*opponentPawns, capturedSquare);

    // Move the pawn that takes
    *friendlyPawns = toggleBit(*friendlyPawns, move.fromSquare);
    *friendlyPawns = setBit(*friendlyPawns, board->epSquare);

    board->epSquare = -1;
}

void makeCastleMove(Board* board, Move move) {
    if (move.castle == K) {
        board->king_W = board->king_W >> 2;
        board->rook_W = toggleBit(board->rook_W, H1);
        board->rook_W = setBit(board->rook_W, F1);
        board->whiteKingSq = G1;
    } else if (move.castle == Q) {
        board->king_W = board->king_W << 2;
        board->rook_W = toggleBit(board->rook_W, A1);
        board->rook_W = setBit(board->rook_W, D1);
        board->whiteKingSq = C1;
    } else if (move.castle == k) {
        board->king_B = board->king_B >> 2;
        board->rook_B = toggleBit(board->rook_B, H8);
        board->rook_B = setBit(board->rook_B, F8);
        board->blackKingSq = G8;
    } else if (move.castle == q) {
        board->king_B = board->king_B << 2;
        board->rook_B = toggleBit(board->rook_B, A8);
        board->rook_B = setBit(board->rook_B, D8);
        board->blackKingSq = C8;
    }

    // Update castling rights
    board->castling &= board->turn ? ALL_CASTLE_B : ALL_CASTLE_W;
}

void pushMove(Board* board, Move move) {
    // Make en passant move
    if (move.toSquare == board->epSquare) {
        bool isPawnMove = move.pieceType == PAWN_W || move.pieceType == PAWN_B;

        if (isPawnMove) {
            makeEnPassantMove(board, move);
            computeOccupancyMasks(board);
            board->turn = board->turn ? 0 : 1;
            return;
        }
    } 
    
    // Make castle move
    if (move.castle) {
        makeCastleMove(board, move); 
        computeOccupancyMasks(board);
        board->epSquare = -1;
        board->turn = board->turn ? 0 : 1;
        return;
    }

    // Reset ep-square
    board->epSquare = -1;

    // Set potential ep-square
    bool starterPawnMoved = (move.pieceType == PAWN_W && (move.fromSquare > A1 && move.fromSquare < H3)) ||
                            (move.pieceType == PAWN_B && (move.fromSquare > A6 && move.fromSquare < H8));
    if (starterPawnMoved) {
        int distanceCovered = abs(move.fromSquare - move.toSquare);
        if (distanceCovered == TWO_RANKS) {
            board->epSquare = board->turn ? move.fromSquare + 8 : move.fromSquare - 8;
        }
    }

    // Make move
    Bitboard friendlyRooks = board->turn ? board->rook_W : board->rook_B;
    Bitboard* pieceThatMoved = (&board->pawn_W + move.pieceType);

    // Update castling rights
    if (move.pieceType == KING_W || move.pieceType == KING_B) {
        board->castling &= board->turn ? ALL_CASTLE_B : ALL_CASTLE_W;
    } else if (*pieceThatMoved == friendlyRooks) {
        if (board->turn && move.fromSquare == A1) board->castling &= 0b1101;
        if (board->turn && move.fromSquare == H1) board->castling &= 0b1110;
        if (!board->turn && move.fromSquare == A8) board->castling &= 0b0111;
        if (!board->turn && move.fromSquare == H8) board->castling &= 0b1011;
    }

    // "Lift up the piece"
    *pieceThatMoved = toggleBit(*pieceThatMoved, move.fromSquare);

    // If not promotion set the piece square directly
    if (move.promotion <= 0) {
        *pieceThatMoved = setBit(*pieceThatMoved, move.toSquare);

        if (*pieceThatMoved == board->king_W) {
            board->whiteKingSq = move.toSquare;
        } else if (*pieceThatMoved == board->king_B) {
            board->blackKingSq = move.toSquare;
        }
    } else {
        Bitboard* targetMask = &(board->pawn_W) + move.promotion;
        *targetMask = setBit(*targetMask, move.toSquare);
    }

    Bitboard* bb = board->turn ? &(board->pawn_B) : &(board->pawn_W);
    Bitboard opponentRooks = board->turn ? board->rook_B : board->rook_W;
    for (int i = 0; i < 5; i++) {

        if (getBit(*bb, move.toSquare)) {

            // Update castling rights if rooks are captured
            if (*bb == opponentRooks) {
                if (board->turn && move.toSquare == H8) board->castling &= 0b1011;
                if (board->turn && move.toSquare == A8) board->castling &= 0b0111;
                if (!board->turn && move.toSquare == A1) board->castling &= 0b1101;
                if (!board->turn && move.toSquare == H1) board->castling &= 0b1110;
            }

            // Remove captured piece
            *bb = toggleBit(*bb, move.toSquare);
        }
        ++bb;
    }

    // Toggle turn
    board->turn = board->turn ? 0 : 1;
    computeOccupancyMasks(board);
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
