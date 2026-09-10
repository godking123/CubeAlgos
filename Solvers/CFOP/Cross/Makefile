CXX = g++
CXXFLAGS = -std=c++17 -Wall -O2 -MMD -MP
BUILD = .build
ROOT = ../../..

# Cube representation, moves, scrambles and the rotation-aware solver state
CORE = $(ROOT)/CubeState/CubeState.cc $(ROOT)/CubeState/MoveTable.cc \
       $(ROOT)/CubeState/Scramble.cc $(ROOT)/CubeState/SolverState.cc

SRCS = cross-test.cc Cross.cc $(CORE)
OBJS = $(addprefix $(BUILD)/,$(notdir $(SRCS:.cc=.o)))

.PHONY: all
all: cross-test

cross-test: $(OBJS)
	$(CXX) $(CXXFLAGS) $^ -o $@

$(BUILD)/%.o: %.cc | $(BUILD)
	$(CXX) $(CXXFLAGS) -c $< -o $@

$(BUILD)/%.o: $(ROOT)/CubeState/%.cc | $(BUILD)
	$(CXX) $(CXXFLAGS) -c $< -o $@

$(BUILD):
	@mkdir -p $(BUILD)

-include $(OBJS:.o=.d)

.PHONY: run
run: cross-test
	./cross-test

.PHONY: clean
clean:
	rm -rf $(BUILD) cross-test
