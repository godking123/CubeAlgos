CXX = g++
CXXFLAGS = -std=c++17 -Wall -O2 -I. -MMD -MP
BUILD = .build

# The state layer: cube representation, moves, notation, scrambles. Knows nothing
# about how the cube is solved.
CORE = CubeState/CubeState.cc CubeState/MoveTable.cc CubeState/Scramble.cc

# One block per solving method, plus the table that lets Main pick between them.
# A new method is a new directory here and one line in Solvers/Method.cc.
SOLVERS = Solvers/Method.cc \
          Solvers/Kociemba/Coords.cc Solvers/Kociemba/CoordTables.cc \
          Solvers/Kociemba/Phase1.cc Solvers/Kociemba/Phase2.cc \
          Solvers/Kociemba/Kociemba.cc \
          Solvers/CFOP/CFOP.cc \
          Solvers/Roux/Roux.cc

CORE_OBJS = $(addprefix $(BUILD)/,$(notdir $(CORE:.cc=.o) $(SOLVERS:.cc=.o)))

ALL_OBJS = $(CORE_OBJS) $(BUILD)/Main.o $(BUILD)/Tests.o

.PHONY: all
all: cubealgo tests

cubealgo: $(BUILD)/Main.o $(CORE_OBJS)
	$(CXX) $(CXXFLAGS) $^ -o $@

tests: $(BUILD)/Tests.o $(CORE_OBJS)
	$(CXX) $(CXXFLAGS) $^ -o $@

$(BUILD)/%.o: %.cc | $(BUILD)
	$(CXX) $(CXXFLAGS) -c $< -o $@

$(BUILD)/%.o: CubeState/%.cc | $(BUILD)
	$(CXX) $(CXXFLAGS) -c $< -o $@

$(BUILD)/%.o: Solvers/%.cc | $(BUILD)
	$(CXX) $(CXXFLAGS) -c $< -o $@

$(BUILD)/%.o: Solvers/Kociemba/%.cc | $(BUILD)
	$(CXX) $(CXXFLAGS) -c $< -o $@

$(BUILD)/%.o: Solvers/CFOP/%.cc | $(BUILD)
	$(CXX) $(CXXFLAGS) -c $< -o $@

$(BUILD)/%.o: Solvers/Roux/%.cc | $(BUILD)
	$(CXX) $(CXXFLAGS) -c $< -o $@

$(BUILD):
	@mkdir -p $(BUILD)

-include $(ALL_OBJS:.o=.d)

.PHONY: test
test: tests
	./tests

.PHONY: clean
clean:
	rm -rf $(BUILD) cubealgo tests
