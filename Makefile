# Router Project Makefile
# Usage:
#   make              - Build debug version (default)
#   make release      - Build optimized release version
#   make debug        - Build debug version with symbols
#   make asan         - Build with AddressSanitizer
#   make ubsan        - Build with UndefinedBehaviorSanitizer
#   make sanitize     - Build with both ASan and UBSan
#   make test         - Build and run tests (requires RapidCheck)
#   make clean        - Remove build artifacts
#   make format       - Format source code with clang-format
#   make help         - Show this help

# Compiler
CXX := g++

# Directories
SRC_DIR := src
TEST_DIR := test
BUILD_DIR := build
OBJ_DIR := $(BUILD_DIR)/obj
TEST_OBJ_DIR := $(BUILD_DIR)/test_obj

# Target
TARGET := router

# Source files (add new source files here)
LIB_SOURCES := \
    $(SRC_DIR)/RouteTable.cpp \
    $(SRC_DIR)/IpAddress.cpp \
    $(SRC_DIR)/Router.cpp \
    $(SRC_DIR)/Interface.cpp \
    $(SRC_DIR)/FileParser.cpp \
    $(SRC_DIR)/ArgParser.cpp

MAIN_SOURCE := $(SRC_DIR)/main.cpp

# Object files
LIB_OBJECTS := $(patsubst $(SRC_DIR)/%.cpp,$(OBJ_DIR)/%.o,$(LIB_SOURCES))
MAIN_OBJECT := $(OBJ_DIR)/main.o

# C++ Standard and common flags
CXXSTD := -std=c++17
INCLUDES := -I$(SRC_DIR)

# Warning flags
WARNINGS := \
    -Wall \
    -Wextra \
    -Wpedantic \
    -Wshadow \
    -Wnon-virtual-dtor \
    -Wold-style-cast \
    -Wcast-align \
    -Wunused \
    -Woverloaded-virtual \
    -Wconversion \
    -Wsign-conversion \
    -Wnull-dereference \
    -Wdouble-promotion \
    -Wformat=2 \
    -Wimplicit-fallthrough \
    -Werror=return-type

# GCC-specific warnings
ifeq ($(CXX),g++)
    WARNINGS += \
        -Wmisleading-indentation \
        -Wduplicated-cond \
        -Wduplicated-branches \
        -Wlogical-op \
        -Wuseless-cast
endif

# Build type flags
DEBUG_FLAGS := -g3 -O0 -DDEBUG -fno-omit-frame-pointer
RELEASE_FLAGS := -O3 -DNDEBUG -march=native

# Sanitizer flags
ASAN_FLAGS := -fsanitize=address -fno-omit-frame-pointer
UBSAN_FLAGS := -fsanitize=undefined

# Default build type
BUILD_TYPE ?= debug

# Set flags based on build type
ifeq ($(BUILD_TYPE),release)
    CXXFLAGS := $(CXXSTD) $(INCLUDES) $(WARNINGS) $(RELEASE_FLAGS)
else ifeq ($(BUILD_TYPE),asan)
    CXXFLAGS := $(CXXSTD) $(INCLUDES) $(WARNINGS) $(DEBUG_FLAGS) $(ASAN_FLAGS)
    LDFLAGS += $(ASAN_FLAGS)
else ifeq ($(BUILD_TYPE),ubsan)
    CXXFLAGS := $(CXXSTD) $(INCLUDES) $(WARNINGS) $(DEBUG_FLAGS) $(UBSAN_FLAGS)
    LDFLAGS += $(UBSAN_FLAGS)
else ifeq ($(BUILD_TYPE),sanitize)
    CXXFLAGS := $(CXXSTD) $(INCLUDES) $(WARNINGS) $(DEBUG_FLAGS) $(ASAN_FLAGS) $(UBSAN_FLAGS)
    LDFLAGS += $(ASAN_FLAGS) $(UBSAN_FLAGS)
else
    CXXFLAGS := $(CXXSTD) $(INCLUDES) $(WARNINGS) $(DEBUG_FLAGS)
endif

# Default target
.DEFAULT_GOAL := debug

# Phony targets
.PHONY: all release debug asan ubsan sanitize clean format help

# Build Targets
all: debug

debug:
	@$(MAKE) $(TARGET) BUILD_TYPE=debug

release:
	@$(MAKE) $(TARGET) BUILD_TYPE=release

asan:
	@$(MAKE) $(TARGET) BUILD_TYPE=asan

ubsan:
	@$(MAKE) $(TARGET) BUILD_TYPE=ubsan

sanitize:
	@$(MAKE) $(TARGET) BUILD_TYPE=sanitize

# Main target
$(TARGET): $(LIB_OBJECTS) $(MAIN_OBJECT)
	@echo "Linking $(TARGET)"
	@$(CXX) $(CXXFLAGS) $^ -o $@ $(LDFLAGS)
	@echo "Build complete: ./$(TARGET)"

# Compile source files
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp | $(OBJ_DIR)
	@echo "Compiling $<"
	@$(CXX) $(CXXFLAGS) -c $< -o $@

# Create build directories
$(OBJ_DIR):
	@mkdir -p $(OBJ_DIR)

format:
	@if command -v clang-format > /dev/null 2>&1; then \
	    echo "Formatting source files..."; \
	    find $(SRC_DIR) $(TEST_DIR) -name '*.cpp' -o -name '*.hpp' 2>/dev/null | xargs clang-format -i; \
	    echo "Formatting complete"; \
	else \
	    echo "Error: clang-format not found"; \
	    exit 1; \
	fi

clean:
	@echo "Cleaning..."
	@rm -rf $(BUILD_DIR)
	@rm -f $(TARGET)
	@rm -f $(TEST_DIR)/Test*
	@rm -f /tmp/out1.txt /tmp/out2.txt
	@echo "Clean complete"

help:
	@echo "Router Project Build System"
	@echo ""
	@echo "Usage: make [target]"
	@echo ""
	@echo "Build Targets:"
	@echo "  debug       - Build debug version with symbols (default)"
	@echo "  release     - Build optimized release version"
	@echo ""
	@echo "Sanitizer Builds:"
	@echo "  asan        - Build with AddressSanitizer"
	@echo "  ubsan       - Build with UndefinedBehaviorSanitizer"
	@echo "  sanitize    - Build with ASan + UBSan combined"
	@echo ""
	@echo "Utilities:"
	@echo "  format      - Format source code with clang-format"
	@echo "  clean       - Remove build artifacts"
	@echo "  help        - Show this help message"
	@echo ""
	@echo "Examples:"
	@echo "  make                  # Build debug"
	@echo "  make release          # Build optimized"
	@echo "  make test-samples     # Test against expected output"
	@echo "  make sanitize && ./router -c cfg.txt -r routes.txt"
