CC = gcc
FLAGS = -Wall -Werror -g
OBJECTS = ./out/main.o ./out/readline.o ./out/banco.o ./out/arquivo.o ./out/crime.o ./out/csv.o ./out/funcoesFornecidas.o ./out/indice.o ./out/utilidades.o
OUT = ./out/cri

all: prog 

out:
	mkdir out

prog: out $(OBJECTS)
	$(CC) $(FLAGS) $(OBJECTS) -o $(OUT)

./out/%.o: ./src/%.c
	$(CC) $(FLAGS) -c $< -o $@

debug:
	valgrind $(OUT)

run:
	./out/cri

clean:
	rm $(OBJECTS) $(OUT)
