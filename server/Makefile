CXX = g++

CXXFLAGS = -std=c++17 -Iinclude -Wall -Wextra -pthread 
LIBS = -lsqlite3 -pthread

SRC = $(wildcard src/*.cpp)
OBJ = $(SRC:.cpp=.o)

server: $(OBJ)
	$(CXX) $(OBJ) -o server $(LIBS)

src/%.o: src/%.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -f src/*.o server