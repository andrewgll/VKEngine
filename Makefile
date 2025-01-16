include .env

ASSEMBLY := Engine
BUILD_DIR := bin

COMPILER := clang++
COMPILER_FLAGS := -std=c++17
RELEASE_FLAGS := -O2 -DNDEBUG
DEBUG_FLAGS := -g -O0
INCLUDE_FLAGS := -IEngine/src -IEngine/include -I$(VULKAN_SDK)/include -I/opt/homebrew/include -Ilibs
LINKER_FLAGS := -lvulkan -lglfw -L/opt/homebrew/lib -Wl,-rpath,$(VULKAN_SDK)/lib -Wl,-rpath,/opt/homebrew/lib

SRC_FILES := $(shell find $(ASSEMBLY) -type f -name "*.cpp")
OBJ_FILES := $(patsubst %.cpp, $(BUILD_DIR)/%.o, $(SRC_FILES))

VERT_SOURCES := $(shell find ./Engine/shaders -type f -name "*.vert")
VERT_OBJ_FILES := $(patsubst %.vert, %.vert.spv, $(VERT_SOURCES))
FRAG_SOURCES := $(shell find ./Engine/shaders -type f -name "*.frag")
FRAG_OBJ_FILES := $(patsubst %.frag, %.frag.spv, $(FRAG_SOURCES))
SHADER_OBJ_FILES := $(VERT_OBJ_FILES) $(FRAG_OBJ_FILES)

build: $(BUILD_DIR) $(SHADER_OBJ_FILES)
	@echo "Building in Release mode..."
	@$(COMPILER) $(COMPILER_FLAGS) $(RELEASE_FLAGS) $(SRC_FILES) -o $(BUILD_DIR)/app $(INCLUDE_FLAGS) $(LINKER_FLAGS) $(CFLAGS)
	@echo "Release build completed! Executable created at $(BUILD_DIR)/app"

debug: $(BUILD_DIR) $(SHADER_OBJ_FILES)
	@echo "Building in Debug mode..."
	@$(COMPILER) $(COMPILER_FLAGS) $(DEBUG_FLAGS) $(SRC_FILES) -o $(BUILD_DIR)/app $(INCLUDE_FLAGS) $(LINKER_FLAGS) $(CFLAGS)
	@echo "Debug build completed! Executable created at $(BUILD_DIR)/app"

%.vert.spv: %.vert
	@echo "Compiling vertex shader: $<"
	@$(GLSLC) -o $@ $<

%.frag.spv: %.frag
	@echo "Compiling fragment shader: $<"
	@$(GLSLC) -o $@ $<

$(BUILD_DIR):
	@mkdir -p $(BUILD_DIR)

clean:
	@echo "Cleaning build directory..."
	@rm -rf $(BUILD_DIR)
	@echo "Clean completed!"
