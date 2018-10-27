all: words

words: words.o
	g++ -std=c++11 -g words.o
	mv a.out words

words.o: main.cpp
	g++ -std=c++11 -c main.cpp
	mv main.o words.o
