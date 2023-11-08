CC=gcc
SOURCES=lib/utils.c
BINARY=out

run:
	./$(BINARY) 
all:
	$(CC) -Wall main.c $(SOURCES) -o $(BINARY) -I lib
valgrind:
	valgrind --tool=memcheck --leak-check=full  --track-origins=yes --show-leak-kinds=all --show-reachable=yes ./$(BINARY)
clean:
	@rm $(BINARY) out.bmp