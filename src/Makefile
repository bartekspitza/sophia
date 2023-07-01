.PHONY: all test

all: uci.c
	@gcc uci.c  bitboards.c moveorderer.c evaluation.c san.c tt.c zobrist.c search.c board.c fen.c utils.c movegen.c magics.c -o sophia

perft: perft.c
	gcc perft.c bitboards.c san.c zobrist.c board.c fen.c utils.c movegen.c magics.c -o perft


profiling: profiling.c
	@gcc -pg profiling.c bitboards.c moveorderer.c evaluation.c san.c tt.c zobrist.c search.c board.c fen.c utils.c movegen.c magics.c -o profiling
	./profiling
	gprof profiling gmon.out > analysis.txt
	rm profiling
	rm gmon.out

perft-pg: perft.c
	@gcc -pg perft.c bitboards.c evaluation.c san.c tt.c zobrist.c search.c board.c fen.c utils.c movegen.c magics.c -o perft
	./perft
	gprof perft gmon.out > analysis.txt
	rm perft

test: test.c
	@gcc test.c -o test bitboards.c evaluation.c san.c zobrist.c board.c fen.c utils.c movegen.c magics.c -L../libs/unity -lunity
	@./test
	@rm test
