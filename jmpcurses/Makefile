
CC= gcc
CC_FLAGS= -Wall
L_FLAGS = -lncurses

.phony: ALL clean

ALL: jmpcurses 

jmpcurses: jmpcurses.c
	$(CC) $(CC_FLAGS) $(L_FLAGS) $^ -o $@

