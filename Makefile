CXX = g++
# -MMD -MP writes a .d beside each object listing the headers it used, so editing
# a header rebuilds what depends on it.
CXXFLAGS = -std=c++17 -Wall -O2 -I. -MMD -MP

TARGET = cubealgo
BUILD  = .build
SRC    = Main.cc CubeState/CubeState.cc CubeState/MoveTable.cc
OBJS   = $(addprefix $(BUILD)/,$(notdir $(SRC:.cc=.o)))

$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) $^ -o $@

# Objects and their dependency files live in .build, so the source tree stays as
# small as what it takes to read the program.
$(BUILD)/%.o: %.cc | $(BUILD)
	$(CXX) $(CXXFLAGS) -c $< -o $@

$(BUILD)/%.o: CubeState/%.cc | $(BUILD)
	$(CXX) $(CXXFLAGS) -c $< -o $@

$(BUILD):
	@mkdir -p $(BUILD)

-include $(OBJS:.o=.d)

.PHONY: clean
clean:
	rm -rf $(BUILD) $(TARGET)
