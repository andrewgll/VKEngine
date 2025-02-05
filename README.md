Hi. This is my attempt to learn Vulkan and write my own small engine from scratch.

The Engine uses these conventions: Z forward, -Y up, triangulate faces.

Windows is not supported for now. (You can manually install everything and change Makefile if you want to)

Dependencies you need to build this engine:
* [VulkanSDK](https://vulkan.lunarg.com/sdk/home)
* [GLFW](https://www.glfw.org/)
* [tiny_obj_loader.h](https://github.com/tinyobjloader/tinyobjloader/blob/release/tiny_obj_loader.h)
* [Make](https://www.gnu.org/software/make/)
* [GLM](https://github.com/g-truc/glm)
* [stb image loader](https://github.com/nothings/stb/blob/master/stb_image.h)
* GLSLC(Should be included with VulkanSDK)

## How to build:
```sh
chmod +x ./install.sh
./install.sh
make
``` 
To run:
```sh
./bin/app
```

## Shortcuts
Press k to access to cursor


## So far these are my milestones:
### 0.20.1 UI system added (ImGui) 
<img src="https://i.ibb.co/XfpCp2NG/Screenshot-2025-02-04-at-19-13-49.png" width="300px">

### 0.19 Shadows with PCF + acne fix
<img src="https://i.ibb.co/wNhhx4fN/Screenshot-2025-01-31-at-13-39-15.png" width="300px">

### 0.18.5 Shadows(No Acne fix)

### 0.18 Physically based rendering(PBR), directional light, Mouse Controller
<img src="https://i.ibb.co/kcXntVg/Screenshot-2025-01-09-at-10-12-59.png" width="300px">

### 0.17 Multiple Texture loader, ObjManager, time in shaders
<img src="https://i.ibb.co/ZMp7GP4/Screenshot-2025-01-07-at-13-11-29.png" width="300px">

### 0.16 specular lighting

### 0.15 point lights, uniform buffers, descriptor sets.
<img src="https://i.ibb.co/G5gLdt8/Screenshot-2024-12-31-at-11-01-59.png" width="300px">

### 0.14 Custom model upload + lighting.
<img src="https://i.ibb.co/mJ4YHTk/Screenshot-2024-12-29-at-09-46-27.png" width="300px">

### 0.13 WASD movement.

### 0.12 Camera View Transformation.
<img src="https://i.ibb.co/ZVBhT9z/Screenshot-2024-12-27-at-10-36-14.png" width="300px">

### 0.11 Perspective Projection.

### 0.1 finally 3D.
<img src="https://i.ibb.co/BySfjdN/Screenshot-2024-12-26-at-22-09-28.png" width="300px">


### 0.09 Refactored everything.
### 0.08 Render system.
### 0.07 2D Transformations.
### 0.06 Resizable window.
### 0.05 Rainbow triangular.
### 0.04 First triangular in VK.
### 0.03 Command buffer created.
### 0.02 Fixed compatability issues with MacOS.
### 0.01 Vk Instance created, device, logical device, render pass, swapchain initialized. 
