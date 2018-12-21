all: words

words: temp/words.o temp/vect.o temp/dict.o temp/datamodule.o temp/wdata.o temp/wordvect.o
	g++ -std=c++11 -g temp/words.o temp/vect.o temp/dict.o temp/datamodule.o \
	                  temp/wdata.o temp/wordvect.o -o words

temp/words.o: src/main.cpp include/dict.h
	g++ -std=c++11 -c src/main.cpp
	mv main.o temp/words.o

temp/vect.o: src/vect.cpp include/vect.h
	g++ -std=c++11 -c src/vect.cpp
	mv vect.o temp/vect.o

temp/dict.o: src/dict.cpp include/dict.h
	g++ -std=c++11 -c src/dict.cpp
	mv dict.o temp/dict.o

temp/datamodule.o: src/datamodule.cpp include/datamodule.h
	g++ -std=c++11 -c src/datamodule.cpp
	mv datamodule.o temp/datamodule.o

temp/wdata.o: src/wdata.cpp include/wdata.h
	g++ -std=c++11 -c src/wdata.cpp
	mv wdata.o temp/wdata.o

temp/wordvect.o: src/wordvect.cpp include/wordvect.h
	g++ -std=c++11 -c src/wordvect.cpp
	mv wordvect.o temp/wordvect.o

clean:
	rm -f *~
	rm -f temp/*.o
	rm -f words
