# Usage:
#		make							- Build release version
#		make debug				- Build debug version
#		make asan					- Build with AddressSanitizer
#		make ubsan				- Build with UndefinedBehaviorSanitizer  
#		make sanitize			- Build with both ASan and UBSan
#		make analyze			- Build with static analysis (clang-tidy + cppcheck)
#		make test					- Build and run property-based tests with RapidCheck
#		make clean				- Remove build artifacts
#		make format				- Format source code with clang-format
#		make help					- Show this help

# Build directory
BUILD_DIR := build

# CMake generator (Ninja if available, otherwise Unix Makefiles)
CMAKE_GENERATOR := $(shell which ninja > /dev/null 2>&1 && echo "Ninja" || echo "Unix Makefiles")

# Default target
.DEFAULT_GOAL := debug

# Phony targets
.PHONY: all release debug asan ubsan tsan sanitize analyze test clean distclean format help

# =
# Build Targets
# =

all: debug

release:
	@echo "Building Release"
	@mkdir -p $(BUILD_DIR)/release
	@cd $(BUILD_DIR)/release && cmake \
		-G "$(CMAKE_GENERATOR)" \
		-DCMAKE_BUILD_TYPE=Release \
		../..
	@cmake --build $(BUILD_DIR)/release
	@echo "Build Complete: ./router"

debug:
	@echo "Building Debug"
	@mkdir -p $(BUILD_DIR)/debug
	@cd $(BUILD_DIR)/debug && cmake \
		-G "$(CMAKE_GENERATOR)" \
		-DCMAKE_BUILD_TYPE=Debug \
		../..
	@cmake --build $(BUILD_DIR)/debug
	@echo "Build Complete: ./router"

asan:
	@echo "Building with AddressSanitizer"
	@mkdir -p $(BUILD_DIR)/asan
	@cd $(BUILD_DIR)/asan && cmake \
		-G "$(CMAKE_GENERATOR)" \
		-DCMAKE_BUILD_TYPE=Debug \
		-DENABLE_ASAN=ON \
		../..
	@cmake --build $(BUILD_DIR)/asan
	@echo "Build Complete: ./router (with ASan)"
	@echo "Note: Run with ASAN_OPTIONS=detect_leaks=1 for leak detection"

ubsan:
	@echo "Building with UndefinedBehaviorSanitizer"
	@mkdir -p $(BUILD_DIR)/ubsan
	@cd $(BUILD_DIR)/ubsan && cmake \
		-G "$(CMAKE_GENERATOR)" \
		-DCMAKE_BUILD_TYPE=Debug \
		-DENABLE_UBSAN=ON \
		../..
	@cmake --build $(BUILD_DIR)/ubsan
	@echo "Build Complete: ./router (with UBSan)"

tsan:
	@echo "Building with ThreadSanitizer"
	@mkdir -p $(BUILD_DIR)/tsan
	@cd $(BUILD_DIR)/tsan && cmake \
		-G "$(CMAKE_GENERATOR)" \
		-DCMAKE_BUILD_TYPE=Debug \
		-DENABLE_TSAN=ON \
		../..
	@cmake --build $(BUILD_DIR)/tsan
	@echo "Build Complete: ./router (with TSan)"

sanitize:
	@echo "Building with ASan + UBSan"
	@mkdir -p $(BUILD_DIR)/sanitize
	@cd $(BUILD_DIR)/sanitize && cmake \
		-G "$(CMAKE_GENERATOR)" \
		-DCMAKE_BUILD_TYPE=Debug \
		-DENABLE_ASAN=ON \
		-DENABLE_UBSAN=ON \
		../..
	@cmake --build $(BUILD_DIR)/sanitize
	@echo "Build Complete: ./router (with ASan + UBSan)"


analyze:
	@echo "Building with Static Analysis"
	@mkdir -p $(BUILD_DIR)/analyze
	@cd $(BUILD_DIR)/analyze && cmake \
		-G "$(CMAKE_GENERATOR)" \
		-DCMAKE_BUILD_TYPE=Debug \
		-DENABLE_CLANG_TIDY=ON \
		-DENABLE_CPPCHECK=ON \
		../..
	@cmake --build $(BUILD_DIR)/analyze
	@echo "Analysis Complete"

test:
	@echo "Building and Running Tests with RapidCheck"
	@mkdir -p $(BUILD_DIR)/test
	@cd $(BUILD_DIR)/test && cmake \
		-G "$(CMAKE_GENERATOR)" \
		-DCMAKE_BUILD_TYPE=Debug \
		-DENABLE_TESTING=ON \
		../..
	@cmake --build $(BUILD_DIR)/test
	@cd $(BUILD_DIR)/test && ctest --output-on-failure
	@echo "Tests Complete"

test-asan:
	@echo "Building and Running Tests with ASan"
	@mkdir -p $(BUILD_DIR)/test-asan
	@cd $(BUILD_DIR)/test-asan && cmake \
		-G "$(CMAKE_GENERATOR)" \
		-DCMAKE_BUILD_TYPE=Debug \
		-DENABLE_TESTING=ON \
		-DENABLE_ASAN=ON \
		../..
	@cmake --build $(BUILD_DIR)/test-asan
	@cd $(BUILD_DIR)/test-asan && ctest --output-on-failure
	@echo "Tests Complete (with ASan)"

test-verbose:
	@echo "Building and Running Tests (Verbose)"
	@mkdir -p $(BUILD_DIR)/test
	@cd $(BUILD_DIR)/test && cmake \
		-G "$(CMAKE_GENERATOR)" \
		-DCMAKE_BUILD_TYPE=Debug \
		-DENABLE_TESTING=ON \
		../..
	@cmake --build $(BUILD_DIR)/test
	@cd $(BUILD_DIR)/test && ctest --output-on-failure --verbose
	@echo "Tests Complete"

format:
	@if command -v clang-format > /dev/null 2>&1; then \
		echo " Formatting source files "; \
		find src test -name '*.cpp' -o -name '*.hpp' 2>/dev/null | xargs clang-format -i; \
		echo " Formatting Complete "; \
	else \
		echo "Error: clang-format not found. Install with: sudo apt install clang-format"; \
		exit 1; \
	fi

clean:
	@echo " Cleaning build directory "
	@rm -rf $(BUILD_DIR)
	@rm -f router
	@rm -f compile_commands.json
	@find test -maxdepth 1 -type f -executable -delete 2>/dev/null || true
	@echo " Clean Complete "

distclean: clean
	@echo " Deep cleaning "
	@rm -rf .cache
	@rm -f .clang-tidy
	@rm -f .clang-format
	@echo " Distclean Complete "

compile-commands:
	@mkdir -p $(BUILD_DIR)/compile-commands
	@cd $(BUILD_DIR)/compile-commands && cmake \
		-DCMAKE_EXPORT_COMPILE_COMMANDS=ON \
		-DENABLE_TESTING=ON \
		../..
	@ln -sf $(BUILD_DIR)/compile-commands/compile_commands.json .
	@echo " compile_commands.json generated "

help:
	@echo "Router Project Build System"
	@echo ""
	@echo "Usage: make [target]"
	@echo ""
	@echo "Build Targets:"
	@echo "  release     - Build optimized release version"
	@echo "  debug       - Build debug version with symbols (default)"
	@echo ""
	@echo "Sanitizer Builds:"
	@echo "  asan        - Build with AddressSanitizer"
	@echo "  ubsan       - Build with UndefinedBehaviorSanitizer"
	@echo "  tsan        - Build with ThreadSanitizer"
	@echo "  sanitize    - Build with ASan + UBSan combined"
	@echo ""
	@echo "Testing:"
	@echo "  test        - Build and run property-based tests with RapidCheck"
	@echo "  test-asan   - Build and run tests with AddressSanitizer"
	@echo "  test-verbose- Build and run tests with verbose output"
	@echo ""
	@echo "Analysis:"
	@echo "  analyze     - Build with clang-tidy and cppcheck"
	@echo "  format      - Format source code with clang-format"
	@echo ""
	@echo "Other:"
	@echo "  clean       - Remove build artifacts"
	@echo "  distclean   - Deep clean including IDE files"
	@echo "  compile-commands - Generate compile_commands.json for IDEs"
	@echo "  help        - Show this help message"
	@echo ""
	@echo "Examples:"
	@echo "  make                    # Build debug"
	@echo "  make test               # Run property-based tests"
	@echo "  make test-asan          # Run tests with memory checking"
	@echo "  make sanitize && ./router -c cfg.txt -r routes.txt"
