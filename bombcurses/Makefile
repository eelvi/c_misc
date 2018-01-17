
CC = cc
CC_FLAGS = -Wall -O1
#CC_FLAGS = -Wall -O0 -g

L_FLAGS = $(shell pkg-config --libs ncurses) -lm

.PHONY: ALL clean

ALL: bombcurses

bombcurses: bombcurses.c
	$(CC) $(CC_FLAGS) $(L_FLAGS) $^ -o $@
