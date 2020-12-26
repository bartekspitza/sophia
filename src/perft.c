#include <stdio.h>
#include <stdbool.h>
#include "board.h"

typedef unsigned long long u64;
u64 perft(Board board, int depth, bool divide);

char* FENS[250] = {
  START_FEN,
  "r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq"
};
int NUM_FENS = 2;

// Starting from depth 1
u64 EXPECTED_RESULTS[][6] = {
  {20, 400 , 8902 , 197281 , 4865609  , 119060324  },
  {48, 2039, 97862, 4085603, 193690690, 8031647685 },
};


int main() {
    initMoveGenerationTables();

    int depth = 3;
    printf("Depth %d\n\n", depth);

    for (int i = 0; i < NUM_FENS;i++) {
      char* fen = FENS[i];

      Board board;
      setFen(&board, fen);

      u64 nodes = perft(board, depth, false);
      u64 expected = EXPECTED_RESULTS[i][depth-1];
      bool matches = nodes == expected;

      printf(matches ? 
      "Success: " : 
      "Fail:    ");

      printf("%llu/%llu %s\n", nodes, expected, fen);
    }

    return 0;
}

u64 perft(Board board, int depth, bool divide) {
  u64 nodes = 0;
  Move moves[256];
  int numMoves = legalMoves(board, moves);

  if (depth == 1) 
    return numMoves;

  for (int i = 0; i < numMoves; i++) {

    // Copy-make
    Board cpy = board;
    pushMove(&cpy, moves[i]);

    //nodes += perft(cpy, depth-1, false);

    u64 count = perft(cpy, depth-1, false);
    if (divide) {
      char san[6];
      moveToSan(moves[i], san);
      printf("%s: %llu\n", san, count);
    }
    nodes += count;

  }

  return nodes;
}