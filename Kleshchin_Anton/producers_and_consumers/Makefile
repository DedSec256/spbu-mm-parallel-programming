GCC_ONLY_FLAGS:=-Wduplicated-branches -Wduplicated-cond -Wlogical-op
FLAGS:=-Werror -pedantic-errors -Wall -Wextra -Wpedantic -Wcast-align -Wcast-qual -Wconversion -Wctor-dtor-privacy -Wextra-semi -Wfloat-equal -Wnon-virtual-dtor -Wold-style-cast -Woverloaded-virtual -Wredundant-decls -Wsign-conversion -Wsign-promo
COMMON_FLAGS:=-std=c++17 -pthread
OUT_NAME:=run
FILES:=src/*.cpp

default_build:
	g++ -O3 $(COMMON_FLAGS) $(FILES) -o $(OUT_NAME)
bulid:
	g++ -O3 $(GCC_ONLY_FLAGS) $(COMMON_FLAGS) $(FLAGS) $(FILES) -o $(OUT_NAME)
debug:
	g++ -g -no-pie $(COMMON_FLAGS) $(FILES) -o $(OUT_NAME)
clang_build:
	clang++ -O3 $(COMMON_FLAGS) $(FLAGS) $(FILES) -o $(OUT_NAME)
