BUILD_DIR ?= build
CMAKE ?= cmake
BUILD_TYPE ?= Debug
CMAKE_ARGS ?=

.PHONY: help configure build test benchmark perf exec sandbox udp-ping udp-many tcp-ping tpc-ping tcp-packet all clean

help:
	@echo "Targets:"
	@echo "  make configure  Configure the CMake build tree"
	@echo "  make build      Build all default targets"
	@echo "  make test       Build and run tests"
	@echo "  make benchmark  Build and run benchmarks"
	@echo "  make exec       Build and run the basic exec sample"
	@echo "  make sandbox    Build and run sandbox samples"
	@echo "  make udp-ping   Build UDP ping sandbox executables"
	@echo "  make udp-many   Build UDP many sandbox executables"
	@echo "  make tcp-ping   Build TCP ping sandbox executables"
	@echo "  make tpc-ping   Alias for tcp-ping"
	@echo "  make tcp-packet Build TCP packet sandbox executables"
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
	python3 ./sandbox/udp-ping/run.py --build-dir $(BUILD_DIR)
	python3 ./sandbox/udp-many/run.py --build-dir $(BUILD_DIR)
	python3 ./sandbox/tcp-ping/run.py --build-dir $(BUILD_DIR)
	python3 ./sandbox/tcp-packet/run.py --build-dir $(BUILD_DIR)

udp-ping: configure
	$(CMAKE) --build $(BUILD_DIR) --target sandbox_udp_ping_server sandbox_udp_ping_client
	python3 ./sandbox/udp-ping/run.py --build-dir $(BUILD_DIR)

udp-many: configure
	$(CMAKE) --build $(BUILD_DIR) --target sandbox_udp_many_host sandbox_udp_many_client
	python3 ./sandbox/udp-many/run.py --build-dir $(BUILD_DIR)

tcp-ping: configure
	$(CMAKE) --build $(BUILD_DIR) --target sandbox_tcp_ping_server sandbox_tcp_ping_client
	python3 ./sandbox/tcp-ping/run.py --build-dir $(BUILD_DIR)

tpc-ping: tcp-ping

tcp-packet: configure
	$(CMAKE) --build $(BUILD_DIR) --target sandbox_tcp_packet_server sandbox_tcp_packet_client
	python3 ./sandbox/tcp-packet/run.py --build-dir $(BUILD_DIR)

all: build test benchmark

clean:
	$(CMAKE) -E rm -rf $(BUILD_DIR)
