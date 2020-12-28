all: uci.c
	@gcc uci.c   evaluation.c search.c board.c fen.c utils.c movegen.c magics.c -o sophia

perft: perft.c
	@gcc perft.c evaluation.c search.c board.c fen.c utils.c movegen.c magics.c -o perft
	@./perft
	@rm perft


