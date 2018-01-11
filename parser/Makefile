all: parser.o AST.o

parser.o: parser.h parser.cpp AST.h AST.cpp ../lexer/lex.yy.cc exception.h
	g++ -c -std=c++11 parser.cpp `llvm-config --cxxflags --ldflags --system-libs --libs core` -o parser.o -fexceptions

AST.o: AST.h AST.cpp ../lexer/lex.yy.cc
	g++ -c -std=c++11 AST.cpp `llvm-config --cxxflags --ldflags --system-libs --libs core` -o AST.o -fexceptions

c--: parser.o AST.o ../lexer/lexer.o c--.cpp
	g++ -std=c++11 c--.cpp AST.o parser.o ../lexer/lexer.o ../lexer/lex.yy.o `llvm-config --cxxflags --ldflags --system-libs --libs core` -o c-- -fexceptions 

clean:
	rm -rf *.o c--

../lexer/lexer.o:
	(cd ../lexer/; make;)

../lexer/lex.yy.cc:
	(cd ../lexer/;make;)
