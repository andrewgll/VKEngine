include .env

ASSEMBLY := Engine
BUILD_DIR := bin

COMPILER := clang++
COMPILER_FLAGS := -std=c++17
RELEASE_FLAGS := -O2 -DNDEBUG
DEBUG_FLAGS := -g -O0
INCLUDE_FLAGS := -IEngine/src -IEngine/include -I$(VULKAN_SDK)/include -I$(VULKAN_SDK)/lib -I/opt/homebrew/include
LINKER_FLAGS := -lvulkan -lglfw -L/opt/homebrew/lib

SRC_FILES := $(shell find $(ASSEMBLY) -type f -name "*.cpp")
OBJ_FILES := $(patsubst %.cpp, $(BUILD_DIR)/%.o, $(SRC_FILES))

build: $(BUILD_DIR)
	@echo "Building in Release mode..."
	@$(COMPILER) $(COMPILER_FLAGS) $(RELEASE_FLAGS) $(SRC_FILES) -o $(BUILD_DIR)/app $(INCLUDE_FLAGS) $(LINKER_FLAGS) $(CFLAGS)
	@echo "Release build completed! Executable created at $(BUILD_DIR)/app"

debug: $(BUILD_DIR)
	@echo "Building in Debug mode..."
	@$(COMPILER) $(COMPILER_FLAGS) $(DEBUG_FLAGS) $(SRC_FILES) -o $(BUILD_DIR)/app $(INCLUDE_FLAGS) $(LINKER_FLAGS) $(CFLAGS)
	@echo "Debug build completed! Executable created at $(BUILD_DIR)/app"

$(BUILD_DIR):
	@mkdir -p $(BUILD_DIR)

clean:
	@echo "Cleaning build directory..."
	@rm -rf $(BUILD_DIR)
	@echo "Clean completed!"
