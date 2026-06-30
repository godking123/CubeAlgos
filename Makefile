CXX = g++
CXXFLAGS = -std=c++17 -Wall -O2 -I.

TARGET = cubealgo
OBJS = Main.o CubeState/CubeState.o CubeState/MoveTable.o

$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) $^ -o $@

%.o: %.cc
	$(CXX) $(CXXFLAGS) -c $< -o $@

.PHONY: clean
clean:
	rm -f $(OBJS) $(TARGET)
