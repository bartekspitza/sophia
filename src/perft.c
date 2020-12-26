#include <stdio.h>
#include <stdbool.h>
#include "board.h"

typedef unsigned long long u64;


u64 perft(Board board, int depth, bool divide);

int main() {
    initMoveGenerationTables();
    Board board;
    setFen(&board, START_FEN);

    int depth = 6;
    printf("Perft %d\n", depth);
    u64 nodes = perft(board, depth, true);
    printf("\nNodes searched: %llu\n", nodes);

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