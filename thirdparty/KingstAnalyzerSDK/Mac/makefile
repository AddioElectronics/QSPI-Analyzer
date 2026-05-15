TARGET  := libSPI.dylib

LINK := -L "../lib/Mac" -lAnalyzer

CC       := clang++
HFILE    := ../inc/*.h
SRC      := ../../src/*.cpp
INC      := -I ../inc/
CXXFLAGS := -Wall -O2 -c
FPIC     := -fPIC
SHARE    := -dynamiclib -o
OBJ      := *.o

$(TARGET) : $(HFILe) $(SRC)
	$(CC) $(CXXFLAGS) $(FPIC) $(SRC) $(INC)
	$(CC) $(SHARE) $(TARGET) $(OBJ) $(LINK)

