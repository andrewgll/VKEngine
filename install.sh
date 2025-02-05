#!/bin/bash

LIBS_DIR="libs"

if [ ! -d "$LIBS_DIR" ]; then
    mkdir -p "$LIBS_DIR"
fi

echo "Downloading required dependencies into $LIBS_DIR..."

# Download GLFW
echo "Downloading GLFW..."
GLFW_URL="https://github.com/glfw/glfw/releases/download/3.4/glfw-3.4.bin.MACOS.zip"
GLFW_ZIP="$LIBS_DIR/glfw.zip"

curl -L "$GLFW_URL" -o "$GLFW_ZIP"
unzip -q "$GLFW_ZIP" -d "$LIBS_DIR"
mv "$LIBS_DIR/glfw-3.4.bin.MACOS/include/GLFW" "$LIBS_DIR/"
rm -rf "$LIBS_DIR/glfw-3.4.bin.MACOS"
rm "$GLFW_ZIP"
echo "GLFW downloaded."

# Download tiny_obj_loader
echo "Downloading tiny_obj_loader..."
curl -L "https://raw.githubusercontent.com/tinyobjloader/tinyobjloader/release/tiny_obj_loader.h" -o "$LIBS_DIR/tiny_obj_loader.h"
echo "tiny_obj_loader downloaded."

# Download GLM
echo "Downloading GLM..."
GLM_URL="https://github.com/g-truc/glm/releases/download/0.9.9.8/glm-0.9.9.8.zip"
GLM_ZIP="$LIBS_DIR/glm.zip"
curl -L "$GLM_URL" -o "$GLM_ZIP"
unzip -q "$GLM_ZIP" -d "$LIBS_DIR/glm_temp"
mv "$LIBS_DIR/glm_temp/glm" "$LIBS_DIR/"
rm -rf "$LIBS_DIR/glm_temp"
rm "$GLM_ZIP"
echo "GLM downloaded."

# Download Dear ImGui
echo "Downloading Dear ImGui..."
IMGUI_URL="https://github.com/ocornut/imgui/archive/refs/heads/master.zip"
IMGUI_ZIP="$LIBS_DIR/imgui.zip"
curl -L "$IMGUI_URL" -o "$IMGUI_ZIP"
unzip -q "$IMGUI_ZIP" -d "$LIBS_DIR/imgui_temp"
mkdir -p "$LIBS_DIR/imgui"
mv "$LIBS_DIR/imgui_temp/imgui-master/"* "$LIBS_DIR/imgui"
rm -rf "$LIBS_DIR/imgui_temp"
rm "$IMGUI_ZIP"
mv "$LIBS_DIR/imgui/backends/imgui_impl_glfw.h" "$LIBS_DIR/imgui"
mv "$LIBS_DIR/imgui/backends/imgui_impl_glfw.cpp" "$LIBS_DIR/imgui"
mv "$LIBS_DIR/imgui/backends/imgui_impl_vulkan.h" "$LIBS_DIR/imgui"
mv "$LIBS_DIR/imgui/backends/imgui_impl_vulkan.cpp" "$LIBS_DIR/imgui"
rm -rf "$LIBS_DIR/imgui/backends"
rm -rf "$LIBS_DIR/imgui/examples"
rm "$LIBS_DIR/imgui/misc/fonts/binary_to_compressed_c.cpp" # TD : figure out the correct way of doing this
echo "Dear ImGui downloaded."

# Download FreeType
echo "Downloading FreeType ft2build.h..."
FREETYPE_URL="https://download.savannah.gnu.org/releases/freetype/freetype-2.13.2.tar.gz"
FREETYPE_TAR="$LIBS_DIR/freetype.tar.gz"
curl -L "$FREETYPE_URL" -o "$FREETYPE_TAR"
mkdir -p "$LIBS_DIR/freetype_temp"
tar -xzf "$FREETYPE_TAR" -C "$LIBS_DIR/freetype_temp" --strip-components=1
mv "$LIBS_DIR/freetype_temp/include/ft2build.h" "$LIBS_DIR/"
mv "$LIBS_DIR/freetype_temp/include/freetype" "$LIBS_DIR/"
rm -rf "$LIBS_DIR/freetype_temp"
rm "$FREETYPE_TAR"
echo "FreeType ft2build.h downloaded."

# Download stb_image
echo "Downloading stb_image..."
curl -L "https://raw.githubusercontent.com/nothings/stb/master/stb_image.h" -o "$LIBS_DIR/stb_image.h"
echo "stb_image downloaded."

# Download SDL
SDL_REPO="https://github.com/libsdl-org/SDL.git"
SDL_TEMP="$LIBS_DIR/sdl_temp"
if [ ! -d "$SDL_TEMP" ]; then
    git clone "$SDL_REPO" "$SDL_TEMP"
fi
mkdir -p "$LIBS_DIR/SDL3"
mv "$SDL_TEMP/include/SDL3/"* "$LIBS_DIR/SDL3/"
rm -rf "$SDL_TEMP"
echo "SDL downloaded."



echo "All dependencies downloaded successfully!"
