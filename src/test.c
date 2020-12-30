#include "../libs/unity/unity.h"
#include "evaluation.h"
#include "board.h"
#include "fen.h"
#include "movegen.h"
#include "search.h"
#include "utils.h"

void setUp(void);
void tearDown(void);

/*-------Tests--------*/

void parseFen_turn(void);
void parseFen_pieceMasks(void);
void parseFen_epSquare(void);
void parseFen_kingSquares(void);
void parseFen_castlingRights(void);

void moveToSan_normal(void);
void moveToSan_castle(void);
void moveToSan_promotion(void);

void sanToMove_normal(void);
void sanToMove_castle(void);
void sanToMove_promotion(void);

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
    initMoveGeneration();

    UNITY_BEGIN();
    RUN_TEST(parseFen_turn);
    RUN_TEST(parseFen_pieceMasks);
    RUN_TEST(parseFen_epSquare);
    RUN_TEST(parseFen_kingSquares);
    RUN_TEST(parseFen_castlingRights);

    RUN_TEST(moveToSan_normal);
    RUN_TEST(moveToSan_castle);
    RUN_TEST(moveToSan_promotion);

    RUN_TEST(sanToMove_normal);
    RUN_TEST(sanToMove_castle);
    RUN_TEST(sanToMove_promotion);

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

void gameResult_undetermined(void) {
    Board board;
    char* fen = "8/4bb1k/8/8/8/4K3/8/8 w - - 0 1";
    setFen(&board, fen);

    Move moves[250];
    int length = legalMoves(board, moves);
    int gameResult = result(board, length);
    TEST_ASSERT_EQUAL_INT16(UN_DETERMINED, gameResult);
}

void gameResult_insufficientMaterial_KB(void) {
    Board board;
    char* fen = "8/5b1k/8/8/8/8/K7/8 w - - 0 1";
    setFen(&board, fen);

    Move moves[250];
    int length = legalMoves(board, moves);
    int gameResult = result(board, length);
    TEST_ASSERT_EQUAL_INT16(DRAW, gameResult);
}
void gameResult_insufficientMaterial_KNN(void) {
    Board board;
    char* fen = "8/4nn1k/8/8/8/8/K7/8 w - - 0 1";
    setFen(&board, fen);

    Move moves[250];
    int length = legalMoves(board, moves);
    int gameResult = result(board, length);
    TEST_ASSERT_EQUAL_INT16(DRAW, gameResult);
}

void gameResult_insufficientMaterial_KN(void) {
    Board board;
    char* fen = "8/5n1k/8/8/8/8/K7/8 w - - 0 1";
    setFen(&board, fen);

    Move moves[250];
    int length = legalMoves(board, moves);
    int gameResult = result(board, length);
    TEST_ASSERT_EQUAL_INT16(DRAW, gameResult);
}

void gameResult_whiteStalemate(void) {
    Board board;
    char* fen = "1r6/7k/8/8/8/2q5/K7/8 w - - 0 1";
    setFen(&board, fen);

    Move moves[250];
    int length = legalMoves(board, moves);
    int gameResult = result(board, length);
    TEST_ASSERT_EQUAL_INT16(DRAW, gameResult);
}

void gameResult_blackStalemate(void) {
    Board board;
    char* fen = "8/7k/5Q2/8/8/8/8/K5R1 b - - 0 1";
    setFen(&board, fen);

    Move moves[250];
    int length = legalMoves(board, moves);
    int gameResult = result(board, length);
    TEST_ASSERT_EQUAL_INT16(DRAW, gameResult);
}

void gameResult_blackCheckmate(void) {
    Board board;
    char* fen = "1r2k3/8/8/8/8/8/1q6/K7 w - - 0 1";
    setFen(&board, fen);

    Move moves[250];
    int length = legalMoves(board, moves);
    int gameResult = result(board, length);
    TEST_ASSERT_EQUAL_INT16(BLACK_WIN, gameResult);
}

void gameResult_whiteCheckmate(void) {
    Board board;
    char* fen = "4k3/4Q3/8/8/8/8/8/K3R3 b - - 0 1";
    setFen(&board, fen);

    Move moves[250];
    int length = legalMoves(board, moves);
    int gameResult = result(board, length);
    TEST_ASSERT_EQUAL_INT16(WHITE_WIN, gameResult);
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
