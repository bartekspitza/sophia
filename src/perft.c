#include <stdio.h>
#include "board.h"

typedef unsigned long long u64;


u64 perft(Board board, int depth);

int main() {
    initMoveGenerationTables();
    Board board;
    setFen(&board, START_FEN);
    printBoard(board);

    u64 nodes = perft(board, 1);
    printf("Perft 1: %llu\n", nodes);
    nodes = perft(board, 2);
    printf("Perft 2: %llu\n", nodes);
    nodes = perft(board, 3);
    printf("Perft 3: %llu\n", nodes);
    nodes = perft(board, 4);
    printf("Perft 4: %llu\n", nodes);

    return 0;
}

u64 perft(Board board, int depth) {
  Move moves[256];
  int numMoves;
  u64 nodes = 0;

  if (depth == 0) 
    return 1ULL;

  numMoves = legalMoves(board, moves);
  for (int i = 0; i < numMoves; i++) {
    Board cpy = board;
    pushMove(&cpy, moves[i]);

    nodes += perft(cpy, depth - 1);
  }

  return nodes;
}