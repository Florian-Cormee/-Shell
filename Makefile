MODE := debug

EXEC := ushell

CC := gcc
CFLAGS := -Wall -std=gnu11
CFLAGS_DEBUG := -g -O0

ifeq ($(MODE), debug)
CFLAGS += $(CFLAGS_DEBUG)
endif

build: main.o parser.o cd.o utils.o command.o logger.o
	$(CC) $(CFLAGS) -o $(EXEC) $^

%.o : %.c
	$(CC) $(CFLAGS) -c $<

clean:
	@rm -rf *.o
	@echo "Removed all object files"

vclean: clean
	@rm -rf $(EXEC)
	@rm -rf *.exe
	@echo "Removed all executable files"

rebuild: vclean build

valgrind: build
	valgrind --leak-check=full ./$(EXEC)

debug: build
	gdb ./$(EXEC)
