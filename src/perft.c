#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <time.h>
#include "board.h"
#include "fen.h"
#include "movegen.h"

typedef unsigned long long u64;
u64 perft(Board board, int depth, bool divide);

char* FENS[250] = {
  START_FEN,
  "r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq",
  "8/2p5/3p4/KP5r/1R3p1k/8/4P1P1/8 w",
  "r3k2r/Pppp1ppp/1b3nbN/nP6/BBP1P3/q4N2/Pp1P2PP/R2Q1RK1 w kq - 0 1",
  "rnbq1k1r/pp1Pbppp/2p5/8/2B5/8/PPP1NnPP/RNBQK2R w KQ - 1 8  ",
  "r4rk1/1pp1qppp/p1np1n2/2b1p1B1/2B1P1b1/P1NP1N2/1PP1QPPP/R4RK1 w",
  "8/3k4/8/4b3/1p1p1p2/8/1PPRP2K/8 w - -",
  "8/4rP2/8/8/4pk2/8/3P2PP/4K2R w K -"
};
int NUM_FENS = 8;

// Starting from depth 1
u64 EXPECTED_RESULTS[][6] = {
  {20, 400 , 8902 , 197281 , 4865609  , 119060324  },
  {48, 2039, 97862, 4085603, 193690690, 8031647685 },
  {14, 191 , 2812 , 43238  , 674624   , 11030083   },
  {6 , 264 , 9467 , 422333 , 15833292 , 706045033  },
  {44, 1486, 62379, 2103487, 89941194 , 3048196529 },
  {46, 2079, 89890, 3894594, 164075551, 6923051137 },
  {12, 197 , 2696 , 41622  , 626367   , 9544506    },
  {17, 182 , 3232 , 42552  , 802123   , 11608216   },
};

int main() {
    initMoveGeneration();

    int depth = 5;
    printf("Depth %d\n\n", depth);


    clock_t start = clock();
    for (int i = 0; i < NUM_FENS;i++) {
      char* fen = FENS[i];

      Board board;
      setFen(&board, fen);

      u64 nodes = perft(board, depth, true);
      u64 expected = EXPECTED_RESULTS[i][depth-1];
      bool matches = nodes == expected;

      printf(matches ? 
      "Success: " : 
      "Fail:    ");

      printf("%llu/%llu %s\n", nodes, expected, fen);
    }

    // Print elapsed time
    clock_t end = clock();
    double time_spent = (double) (end - start) / CLOCKS_PER_SEC;
    printf("\nDone in: %.2fs\n", time_spent);

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

    u64 count = perft(cpy, depth-1, false);
    if (divide) {
      char san[6];
      memset(&san, 0, sizeof(san));
      moveToSan(moves[i], san);
      printf("%s: %llu\n", san, count);
    }
    nodes += count;
  }

  return nodes;
}
