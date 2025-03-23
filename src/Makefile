.PHONY: all test perft profiling

all: uci.c
	@clang uci.c bitboards.c moveorderer.c evaluation.c san.c tt.c zobrist.c search.c board.c fen.c utils.c movegen.c magics.c -o sophia

profiling: profiling.c
	@clang -pg profiling.c bitboards.c moveorderer.c evaluation.c san.c tt.c zobrist.c search.c board.c fen.c utils.c movegen.c magics.c -o profiling
	./profiling
	gprof profiling gmon.out > analysis.txt
	rm profiling
	rm gmon.out

test: test.c
	@clang test.c -o test bitboards.c evaluation.c san.c zobrist.c board.c fen.c utils.c movegen.c magics.c -L../libs/unity -lunity
	@./test
	@rm test

# Fully optimized perft target using LTO and PGO.
perft: perft.c
	@echo "Building instrumented binary for profile generation..."
	clang -O2 -flto -fprofile-generate perft.c bitboards.c san.c zobrist.c board.c fen.c utils.c movegen.c magics.c -o perft-gen
	@echo "Running instrumented binary with depth flag..."
	LLVM_PROFILE_FILE=default.profraw ./perft-gen -d 5
	@echo "Merging profile data..."
	llvm-profdata merge -output=default.profdata default.profraw
	@echo "Building fully optimized perft binary..."
	clang -O2 -flto -fprofile-use perft.c bitboards.c san.c zobrist.c board.c fen.c utils.c movegen.c magics.c -o perft
	@echo "Cleaning up temporary files..."
	rm perft-gen default.profraw
