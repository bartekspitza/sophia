.PHONY: all test

all: uci.c
	@gcc uci.c   evaluation.c san.c tt.c zobrist.c search.c board.c fen.c utils.c movegen.c magics.c -o sophia

perft: perft.c
	@gcc perft.c evaluation.c san.c tt.c zobrist.c search.c board.c fen.c utils.c movegen.c magics.c -o perft
	@./perft
	@rm perft

perft-pg: perft.c
	@gcc -pg perft.c evaluation.c san.c tt.c zobrist.c search.c board.c fen.c utils.c movegen.c magics.c -o perft
	./perft
	gprof perft gmon.out > analysis.txt
	rm perft

test: test.c
	@gcc test.c -o test evaluation.c san.c board.c fen.c utils.c movegen.c magics.c -L../libs/unity -lunity
	@./test
	@rm test
