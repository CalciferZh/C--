SRC = ../calculator/main.c
TARGET = calculator
CC = gcc
C-- = ./c--

all: $(TARGET)

$(TARGET): $(TARGET).s
	$(CC) $(TARGET).s -o $(TARGET)

$(TARGET).s:$(TARGET).ll
	llc -march=x86-64 $(TARGET).ll -o $(TARGET).s

$(TARGET).ll: $(SRC)
	$(C--) $(SRC) 2>>$(TARGET).ll

clean:
	rm -rf $(TARGET)*