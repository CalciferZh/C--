all: lexer.o lex.yy.o

lexer.o: lexer.h lexer.cpp
	g++ -c -std=c++11 -o lexer.o lexer.cpp

lex.yy.o: lex.yy.cc
	g++ -c -std=c++11 -o lex.yy.o lex.yy.cc

lex.yy.cc: c--.lex token.h
	flex++ c--.lex

clean:
	rm -f *.o lex.yy.cc example

example: all
	g++ -std=c++11 example.cpp lexer.o lex.yy.o -o example
