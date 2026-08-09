BUILD_DIR ?= build
CMAKE ?= cmake
BUILD_TYPE ?= Debug
CMAKE_ARGS ?=

.PHONY: help configure build test tests benchmark perf exec sandbox udp-ping udp-ping-posix udp-ping-dirtynet udp-many udp-many-posix udp-many-dirtynet tcp-ping tcp-ping-posix tcp-ping-dirtynet tpc-ping tcp-packet tcp-packet-posix tcp-packet-dirtynet all clean

help:
	@echo "Targets:"
	@echo "  make configure  Configure the CMake build tree"
	@echo "  make build      Build all default targets"
	@echo "  make tests      Build and run tests"
	@echo "  make test       Alias for tests"
	@echo "  make benchmark  Build and run benchmarks"
	@echo "  make exec       Build the maintained executable"
	@echo "  make sandbox    Build and run sandbox samples"
	@echo "  make udp-ping   Build both UDP ping implementations"
	@echo "  make udp-ping-posix    Build the POSIX UDP ping implementation"
	@echo "  make udp-ping-dirtynet Build the dirtyNet UDP ping implementation"
	@echo "  make udp-many   Build both UDP many implementations"
	@echo "  make udp-many-posix    Build the POSIX UDP many implementation"
	@echo "  make udp-many-dirtynet Build the dirtyNet UDP many implementation"
	@echo "  make tcp-ping   Build both TCP ping implementations"
	@echo "  make tcp-ping-posix    Build the POSIX TCP ping implementation"
	@echo "  make tcp-ping-dirtynet Build the dirtyNet TCP ping implementation"
	@echo "  make tpc-ping   Alias for tcp-ping"
	@echo "  make tcp-packet Build both TCP packet implementations"
	@echo "  make tcp-packet-posix    Build the POSIX TCP packet implementation"
	@echo "  make tcp-packet-dirtynet Build the dirtyNet TCP packet implementation"
	@echo "  make clean      Remove the build directory"

configure:
	$(CMAKE) -S . -B $(BUILD_DIR) -G Ninja -DCMAKE_BUILD_TYPE=$(BUILD_TYPE) $(CMAKE_ARGS)

build: configure
	$(CMAKE) --build $(BUILD_DIR)

tests: configure
	$(CMAKE) --build $(BUILD_DIR) --target dirtynet_tests
	ctest --test-dir $(BUILD_DIR) --output-on-failure

test: tests

benchmark: configure
	$(CMAKE) --build $(BUILD_DIR) --target dirtynet_benchmarks
	./$(BUILD_DIR)/benchmarks/dirtynet_benchmarks

perf: benchmark

exec: configure
	$(CMAKE) --build $(BUILD_DIR) --target dirtynet_exec

sandbox: build
	python3 ./sandbox/udp-ping/run.py posix --build-dir $(BUILD_DIR)
	python3 ./sandbox/udp-many/run.py posix --build-dir $(BUILD_DIR)
	python3 ./sandbox/tcp-ping/run.py posix --build-dir $(BUILD_DIR)
	python3 ./sandbox/tcp-packet/run.py posix --build-dir $(BUILD_DIR)

udp-ping: udp-ping-posix udp-ping-dirtynet

udp-ping-posix: configure
	$(CMAKE) --build $(BUILD_DIR) --target sandbox_udp_ping_posix
	python3 ./sandbox/udp-ping/run.py posix --build-dir $(BUILD_DIR)

udp-ping-dirtynet: configure
	$(CMAKE) --build $(BUILD_DIR) --target sandbox_udp_ping_dirtynet
	python3 ./sandbox/udp-ping/run.py dirtynet --build-dir $(BUILD_DIR)

udp-many: udp-many-posix udp-many-dirtynet

udp-many-posix: configure
	$(CMAKE) --build $(BUILD_DIR) --target sandbox_udp_many_posix
	python3 ./sandbox/udp-many/run.py posix --build-dir $(BUILD_DIR)

udp-many-dirtynet: configure
	$(CMAKE) --build $(BUILD_DIR) --target sandbox_udp_many_dirtynet
	python3 ./sandbox/udp-many/run.py dirtynet --build-dir $(BUILD_DIR)

tcp-ping: tcp-ping-posix tcp-ping-dirtynet

tcp-ping-posix: configure
	$(CMAKE) --build $(BUILD_DIR) --target sandbox_tcp_ping_posix
	python3 ./sandbox/tcp-ping/run.py posix --build-dir $(BUILD_DIR)

tcp-ping-dirtynet: configure
	$(CMAKE) --build $(BUILD_DIR) --target sandbox_tcp_ping_dirtynet
	python3 ./sandbox/tcp-ping/run.py dirtynet --build-dir $(BUILD_DIR)

tpc-ping: tcp-ping

tcp-packet: tcp-packet-posix tcp-packet-dirtynet

tcp-packet-posix: configure
	$(CMAKE) --build $(BUILD_DIR) --target sandbox_tcp_packet_posix
	python3 ./sandbox/tcp-packet/run.py posix --build-dir $(BUILD_DIR)

tcp-packet-dirtynet: configure
	$(CMAKE) --build $(BUILD_DIR) --target sandbox_tcp_packet_dirtynet
	python3 ./sandbox/tcp-packet/run.py dirtynet --build-dir $(BUILD_DIR)

all: build tests benchmark

clean:
	$(CMAKE) -E rm -rf $(BUILD_DIR)
