CC := g++

SRC_DIR = $(abspath ./src)

INCLUDE_DIR = $(abspath ./include)
INCLUDE_PATH = $(INCLUDE_DIR)

BUILD_DIR = $(abspath ./build)
$(shell mkdir -p $(BUILD_DIR))

SRCS = $(shell find $(SRC_DIR) -name "*.cpp")

BIN = $(BUILD_DIR)/tiny_parser


CXX_FLAGS = -std=c++11 -I$(INCLUDE_PATH) -DNDEBUG


default: $(BIN)
.PHONY: default

$(BIN): $(SRCS)
	$(CC) $(CXX_FLAGS) -o $@ $^

run: $(BIN)
	$(BIN)

debug_run: $(SRCS)
	$(CC) -std=c++11 -I$(INCLUDE_DIR) -o $(BIN) $^
	$(BIN)

clean: 
	rm -rf $(BUILD_DIR)
.PHONY: clean
