IMPL = .
CXX = g++
CXX_OPTIONS = -std=c++20 -O2 -g -Wall
BUILD_DIR = build

.PHONY: all build clean run run_test_1 run_test_2 run_test_3 run_test_4 run_test_5 run_test_6 run_test_7 run_test_8 run_test_9 run_test_10

all: run

run: run_test_1 \
	run_test_2 \
	run_test_3 \
	run_test_4 \
	run_test_5 \
	run_test_6 \
	run_test_7 \
	run_test_8 \
	run_test_9 \
	run_test_10

run_test_1: $(BUILD_DIR)/test_1
	timeout 10s $(BUILD_DIR)/test_1

run_test_2: $(BUILD_DIR)/test_2
	timeout 10s $(BUILD_DIR)/test_2

run_test_3: $(BUILD_DIR)/test_3
	timeout 10s $(BUILD_DIR)/test_3

run_test_4: $(BUILD_DIR)/test_4
	timeout 10s $(BUILD_DIR)/test_4

run_test_5: $(BUILD_DIR)/test_5
	timeout 10s $(BUILD_DIR)/test_5

run_test_6: $(BUILD_DIR)/test_6
	timeout 10s $(BUILD_DIR)/test_6

run_test_7: $(BUILD_DIR)/test_7
	timeout 10s $(BUILD_DIR)/test_7

run_test_8: $(BUILD_DIR)/test_8
	timeout 10s $(BUILD_DIR)/test_8

run_test_9:  $(BUILD_DIR)/test_9
	timeout 10s $(BUILD_DIR)/test_9

run_test_10: $(BUILD_DIR)/test_10
	timeout 10s $(BUILD_DIR)/test_10

build: $(BUILD_DIR)/test_1 \
	$(BUILD_DIR)/test_2 \
	$(BUILD_DIR)/test_3 \
	$(BUILD_DIR)/test_4 \
	$(BUILD_DIR)/test_5 \
	$(BUILD_DIR)/test_6 \
	$(BUILD_DIR)/test_7 \
	$(BUILD_DIR)/test_8 \
	$(BUILD_DIR)/test_9 \
	$(BUILD_DIR)/test_10

clean:
	$(RM) -r $(BUILD_DIR)

$(BUILD_DIR)/%: $(BUILD_DIR)/%.o $(IMPL)/locker.hpp interval_tree.hpp
	@mkdir -p $(BUILD_DIR)
	$(CXX) -o $@ $(CXX_OPTIONS) -DNDEBUG -I $(IMPL) -I . $<

$(BUILD_DIR)/%.o: %.cpp $(IMPL)/locker.hpp interval_tree.hpp
	@mkdir -p $(BUILD_DIR)
	$(CXX) -c -o $@ $(CXX_OPTIONS) -DNDEBUG -I $(IMPL) -I . $<
