CXX = clang++
CXXFLAGS = -std=c++17 -Wall -Wextra -Wpedantic -Werror
ifeq ($(shell uname -s),Darwin)
CXX = $(shell xcrun --find clang++)
CXXFLAGS += -isysroot $(shell xcrun --sdk macosx --show-sdk-path)
LLVM_COV = $(shell xcrun --find llvm-cov)
LLVM_PROFDATA = $(shell xcrun --find llvm-profdata)
endif
DOMAIN = lab1/vector/Vector3.cpp lab1/tictactoe/TicTacToe.cpp
TEST_SOURCES = lab1/tests/vector_tests.cpp lab1/tests/tictactoe_tests.cpp
HEADERS = lab1/vector/Vector3.hpp lab1/tictactoe/TicTacToe.hpp
LLVM_COV ?= llvm-cov
LLVM_PROFDATA ?= llvm-profdata

.PHONY: all test coverage docs clean
all: build/vector_cli build/tictactoe_cli

build:
	mkdir -p build

build/vector_cli: lab1/vector/main.cpp lab1/vector/Vector3.cpp $(HEADERS) lab1/cli/Input.hpp | build
	$(CXX) $(CXXFLAGS) lab1/vector/main.cpp lab1/vector/Vector3.cpp -o $@

build/tictactoe_cli: lab1/tictactoe/main.cpp lab1/tictactoe/TicTacToe.cpp $(HEADERS) lab1/cli/Input.hpp | build
	$(CXX) $(CXXFLAGS) lab1/tictactoe/main.cpp lab1/tictactoe/TicTacToe.cpp -o $@

build/doctest.h: | build
	curl --fail --location --retry 3 https://raw.githubusercontent.com/doctest/doctest/v2.4.11/doctest/doctest.h -o $@

build/tests: $(TEST_SOURCES) $(DOMAIN) $(HEADERS) build/doctest.h
	$(CXX) $(CXXFLAGS) -Ibuild $(TEST_SOURCES) $(DOMAIN) -o $@

test: build/tests
	./build/tests

build/coverage_tests: $(TEST_SOURCES) $(DOMAIN) $(HEADERS) build/doctest.h
	$(CXX) $(CXXFLAGS) -O0 -g -fprofile-instr-generate -fcoverage-mapping -Ibuild $(TEST_SOURCES) $(DOMAIN) -o $@

coverage: build/coverage_tests
	LLVM_PROFILE_FILE=build/tests.profraw ./build/coverage_tests
	$(LLVM_PROFDATA) merge -sparse build/tests.profraw -o build/tests.profdata
	$(LLVM_COV) report ./build/coverage_tests -instr-profile=build/tests.profdata $(DOMAIN)
	$(LLVM_COV) export ./build/coverage_tests -instr-profile=build/tests.profdata $(DOMAIN) > build/coverage.json
	python3 scripts/check_coverage.py build/coverage.json
	$(LLVM_COV) show ./build/coverage_tests -instr-profile=build/tests.profdata $(DOMAIN) -format=html -output-dir=build/coverage

docs:
	doxygen Doxyfile

clean:
	rm -rf build
