CC = gcc
CFLAGS = -Wall -Wextra -std=c99 
LFLAGS = 
LIBS = -lm
SOURCES = main.c
OBJECTS = $(subst .c,.o,$(SOURCES))
EXE = main
.PHONY: clean help

main : main.c
	$(CC) $(CFLAGS) $^ $(LIBS) -o $@ 

%.o : %.c
	$(CC) $(CFLAGS) -c $< 

all : $(EXE)

clean:
	rm -f $(OBJECTS) $(EXE) *~

cleanup:
	rm -f $(OBJECTS) *~

help:
	@echo "Valid targets:"
	@echo "  all:    generates all binary files"
	@echo "  clean:  removes .o and .exe files"