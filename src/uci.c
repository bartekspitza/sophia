#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "board.h"

Board board;

void parsePosition(char *command) {
    // shift pointer to the right where next token begins
    command += 9;
    
    char *current_char = command;
    
    if (strncmp(command, "startpos", 8) == 0) {
        setFen(&board, START_FEN);
    
    } else {
        current_char = strstr(command, "fen");
        
        if (current_char == NULL) {
            setFen(&board, START_FEN);
        } else {
            current_char += 4;
            setFen(&board, current_char);
        }
    }
    
    // parse moves after position
    current_char = strstr(command, "moves");
    
    // moves available
    if (current_char != NULL)
    {
        // shift pointer to the right where next token begins
        current_char += 6;
        
        // loop over moves within a move string
        while(*current_char)
        {
            // parse next move
            Move move; 
            sanToMove(board, &move, current_char);
            
            
            // make move on the chess board
            pushMove(&board, move);
            
            // move current character mointer to the end of current move
            while (*current_char && *current_char != ' ') current_char++;
            
            // go to the next move
            current_char++;
        }        
    }
}

void getBestMove(void) {
    Move moves[250];
    int length = legalMoves(board, moves);

    Move best = moves[rand() % length];

    char san[6];
    moveToSan(best, san);
    printf("bestmove %s\n", san);
}

void engineInfo(void) {
    printf("id name Sohia\n");
    printf("id author Bartek Spitza\n");
    printf("uciok\n");
}

int main(void) {
    setbuf(stdin, NULL);
    setbuf(stdout, NULL);

    int bufferSize = 2000;
    char input[bufferSize];


    initMoveGeneration();
    srand(time(NULL));

    while(1) {

        memset(input, 0, sizeof(input));
        fflush(stdout);

        if (!fgets(input, bufferSize, stdin))
            continue;
        
        if (input[0] == '\n')
            continue;
        
        if (strncmp(input, "isready", 7) == 0) {
            printf("readyok\n");
            continue;
        }

        else if (strncmp(input, "position", 8) == 0)
            parsePosition(input);
        
        else if (strncmp(input, "ucinewgame", 10) == 0)
            parsePosition("position startpos");

        else if (strncmp(input, "go", 2) == 0)
            getBestMove();
        
        else if (strncmp(input, "quit", 4) == 0)
            break;
        
        else if (strncmp(input, "uci", 3) == 0) {
            engineInfo();
        }
    }

    return 0;
}