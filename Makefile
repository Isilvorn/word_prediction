all: words

words: temp/words.o temp/vect.o temp/datamodule.o
	g++ -std=c++11 -g temp/words.o temp/vect.o temp/datamodule.o
	mv a.out words

temp/words.o: src/main.cpp
	g++ -std=c++11 -c src/main.cpp
	mv main.o temp/words.o

temp/vect.o: src/vect.cpp
	g++ -std=c++11 -c src/vect.cpp
	mv vect.o temp/vect.o

temp/datamodule.o: src/datamodule.cpp
	g++ -std=c++11 -c src/datamodule.cpp
	mv datamodule.o temp/datamodule.o

clean:
	rm -f *~
	rm temp/*.o
	rm words
