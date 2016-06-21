CXX=g++-5
LD=g++-5


# FLAGS=-Wall -ggdb
FLAGS=-O3 -Wall

MAINS=build/main.o

TARGETS=$(patsubst build/%.o, %, $(MAINS))
OBJ=$(patsubst src/%.cpp, build/%.o,$(wildcard src/*.cpp)) #

DEPS=$(wildcard src/*.h)
SRC=$(wildcard src/*.cpp)

INC=include
DATA=$(wildcard *.fa) $(wildcard *.tree)
OTHER=Makefile README

FILES=$(SRC) $(DEPS) $(INC) $(DATA) $(OTHER)

CFLAGS=-I $(INC) $(FLAGS)

.PHONY: clean src-pkg

all: $(TARGETS)

#debug:
#	echo $(DEPS)

# Rule for object files
build/%.o: src/%.cpp $(DEPS)
	$(CXX) -c -o $@ $< $(CFLAGS)

# Rule for targets
$(TARGETS): $(OBJ)
	$(LD) -o $@ $^ $(LDFLAGS)


clean:
	rm -f src/*.o *~ $(TARGETS)

src-pkg: $(TARGETS) $(DEPS)
	tar czf Empar.tar.gz $(FILES)
