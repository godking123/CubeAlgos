CXX = g++
CXXFLAGS = -std=c++17 -Wall -O2 -I. -MMD -MP
BUILD = .build

CORE = CubeState/CubeState.cc CubeState/MoveTable.cc CubeState/Coords.cc \
       CubeState/CoordTables.cc CubeState/Scramble.cc Solver/Phase1.cc

CORE_OBJS = $(addprefix $(BUILD)/,$(notdir $(CORE:.cc=.o)))

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

$(BUILD)/%.o: Solver/%.cc | $(BUILD)
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
