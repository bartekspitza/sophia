#include "board.h"

int main() {
    Board board;
    setFen(&board, "rnbqkbnr/ppp2ppp/8/3ppP2/8/1NN3P1/8/4K3 w kq e6 0 4");
    printBoard(board);

    return 0;
}