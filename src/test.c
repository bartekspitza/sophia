#include "../libs/unity/unity.h"
#include "evaluation.h"
#include "board.h"
#include "fen.h"
#include "movegen.h"
#include "utils.h"
#include "san.h"
#include "bitboards.h"
#include "zobrist.h"

void setUp(void);
void tearDown(void);

/*-------Tests--------*/

void parseFen_turn(void);
void parseFen_pieceMasks(void);
void parseFen_epSquare(void);
void parseFen_kingSquares(void);
void parseFen_castlingRights(void);
void parseFen_hash(void);

void moveToSan_normal(void);
void moveToSan_castle(void);
void moveToSan_promotion(void);

void sanToMove_normal(void);
void sanToMove_castle(void);
void sanToMove_promotion(void);
void sanToMove_pieceType(void);

void pushMove_hash_noCapture(void);
void pushMove_hash_capture(void);
void pushMove_hash_moveThatSetsEpSquare(void);
void pushMove_hash_moveThatResetsEpSquare(void);
void pushMove_hash_enPassantMove(void);
void pushMove_hash_castlingMoveWhiteKing(void);
void pushMove_hash_castlingMoveWhiteQueen(void);
void pushMove_hash_castlingMoveBlackKing(void);
void pushMove_hash_castlingMoveBlackQueen(void);
void pushMove_hash_kingMoveThatRemovesCastlingRights(void);
void pushMove_hash_rookMoveThatRemovesCastlingRights(void);
void pushMove_hash_castlingMoveWhenEpSquareIsSet(void);
void pushMove_hash_promotionMove(void);
void pushMove_hash_fullSearchCheck(void);

void gameResult_blackCheckmate(void);
void gameResult_whiteCheckmate(void);
void gameResult_blackStalemate(void);
void gameResult_whiteStalemate(void);
void gameResult_insufficientMaterial_KN(void);
void gameResult_insufficientMaterial_KNN(void);
void gameResult_insufficientMaterial_KB(void);
void gameResult_undetermined(void);

/*-------Tests--------*/

int main(void) {
    initBitboards();
    initMoveGeneration();
    initZobrist();

    UNITY_BEGIN();
    RUN_TEST(parseFen_turn);
    RUN_TEST(parseFen_pieceMasks);
    RUN_TEST(parseFen_epSquare);
    RUN_TEST(parseFen_kingSquares);
    RUN_TEST(parseFen_castlingRights);
    RUN_TEST(parseFen_hash);

    RUN_TEST(moveToSan_normal);
    RUN_TEST(moveToSan_castle);
    RUN_TEST(moveToSan_promotion);

    RUN_TEST(sanToMove_normal);
    RUN_TEST(sanToMove_castle);
    RUN_TEST(sanToMove_promotion);
    RUN_TEST(sanToMove_pieceType);

    RUN_TEST(pushMove_hash_noCapture);
    RUN_TEST(pushMove_hash_capture);
    RUN_TEST(pushMove_hash_moveThatSetsEpSquare);
    RUN_TEST(pushMove_hash_moveThatResetsEpSquare);
    RUN_TEST(pushMove_hash_enPassantMove);
    RUN_TEST(pushMove_hash_castlingMoveWhiteKing);
    RUN_TEST(pushMove_hash_castlingMoveWhiteQueen);
    RUN_TEST(pushMove_hash_castlingMoveBlackKing);
    RUN_TEST(pushMove_hash_castlingMoveBlackQueen);
    RUN_TEST(pushMove_hash_kingMoveThatRemovesCastlingRights);
    RUN_TEST(pushMove_hash_rookMoveThatRemovesCastlingRights);
    RUN_TEST(pushMove_hash_castlingMoveWhenEpSquareIsSet);
    RUN_TEST(pushMove_hash_promotionMove);
    RUN_TEST(pushMove_hash_fullSearchCheck);

    RUN_TEST(gameResult_blackCheckmate);
    RUN_TEST(gameResult_blackStalemate);
    RUN_TEST(gameResult_whiteCheckmate);
    RUN_TEST(gameResult_whiteStalemate);
    RUN_TEST(gameResult_insufficientMaterial_KN);
    RUN_TEST(gameResult_insufficientMaterial_KNN);
    RUN_TEST(gameResult_insufficientMaterial_KB);
    RUN_TEST(gameResult_undetermined);
    return UNITY_END();
}


void zobristSearch(Board board, int depth) {
    Bitboard correctHash = hash(board);
    TEST_ASSERT_EQUAL_UINT64(correctHash, board.hash);
    if (depth == 0) return;

    Move moves[256];
    int numMoves = legalMoves(&board, moves);

    for (int i = 0; i < numMoves; i++) {
        if (moves[i].validation == LEGAL) {
            Board cpy = board;
            pushMove(&cpy, moves[i]);
            zobristSearch(cpy, depth-1);

        }
    }
}
void pushMove_hash_fullSearchCheck(void) {
    Board board;

    setFen(&board, START_FEN);
    zobristSearch(board, 4);

    setFen(&board, "r3k2r/Pppp1ppp/1b3nbN/nP6/BBP1P3/q4N2/Pp1P2PP/R2Q1RK1 w kq - 0 1");
    zobristSearch(board, 4);

    setFen(&board, "rnbq1k1r/pp1Pbppp/2p5/8/2B5/8/PPP1NnPP/RNBQK2R w KQ");
    zobristSearch(board, 4);

    setFen(&board, "r4rk1/1pp1qppp/p1np1n2/2b1p1B1/2B1P1b1/P1NP1N2/1PP1QPPP/R4RK1 w - - 0 1");
    zobristSearch(board, 4);
}

void pushMove_hash_promotionMove(void) {
    Board board;
    setFen(&board, "r3k2r/Pppp1ppp/1b3nbN/nP6/BBP1P3/q4N2/Pp1P2PP/R2Q1RK1 b kq - 0 1");
    pushSan(&board, "b2a1q");

    Bitboard correctHash = hash(board);
    TEST_ASSERT_EQUAL_UINT64(correctHash, board.hash);
}

void pushMove_hash_castlingMoveWhenEpSquareIsSet(void) {
    Board board;
    setFen(&board, "r3k2r/8/8/8/3P4/8/8/4K3 b kq d3 0 1");
    pushSan(&board, "e8g8");

    Bitboard correctHash = hash(board);
    TEST_ASSERT_EQUAL_UINT64(correctHash, board.hash);
}

void pushMove_hash_kingMoveThatRemovesCastlingRights(void) {
    Board board;
    setFen(&board, "4k3/8/8/8/8/8/8/R3K2R w KQ - 0 1");
    pushSan(&board, "h1h2");

    Bitboard correctHash = hash(board);
    TEST_ASSERT_EQUAL_UINT64(correctHash, board.hash);
}

void pushMove_hash_rookMoveThatRemovesCastlingRights(void) {
    Board board;
    setFen(&board, "4k3/8/8/8/8/8/8/R3K2R w KQ - 0 1");
    pushSan(&board, "a1a2");

    Bitboard correctHash = hash(board);
    TEST_ASSERT_EQUAL_UINT64(correctHash, board.hash);
}

void pushMove_hash_castlingMoveWhiteQueen(void) {
    Board board;
    setFen(&board, "4k3/8/8/8/8/8/8/R3K2R w KQ - 0 1");
    pushSan(&board, "e1c1");
    
    Bitboard correctHash = hash(board);
    TEST_ASSERT_EQUAL_UINT64(correctHash, board.hash);
}
void pushMove_hash_castlingMoveBlackKing(void) {
    Board board;
    setFen(&board, "r3k2r/8/8/8/8/8/8/4K3 b kq - 0 1");
    pushSan(&board, "e8g8");

    Bitboard correctHash = hash(board);
    TEST_ASSERT_EQUAL_UINT64(correctHash, board.hash);
}

void pushMove_hash_castlingMoveBlackQueen(void) {
    Board board;
    setFen(&board, "r3k2r/8/8/8/8/8/8/4K3 b kq - 0 1");
    pushSan(&board, "e8c8");

    Bitboard correctHash = hash(board);
    TEST_ASSERT_EQUAL_UINT64(correctHash, board.hash);
}

void pushMove_hash_castlingMoveWhiteKing(void) {
    Board board;
    setFen(&board, "4k3/8/8/8/8/8/8/R3K2R w KQ - 0 1");
    pushSan(&board, "e1g1");
    
    Bitboard correctHash = hash(board);
    TEST_ASSERT_EQUAL_UINT64(correctHash, board.hash);
}

void pushMove_hash_enPassantMove(void) {
    Board board;
    setFen(&board, START_FEN);
    pushSan(&board, "e2e4");
    pushSan(&board, "a7a6");
    pushSan(&board, "e4e5");
    pushSan(&board, "d7d5");
    pushSan(&board, "e5d6");

    Bitboard correctHash = hash(board);
    TEST_ASSERT_EQUAL_UINT64(correctHash, board.hash);
}

void pushMove_hash_moveThatResetsEpSquare(void) {
    Board board;
    setFen(&board, START_FEN);
    pushSan(&board, "e2e4");
    pushSan(&board, "e7e6");

    Bitboard correctHash = hash(board);
    TEST_ASSERT_EQUAL_UINT64(correctHash, board.hash);
}

void pushMove_hash_moveThatSetsEpSquare(void) {
    Board board;
    setFen(&board, START_FEN);
    pushSan(&board, "e2e4");

    Bitboard correctHash = hash(board);
    TEST_ASSERT_EQUAL_UINT64(correctHash, board.hash);
}
void pushMove_hash_noCapture(void) {
    Board board;
    setFen(&board, START_FEN);
    pushSan(&board, "e2e3");
    Bitboard correctHash = hash(board);

    TEST_ASSERT_EQUAL_UINT64(correctHash, board.hash);
}

void pushMove_hash_capture(void) {
    Board board;
    setFen(&board, START_FEN);
    pushSan(&board, "e2e7");
    Bitboard correctHash = hash(board);
    TEST_ASSERT_EQUAL_UINT64(correctHash, board.hash);
}

void gameResult_undetermined(void) {
    Board board;
    char* fen = "8/4bb1k/8/8/8/4K3/8/8 w - - 0 1";
    setFen(&board, fen);

    Move moves[250];
    int length = legalMoves(&board, moves);
    int gameResult = result(board, moves, length);
    TEST_ASSERT_EQUAL_INT16(UN_DETERMINED, gameResult);
}

void gameResult_insufficientMaterial_KB(void) {
    Board board;
    char* fen = "8/5b1k/8/8/8/8/K7/8 w - - 0 1";
    setFen(&board, fen);

    Move moves[250];
    int length = legalMoves(&board, moves);
    int gameResult = result(board, moves, length);
    TEST_ASSERT_EQUAL_INT16(DRAW, gameResult);
}
void gameResult_insufficientMaterial_KNN(void) {
    Board board;
    char* fen = "8/4nn1k/8/8/8/8/K7/8 w - - 0 1";
    setFen(&board, fen);

    Move moves[250];
    int length = legalMoves(&board, moves);
    int gameResult = result(board, moves, length);
    TEST_ASSERT_EQUAL_INT16(DRAW, gameResult);
}

void gameResult_insufficientMaterial_KN(void) {
    Board board;
    char* fen = "8/5n1k/8/8/8/8/K7/8 w - - 0 1";
    setFen(&board, fen);

    Move moves[250];
    int length = legalMoves(&board, moves);
    int gameResult = result(board, moves, length);
    TEST_ASSERT_EQUAL_INT16(DRAW, gameResult);
}

void gameResult_whiteStalemate(void) {
    Board board;
    char* fen = "1r6/7k/8/8/8/2q5/K7/8 w - - 0 1";
    setFen(&board, fen);

    Move moves[250];
    int length = legalMoves(&board, moves);
    int gameResult = result(board, moves, length);
    TEST_ASSERT_EQUAL_INT16(DRAW, gameResult);
}

void gameResult_blackStalemate(void) {
    Board board;
    char* fen = "8/7k/5Q2/8/8/8/8/K5R1 b - - 0 1";
    setFen(&board, fen);

    Move moves[250];
    int length = legalMoves(&board, moves);
    int gameResult = result(board, moves, length);
    TEST_ASSERT_EQUAL_INT16(DRAW, gameResult);
}

void gameResult_blackCheckmate(void) {
    Board board;
    char* fen = "1r2k3/8/8/8/8/8/1q6/K7 w - - 0 1";
    setFen(&board, fen);

    Move moves[250];
    int length = legalMoves(&board, moves);
    int gameResult = result(board, moves, length);
    TEST_ASSERT_EQUAL_INT16(BLACK_WIN, gameResult);
}

void gameResult_whiteCheckmate(void) {
    Board board;
    char* fen = "4k3/4Q3/8/8/8/8/8/K3R3 b - - 0 1";
    setFen(&board, fen);

    Move moves[250];
    int length = legalMoves(&board, moves);
    int gameResult = result(board, moves, length);
    TEST_ASSERT_EQUAL_INT16(WHITE_WIN, gameResult);
}

void sanToMove_pieceType(void) {
    Board board;
    setFen(&board, "rnbqk3/6p1/8/8/8/8/6P1/RNBQK3 w Qq - 0 1");
    Move move;

    // White moves
    sanToMove(board, &move, "g2g3");
    TEST_ASSERT_EQUAL_INT16(PAWN_W, move.pieceType);
    sanToMove(board, &move, "e1e2");
    TEST_ASSERT_EQUAL_INT16(KING_W, move.pieceType);
    sanToMove(board, &move, "d1d2");
    TEST_ASSERT_EQUAL_INT16(QUEEN_W, move.pieceType);
    sanToMove(board, &move, "c1d2");
    TEST_ASSERT_EQUAL_INT16(BISHOP_W, move.pieceType);
    sanToMove(board, &move, "b1c3");
    TEST_ASSERT_EQUAL_INT16(KNIGHT_W, move.pieceType);
    sanToMove(board, &move, "a1a2");
    TEST_ASSERT_EQUAL_INT16(ROOK_W, move.pieceType);

    // Black moves
    board.turn = BLACK;
    sanToMove(board, &move, "g7g7");
    TEST_ASSERT_EQUAL_INT16(PAWN_B, move.pieceType);
    sanToMove(board, &move, "e8e7");
    TEST_ASSERT_EQUAL_INT16(KING_B, move.pieceType);
    sanToMove(board, &move, "d8d7");
    TEST_ASSERT_EQUAL_INT16(QUEEN_B, move.pieceType);
    sanToMove(board, &move, "c8d7");
    TEST_ASSERT_EQUAL_INT16(BISHOP_B, move.pieceType);
    sanToMove(board, &move, "b8c6");
    TEST_ASSERT_EQUAL_INT16(KNIGHT_B, move.pieceType);
    sanToMove(board, &move, "a8a7");
    TEST_ASSERT_EQUAL_INT16(ROOK_B, move.pieceType);
}

void sanToMove_castle(void) {
    Board board;
    board.castling = K | Q | k | q;
    Move move;

    sanToMove(board, &move, "e1g1");
    TEST_ASSERT_EQUAL_INT16(K, move.castle);
    sanToMove(board, &move, "e1c1");
    TEST_ASSERT_EQUAL_INT16(Q, move.castle);
    sanToMove(board, &move, "e8g8");
    TEST_ASSERT_EQUAL_INT16(k, move.castle);
    sanToMove(board, &move, "e8c8");
    TEST_ASSERT_EQUAL_INT16(q, move.castle);
}

void sanToMove_promotion(void) {
    Board board;
    board.turn = WHITE;
    Move move;
    sanToMove(board, &move, "e7e8q");

    TEST_ASSERT_EQUAL_INT16(E7, move.fromSquare);
    TEST_ASSERT_EQUAL_INT16(E8, move.toSquare);
    TEST_ASSERT_EQUAL_INT16(QUEEN_W, move.promotion);
}

void sanToMove_normal(void) {
    Board board;
    Move move;
    sanToMove(board, &move, "e2e3");

    TEST_ASSERT_EQUAL_INT16(E2, move.fromSquare);
    TEST_ASSERT_EQUAL_INT16(E3, move.toSquare);
}

void moveToSan_normal(void) {
    Move move = {.fromSquare=E2, .toSquare=E3};
    char san[6];
    moveToSan(move, san);
    TEST_ASSERT_EQUAL_STRING("e2e3", san);
}

void moveToSan_castle(void) {
    Move move = {.fromSquare=0, .toSquare=0, .castle=K};
    char san[6];
    moveToSan(move, san);
    TEST_ASSERT_EQUAL_STRING("e1g1", san);
}

void moveToSan_promotion(void) {
    Move move = {.fromSquare=E7, .toSquare=E8, .promotion=QUEEN_W};
    char san[6];
    moveToSan(move, san);
    TEST_ASSERT_EQUAL_STRING("e7e8q", san);
}

void parseFen_castlingRights(void) {
    Board board;
    char* fen = "r3k3/8/8/8/8/8/8/4K2R b Kq d3 0 1";
    setFen(&board, fen);

    TEST_ASSERT_EQUAL_INT16(K | q, board.castling);
}

void parseFen_kingSquares(void) {
    Board board;
    char* fen = "8/8/4k3/8/8/3K4/8/8 b - d3 0 1";
    setFen(&board, fen);

    TEST_ASSERT_EQUAL_INT16(D3, board.whiteKingSq);
    TEST_ASSERT_EQUAL_INT16(E6, board.blackKingSq);
}

void parseFen_turn(void) {
    Board board;
    char* fen = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR b KQkq - 0 1";
    setFen(&board, fen);

    TEST_ASSERT_TRUE(board.turn == BLACK);
}

void parseFen_hash(void) {
    Board board;
    setFen(&board, START_FEN);
    Bitboard zob = hash(board);

    TEST_ASSERT_EQUAL_UINT64(zob, board.hash);
}

void parseFen_pieceMasks(void) {
    Board board;
    char* fen = "r3k2r/Pp1p1ppp/1b3nbN/nP6/BBpPP3/q4N2/Pp4PP/R2Q1RK1 b kq d3 0 1";
    setFen(&board, fen);

    Bitboard correctPieceMasks[] = {
        36029072299557632, 1099511889920, 3221225472, 132, 16, 2, 
        24488323510714368, 4947802324992, 72567767433216, 9295429630892703744U, 8388608, 576460752303423488
    };

    // Compare piece masks
    for (int i = 0;i  < 12;i ++) {
        Bitboard pieceMask = *(&(board.pawn_W)+i);
        Bitboard expected = correctPieceMasks[i];
        TEST_ASSERT_EQUAL_UINT64(expected, pieceMask);
    }
}

void parseFen_epSquare(void) {
    Board board;
    char* fen = "r3k2r/Pp1p1ppp/1b3nbN/nP6/BBpPP3/q4N2/Pp4PP/R2Q1RK1 b kq d3 0 1";
    setFen(&board, fen);

    TEST_ASSERT_EQUAL_INT16(D3, board.epSquare);
}



// Unity functions
void setUp(void) {}
void tearDown(void) {}
