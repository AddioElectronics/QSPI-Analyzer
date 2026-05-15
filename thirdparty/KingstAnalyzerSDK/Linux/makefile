TARGET  := libSPI.so

LINK := -L "../lib/Linux" -lAnalyzer

CC       := g++
HFILE    := ../inc/*.h
SRC      := ../../src/*.cpp
INC      := -I ../inc/
CXXFLAGS := -Wall -O2 -c
FPIC     := -fPIC
SHARE    := -shared -o
OBJ      := *.o

$(TARGET) : $(HFILe) $(SRC)
	$(CC) $(CXXFLAGS) $(FPIC) $(SRC) $(INC)
	$(CC) $(SHARE) $(TARGET) $(OBJ) $(LINK)

