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

# Download stb_image
echo "Downloading stb_image..."
curl -L "https://raw.githubusercontent.com/nothings/stb/master/stb_image.h" -o "$LIBS_DIR/stb_image.h"
echo "stb_image downloaded."

echo "All dependencies downloaded successfully!"
