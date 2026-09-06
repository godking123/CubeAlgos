CXX = g++
CXXFLAGS = -std=c++17 -Wall -O2 -I../..

SRCS = CrossTest.cc \
       Cross.cc \
       ../../../CubeState/CubeState.cc \
       ../../../CubeState/MoveTable.cc \
       ../../../CubeState/Scramble.cc

crosstest: $(SRCS)
	$(CXX) $(CXXFLAGS) $^ -o crosstest

.PHONY: clean
clean:
	rm -f crosstest