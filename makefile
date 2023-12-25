default: build_13

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
	mkdir build_test; cd build_test; cmake .. -DCMAKE_BUILD_TYPE=RelWithDebInfo ; make tests;

build_13: 
	mkdir build_test; cd build_test; cmake .. -DCMAKE_BUILD_TYPE=RelWithDebInfo -DCMAKE_C_COMPILER=/usr/bin/gcc-13 -DCMAKE_CXX_COMPILER=/usr/bin/g++-13 -DLINK_LIBRARIES=-lstdc++_libbacktrace; make tests;

clang:
	mkdir build_test; cd build_test; cmake .. -D CMAKE_C_COMPILER=clang-16 -D CMAKE_CXX_COMPILER=clang++-16 -DCMAKE_BUILD_TYPE=RelWithDebInfo ; make tests;

