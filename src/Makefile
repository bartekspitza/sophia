all: main.c
	@gcc main.c fen.c board.c utils.c movegenerator.c magics.c -o sophia.out
	@./sophia.out
	@rm sophia.out

perft: perft.c
	@gcc perft.c board.c fen.c utils.c movegenerator.c magics.c -o perft
	@./perft
	@rm perft

