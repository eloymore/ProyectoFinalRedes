OUT = ProyectoFinal.exe
FLAGS = -lSDL2main -lSDL2 -lpthread -Wall -std=c++11
SRC = $(wildcard *.cc)
OBJ = $(SRC:.cc=.o)
BUILDDIR = ./build

all: makedirs program

makedirs:
	rm -rdf $(BUILDDIR)
	mkdir -p $(BUILDDIR)

program: $(OUT)

$(OUT): $(OBJ)
	g++ -o $(BUILDDIR)/$@ $(addprefix $(BUILDDIR)/,$^) $(FLAGS)

%.o: %.cc
	g++ -c $< -o $(BUILDDIR)/$@ 

.PHONY: clean
clean: 
	rm -rdf $(BUILDDIR)