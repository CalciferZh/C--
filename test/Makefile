all: cl my

cl:cl.s
	clang cl.s -o cl

my:my.s
	clang my.s -o my

cl.s:cl.ll
	llc -march=x86-64 cl.ll -o cl.s

my.s:my.ll
	llc -march=x86-64 my.ll -o my.s

cl.ll: 1.c
	clang -emit-llvm 1.c -S -o cl.ll

my.ll: 1.c
	./example 2>>my.ll

clean:
	rm -rf my* cl*
