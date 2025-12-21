default: build

help:
	@echo "Usage: make [target]"
	@echo "Targets:"
	@echo "  build    - build the project"
	@echo "  clang    - build project with clang"
	@echo "  clean    - clean the project"
	@echo "  help     - print this help message"


clean: 
	rm -rf build build_test;

build:
	mkdir -p build_test
	cd build_test && cmake .. -DCMAKE_BUILD_TYPE=RelWithDebInfo
	cd build_test && $(MAKE)            # or: $(MAKE) ip_tests
# 	cd build_test && $(MAKE) tests      # runs custom CMake target “tests”

test: build
	cd build_test && ./tests/dirtyNet_test         # runs the test executable

bench: 
	mkdir -p build_bench
	cd build_bench && cmake .. -DCMAKE_BUILD_TYPE=RelWithDebInfo -DCMAKE_CXX_FLAGS="-pg" && $(MAKE) && ./benchmarks/dirtyNet_bench  # runs the benchmark executable
	cd ..

bench_macos:
	mkdir -p build_bench && cd build_bench && cmake .. -DCMAKE_BUILD_TYPE=RelWithDebInfo -DCMAKE_CXX_FLAGS="-pg" -DCMAKE_C_COMPILER=clang -DCMAKE_CXX_COMPILER=clang++ -DCMAKE_PREFIX_PATH=/opt/homebrew && $(MAKE) && ./benchmarks/dirtyNet_bench

build_13: 
	mkdir build_test; cd build_test; cmake .. -DCMAKE_BUILD_TYPE=RelWithDebInfo -DCMAKE_C_COMPILER=/usr/bin/gcc-13 -DCMAKE_CXX_COMPILER=/usr/bin/g++-13 -DLINK_LIBRARIES=-lstdc++_libbacktrace; make tests;

clang:
	mkdir build_test; cd build_test; cmake .. -D CMAKE_C_COMPILER=clang-16 -D CMAKE_CXX_COMPILER=clang++-16 -DCMAKE_BUILD_TYPE=RelWithDebInfo ; make tests;


