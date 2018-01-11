# C--

C-- is a plastic compiler for simple C grammar, using Flex and LLVM.

## Build C--

1. `cd lexer/;make`

1. `cd parser/;make`

## Use C--

1. Copy `Makefile.compile` to the same directory with `C--`

1. Edit `Makefile.compile` as you wish and then rename it to `Makefile`

1. Enter `make`

## Supported Grammar

See `calculator/main.c` as an example.

## Requirements

* Flex
* LLVM 5.0.1
