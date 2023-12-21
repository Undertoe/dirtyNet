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
	mkdir build_test; cd build_test; cmake .. -DCMAKE_BUILD_TYPE=RelWithDebInfo ; make tests;

clang:
	mkdir build_test; cd build_test; cmake .. -D CMAKE_C_COMPILER=clang-16 -D CMAKE_CXX_COMPILER=clang++-16 -DCMAKE_BUILD_TYPE=RelWithDebInfo ; make tests;

