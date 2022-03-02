all: build debug

build:
	clang++ --std=c++20 entropia.cpp -o build/entropia -O3

debug:
	clang++ --std=c++20 entropia.cpp -o debug/entropia -g

clean:
	rm build/entropia
	rm debug/entropia

.PHONY: clean build debug
