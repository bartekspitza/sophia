all: uci.c
	@gcc uci.c   evaluation.c search.c board.c fen.c utils.c movegenerator.c magics.c -o sophia

perft: perft.c
	@gcc perft.c evaluation.c search.c board.c fen.c utils.c movegenerator.c magics.c -o perft
	@./perft
	@rm perft


