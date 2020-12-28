#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "board.h"
#include "fen.h"
#include "search.h"
#include "movegen.h"
#include "evaluation.h"

void parsePosition(char *command, Board* board);
void getBestMove(Board board);
void printEngineInfo(void);

char* AUTHOR = "Bartek Spitza";
char* ENGINE_NAME = "Sophia";

int main(void) {
    initMoveGeneration();
    initEvaluation();
    setbuf(stdin, NULL);
    setbuf(stdout, NULL);

    int bufferSize = 2000;
    char input[bufferSize];

    Board board;
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
            parsePosition(input, &board);

        else if (strncmp(input, "ucinewgame", 10) == 0)
            parsePosition("position startpos", &board);

        else if (strncmp(input, "go", 2) == 0)
            getBestMove(board);

        else if (strncmp(input, "quit", 4) == 0)
            break;

        else if (strncmp(input, "uci", 3) == 0)
            printEngineInfo();
    }

    return 0;
}

void parsePosition(char *command, Board* board) {
    // shift pointer to the right where next token begins
    command += 9;
    
    char *current_char = command;
    
    if (strncmp(command, "startpos", 8) == 0) {
        setFen(board, START_FEN);
    
    } else {
        current_char = strstr(command, "fen");
        
        if (current_char == NULL) {
            setFen(board, START_FEN);
        } else {
            current_char += 4;
            setFen(board, current_char);
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
            sanToMove(*board, &move, current_char);
            
            
            // make move on the chess board
            pushMove(board, move);
            
            // move current character mointer to the end of current move
            while (*current_char && *current_char != ' ') current_char++;
            
            // go to the next move
            current_char++;
        }        
    }
}

void getBestMove(Board board) {
    Move bestMove;
    int nodesSearched;
    int eval = search(board, &bestMove, &nodesSearched);
    char san[6];
    moveToSan(bestMove, san);
    printf("bestmove %s\n", san);
}

void printEngineInfo(void) {
    printf("id name %s\n", ENGINE_NAME);
    printf("id author %s\n", AUTHOR);
    printf("uciok\n");
}
