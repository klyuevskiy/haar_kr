SOURCE=haar
HAAR_LIB=haar.hpp
COMPILER?=g++
CC=$(COMPILER)
OPTFLAGS=-O0
EXE=exe

ifeq ($(CC), g++)
	PRINT=-lstdc++exp
endif

CFLAGS=-g -std=c++23 $(PRINT) $(OPTFLAGS) -DDEBUG

all : $(SOURCE).$(EXE)

$(SOURCE).$(EXE) : $(SOURCE).cpp $(HAAR_LIB)
	$(CC) -o $@ $< $(CFLAGS)

clean:
	-del $(SOURCE).$(EXE) $(SOURCE).ilk $(SOURCE).pdb 