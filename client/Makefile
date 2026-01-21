CXX = g++
CXXFLAGS = -std=c++17 -Iinclude -Wall -Wextra

SRC = $(wildcard src/*.cpp)
OBJ = $(SRC:.cpp=.o)

client: $(OBJ)
	$(CXX) $(OBJ) -o client

src/%.o: src/%.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -f src/*.o client