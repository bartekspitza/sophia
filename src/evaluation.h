#ifndef EVALUATION_H
#define EVALUATION_H

#include "typedefs.h"

extern int MAX_EVAL;
extern int MIN_EVAL;

void initEvaluation(void);
int evaluate(Board board, int result);
#endif