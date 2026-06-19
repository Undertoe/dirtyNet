BUILD_DIR ?= build
CMAKE ?= cmake
BUILD_TYPE ?= Debug
CMAKE_ARGS ?=

.PHONY: help configure build test benchmark perf exec sandbox udp-ping all clean

help:
	@echo "Targets:"
	@echo "  make configure  Configure the CMake build tree"
	@echo "  make build      Build all default targets"
	@echo "  make test       Build and run tests"
	@echo "  make benchmark  Build and run benchmarks"
	@echo "  make exec       Build and run the basic exec sample"
	@echo "  make sandbox    Build and run sandbox samples"
	@echo "  make udp-ping   Build UDP ping-pong sandbox executables"
	@echo "  make clean      Remove the build directory"

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

udp-ping: configure
	$(CMAKE) --build $(BUILD_DIR) --target sandbox_udp_pingpong_server sandbox_udp_pingpong_client
	./sandbox/udp_pingpong/run_pingpong.py --build-dir $(BUILD_DIR)

all: build test benchmark

clean:
	$(CMAKE) -E rm -rf $(BUILD_DIR)
