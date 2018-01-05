all: parser.o AST.o

parser.o: parser.h parser.cpp AST.h AST.cpp ../lexer/lex.yy.cc
	g++ -c -std=c++11 parser.cpp `llvm-config --cxxflags --ldflags --system-libs --libs core` -o parser.o

AST.o: AST.h AST.cpp ../lexer/lex.yy.cc
	g++ -c -std=c++11 AST.cpp `llvm-config --cxxflags --ldflags --system-libs --libs core` -o AST.o

example: parser.o AST.o ../lexer/lexer.o example.cpp
	g++ -std=c++11 example.cpp AST.o parser.o ../lexer/lexer.o ../lexer/lex.yy.o `llvm-config --cxxflags --ldflags --system-libs --libs core` -o example

clean:
	rm -rf *.o example

../lexer/lexer.o:
	(cd ../lexer/; make;)
