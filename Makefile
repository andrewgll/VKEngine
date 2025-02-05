include .env

ASSEMBLY := Engine
BUILD_DIR := bin

ifeq ($(OS),Windows_NT)
	UNAME_S := $(OS)
    COMPILER := g++
    COMPILER_FLAGS := -std=c++17 -m64
    RELEASE_FLAGS := -O2 -DNDEBUG
    DEBUG_FLAGS := -g -O0
    INCLUDE_FLAGS := -IEngine/src -IEngine/include -I/mingw64/include -I$(VULKAN_SDK)/Include -Ilibs -Ilibs/glfw/include
    LINKER_FLAGS := -L$(VULKAN_SDK)/Lib -lvulkan-1 -Llibs/glfw/lib-mingw-w64 -lglfw3 -lgdi32 -luser32

    GLSLC := $(VULKAN_SDK)/Bin/glslc.exe
    MKDIR := mkdir
    RM := rmdir /s /q
    
    SRC_FILES := $(shell dir /b /s Engine\src\*.cpp)
    OBJ_FILES := $(patsubst %.cpp, $(BUILD_DIR)/%.o, $(SRC_FILES))

    VERT_SOURCES := $(shell dir /b /s Engine\shaders\*.vert)
    VERT_OBJ_FILES := $(patsubst %.vert, %.vert.spv, $(VERT_SOURCES))
    FRAG_SOURCES := $(shell dir /b /s Engine\shaders\*.frag)
    FRAG_OBJ_FILES := $(patsubst %.frag, %.frag.spv, $(FRAG_SOURCES))
    SHADER_OBJ_FILES := $(VERT_OBJ_FILES) $(FRAG_OBJ_FILES)
else
	UNAME_S := $(shell uname -s)
	ifeq ($(UNAME_S), Darwin)
		COMPILER := clang++
		COMPILER_FLAGS := -std=c++17
		RELEASE_FLAGS := -O2 -DNDEBUG
		DEBUG_FLAGS := -g -O0
		
		INCLUDE_FLAGS := -IEngine/src -IEngine/include -I$(VULKAN_SDK)/include -I/opt/homebrew/include -Ilibs -Ilibs/imgui
		LINKER_FLAGS := -lvulkan -lglfw -L/opt/homebrew/lib -Wl,-rpath,$(VULKAN_SDK)/lib -Wl,-rpath,/opt/homebrew/lib -lfreetype

    	RM := rm -r
		MKDIR := mkdir -p

		SRC_FILES := $(shell find $(ASSEMBLY) -type f -name "*.cpp")
		OBJ_FILES := $(patsubst %.cpp, $(BUILD_DIR)/%.o, $(SRC_FILES))

		# Add ImGui source files
		IMGUI_SRC_FILES := $(shell find libs/imgui -type f -name "*.cpp")
		IMGUI_OBJ_FILES := $(patsubst %.cpp, $(BUILD_DIR)/%.o, $(IMGUI_SRC_FILES))

		VERT_SOURCES := $(shell find ./Engine/shaders -type f -name "*.vert")
		VERT_OBJ_FILES := $(patsubst %.vert, %.vert.spv, $(VERT_SOURCES))
		FRAG_SOURCES := $(shell find ./Engine/shaders -type f -name "*.frag")
		FRAG_OBJ_FILES := $(patsubst %.frag, %.frag.spv, $(FRAG_SOURCES))
		SHADER_OBJ_FILES := $(VERT_OBJ_FILES) $(FRAG_OBJ_FILES)
	endif
endif

$(BUILD_DIR)/%.o: %.cpp | $(BUILD_DIR)
	@mkdir -p $(@D)
	@echo "Compiling: $<"
	@$(COMPILER) $(COMPILER_FLAGS) $(INCLUDE_FLAGS) -c $< -o $@

build: $(BUILD_DIR) $(SHADER_OBJ_FILES) $(OBJ_FILES) $(IMGUI_OBJ_FILES)
	@echo "Building in Release mode"
	@$(COMPILER) $(COMPILER_FLAGS) $(RELEASE_FLAGS) $(OBJ_FILES) $(IMGUI_OBJ_FILES) -o $(BUILD_DIR)/app $(INCLUDE_FLAGS) $(LINKER_FLAGS)
	@echo "Release build completed! Executable created at $(BUILD_DIR)/app"

debug: $(BUILD_DIR) $(SHADER_OBJ_FILES) $(OBJ_FILES) $(IMGUI_OBJ_FILES)
	@echo "Building in Debug mode" 
	@$(COMPILER) $(COMPILER_FLAGS) $(DEBUG_FLAGS) $(OBJ_FILES) $(IMGUI_OBJ_FILES) -o $(BUILD_DIR)/app $(INCLUDE_FLAGS) $(LINKER_FLAGS)
	@echo "Debug build completed! Executable created at $(BUILD_DIR)/app"

%.vert.spv: %.vert
	@echo "Compiling vertex shader: $<"
	@$(GLSLC) -o $@ $<

%.frag.spv: %.frag
	@echo "Compiling fragment shader: $<"
	@$(GLSLC) -o $@ $<

$(BUILD_DIR):
	@echo "Creating build directory..."
	@$(MKDIR) $(BUILD_DIR)

clean:
	@echo "Cleaning build directory..."
	@$(RM) $(BUILD_DIR)
	@echo "Clean completed!"