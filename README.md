# c--

c-- is a plastic compiler for simple C grammar, using Flex and LLVM.

## Build c--

1. `cd lexer/;make`

1. `cd parser/;make`

## Use c--

1. Copy `Makefile.compile` to the same directory with `c--`

1. Edit `Makefile.compile` as you wish and then rename it to `Makefile`

1. Enter `make`

## Supported Grammar

See `calculator/main.c` as an example.

## Requirements

* Flex
* LLVM 5.0.1
