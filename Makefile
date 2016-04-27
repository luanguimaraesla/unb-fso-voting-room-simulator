BINFOLDER := ./bin/
INCFOLDER := ./include/
SRCFOLDER := ./source/
OBJFOLDER := ./obj/

COMPILER := gcc
FLAGS := -Wall -ansi -I$(INCFOLDER)
CFLAGS := -pthread -lX11

SRCFILES := $(wildcard source/*.c)

all:	 $(SRCFILES:source/%.c=obj/%.o)
		$(COMPILER) $(FLAGS) $(OBJFOLDER)*.o -o $(BINFOLDER)simulator $(CFLAGS)

obj/%.o:	source/%.c
				$(COMPILER) $(FLAGS) -c $< -o $@ $(CFLAGS)

.PHONY: clean
clean:
	rm -rf $(OBJFOLDER)*.o

run:
	$(BINFOLDER)simulator
