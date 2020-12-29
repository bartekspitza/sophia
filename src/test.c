#include "../libs/unity/unity.h"
#include "evaluation.h"
#include "board.h"
#include "fen.h"
#include "movegen.h"
#include "search.h"

void setUp(void) {
    // set stuff up here
}

void tearDown(void) {
    // clean stuff up here
}

void test(void) {
}

// not needed when using generate_test_runner.rb
int main(void) {
    UNITY_BEGIN();
    RUN_TEST(test);
    return UNITY_END();
}
