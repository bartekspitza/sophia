#include <stdlib.h>
#include "fen.h"
#include "board.h"

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

void setFen(Board* board, char* fen) {
    reset(board);

	int rank = 7;
    int file = 0;
    int piece = -1;
    int turn;
    int spacesEncountered = 0;
    char enPassantFile;
    char enPassantRank;
	while (*fen) {
        piece = -1;

		switch (*fen) {
            case 'b': 
                piece = BISHOP_B; 
                turn = BLACK;
                enPassantFile = *fen;
                break;
            case 'w':
                turn = WHITE;
                break;
            case 'p': piece = PAWN_B; break;
            case 'n': piece = KNIGHT_B; break;
            case 'r': piece = ROOK_B; break;
            case 'q': piece = QUEEN_B; break;
            case 'k': piece = KING_B; break;
            case 'P': piece = PAWN_W; break;
            case 'N': piece = KNIGHT_W; break;
            case 'B': piece = BISHOP_W; break;
            case 'R': piece = ROOK_W; break;
            case 'Q': piece = QUEEN_W; break;
            case 'K': piece = KING_W; break;

            case 'a':
            case 'c':
            case 'd':
            case 'e':
            case 'f':
            case 'g':
            case 'h': 
                enPassantFile = *fen; 
                break;

            case '1':
            case '2':
            case '3':
            case '4':
            case '5':
            case '6':
            case '7':
            case '8':
                enPassantRank = *fen;
                file += atoi(fen);
                break;

            case '/':
                rank--;
                file = 0;
                fen++;
                continue;
            case ' ':
                spacesEncountered++;
                enPassantFile = '0';
                enPassantRank = '0';
                fen++;
                continue;
            case '-':
                fen++;
                continue;
        }

        // Pieces
        if (spacesEncountered == 0) {
            if (piece != -1 && rank >= 0) {
                int square = ((rank+1) * 8) - file - 1;
                file++;

                Bitboard* bb = pieceBitboard(board, piece);
                *bb |= 1LL << square;
            }

        // Turn
        } else if (spacesEncountered == 1) {
            board->turn = turn;

        // Castling rights
        } else if (spacesEncountered == 2) {
            if (piece == KING_W) board->castling |= 1;
            else if (piece == QUEEN_W) board->castling |= 1 << 1;
            else if (piece == KING_B) board->castling |= 1 << 2;
            else if (piece == QUEEN_B) board->castling |= 1 << 3;
        
        // En passant square
        } else if (spacesEncountered == 3) {
            if (enPassantRank != '0' && enPassantFile != '0') {
                board->epSquare = ((8 - ('h' - enPassantFile)+1) * 8) - (7 - ('8' - enPassantRank));
            }
        }

        // TODO: Half- and fullmove clock
		
		fen++;
	}

    for (int i = 0; i < 64; i++) {
        if (getBit(board->king_W, i)) {
            board->whiteKingSq = i;
        }
        if (getBit(board->king_B, i)) {
            board->blackKingSq = i;
        }
    }
}