Hi. This is my attempt to learn Vulkan and write my own small engine from scratch.

The Engine uses these conventions: Z forward, -Y up, triangulate faces.

Dependencies you need to run this engine:
* [VulkanSDK](https://vulkan.lunarg.com/sdk/home)
* [GLFW](https://www.glfw.org/)
* [tiny_obj_loader.h](https://github.com/tinyobjloader/tinyobjloader/blob/release/tiny_obj_loader.h)
* [Make](https://www.gnu.org/software/make/)
* [GLM](https://github.com/g-truc/glm)
* GLSLC(Should be included with VulkanSDK)

For now Makefile is compatible only with MacOS and homebrew.
## How to build:
```sh
make
``` 
for debug build:
```sh
make debug
```
To run:
```sh
./bin/app
```


## So far these are my milestones:
### 0.16 specular lighting
### 0.15 point lights, uniform buffers, descriptor sets.
![VK](https://i.ibb.co/G5gLdt8/Screenshot-2024-12-31-at-11-01-59.png)
### 0.14 Custom model upload + lighting.
![VK](https://i.ibb.co/mJ4YHTk/Screenshot-2024-12-29-at-09-46-27.png)
### 0.13 WASD movement.
### 0.12 Camera View Transformation.
![VK](https://i.ibb.co/ZVBhT9z/Screenshot-2024-12-27-at-10-36-14.png)

### 0.11 Perspective Projection.
### 0.1 finally 3D.
![VK](https://i.ibb.co/BySfjdN/Screenshot-2024-12-26-at-22-09-28.png)

### 0.09 Refactored everything.
### 0.08 Render system.
### 0.07 2D Transformations.
### 0.06 Resizable window.
### 0.05 Rainbow triangular.
### 0.04 First triangular in VK.
### 0.03 Command buffer created.
### 0.02 Fixed compatability issues with MacOS.
### 0.01 Vk Instance created, device, logical device, render pass, swapchain initialized. 
