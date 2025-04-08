CC := g++

BUILD_DIR = $(abspath ./build)
SRC_DIR = $(abspath ./src)

$(shell mkdir -p $(BUILD_DIR))

SRCS = $(shell find $(SRC_DIR) -name "*.cpp")
BIN = $(BUILD_DIR)/tiny_lexer

default: $(BIN)
.PHONY: default

$(BIN): $(SRCS)
	$(CC) -o $@ $^

run: $(BIN)
	$(BIN)

clean: 
	rm -rf $(BUILD_DIR)
.PHONY: clean
