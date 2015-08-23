CC=g++

# FLAGS=-Wall
# FLAGS=-Wall -ggdb
FLAGS=-O3 -Wall

MAINS=Empar.o 

TARGETS=$(patsubst %.o,%,$(MAINS))
OBJ=$(filter-out $(MAINS),$(patsubst %.cpp,%.o,$(wildcard *.cpp)))

DEPS=$(wildcard *.h)
SRC=$(wildcard *.cpp)

INC=include
DATA=$(wildcard *.fa) $(wildcard *.tree)
OTHER=Makefile README

FILES=$(SRC) $(DEPS) $(INC) $(DATA) $(OTHER)

CFLAGS=-I $(INC) $(FLAGS)

.PHONY: clean src-pkg

all: $(TARGETS)

# Rule for object files
%.o: %.cpp $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

# Rule for targets
$(TARGETS): %: %.o $(OBJ)
	$(CC) -o $@ $< $(OBJ) $(CFLAGS)


clean:
	rm -f *.o *~ $(TARGETS) 
src-pkg: $(TARGETS) $(DEPS)
	tar czf MixPar.tar.gz $(FILES)