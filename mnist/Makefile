CXX = clang++
CXXFLAGS = -Wall -g
LDFLAGS = 

TARGET = mnist

SRC = $(wildcard *.cpp) 
OBJ = $(SRC:.cpp=.o)

$(info $(OBJ))

$(TARGET): $(OBJ)
	$(CXX) -o $@ $^ $(LDFLAGS)

.PHONY: clean
clean:
	rm -f $(OBJ) myprog
