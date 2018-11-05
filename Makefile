all: words

words: temp/words.o
	g++ -std=c++11 -g temp/words.o
	mv a.out words

temp/words.o: src/main.cpp
	g++ -std=c++11 -c src/main.cpp
	mv main.o temp/words.o

clean:
	rm -f *~
	rm temp/*
