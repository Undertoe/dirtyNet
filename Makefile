BUILD_DIR ?= build
CMAKE ?= cmake
BUILD_TYPE ?= Debug
CMAKE_ARGS ?=

.PHONY: configure build test benchmark perf exec sandbox all clean

configure:
	$(CMAKE) -S . -B $(BUILD_DIR) -G Ninja -DCMAKE_BUILD_TYPE=$(BUILD_TYPE) $(CMAKE_ARGS)

build: configure
	$(CMAKE) --build $(BUILD_DIR)

test: build
	ctest --test-dir $(BUILD_DIR) --output-on-failure

benchmark: build
	./$(BUILD_DIR)/perf/dirtynet_perf

perf: benchmark

exec: build
	./$(BUILD_DIR)/exec/basic/dirtynet_exec_basic

sandbox: build
	./$(BUILD_DIR)/sandbox/hello_world/sandbox_hello_world
	./$(BUILD_DIR)/sandbox/with_lib/sandbox_with_lib

all: build test benchmark

clean:
	$(CMAKE) -E rm -rf $(BUILD_DIR)
