togl_demo
---------

A small OpenGL 3.3 demo written in C++.  
Shows a rotating GLB model, a skybox, MSAA rendering and a simple ImGui console.


## Demo
[[demo](https://github.com/user-attachments/assets/86da620a-e173-4bba-8d67-5e54edaef4cb)](https://github-production-user-asset-6210df.s3.amazonaws.com/236650562/520363374-86da620a-e173-4bba-8d67-5e54edaef4cb.gif?X-Amz-Algorithm=AWS4-HMAC-SHA256&X-Amz-Credential=AKIAVCODYLSA53PQK4ZA%2F20251129%2Fus-east-1%2Fs3%2Faws4_request&X-Amz-Date=20251129T090533Z&X-Amz-Expires=300&X-Amz-Signature=52e027402748c45efdebf9c3fbc74e02e298050c144eda7579d979a86977d1d7&X-Amz-SignedHeaders=host)


## Features
- GLB model loading (tinygltf)
- Skybox rendering
- MSAA (0/2/4/8x) with runtime switching
- Simple ImGui console (F1)
- Custom MSAA framebuffer resolve
- Logging system with timestamps


## Build
You must provide all required libraries yourself (GLFW, GLAD, ImGui, GLM, stb_image, tinygltf).

Example MinGW build command:

```
g++ -std=c++17 main/main.cpp src/glad.c include/tiny_gltf/tiny_gltf.cc include/imgui/imgui.cpp include/imgui/imgui_draw.cpp include/imgui/imgui_tables.cpp include/imgui/imgui_widgets.cpp include/imgui/imgui_demo.cpp include/backends/imgui_impl_glfw.cpp include/backends/imgui_impl_opengl3.cpp -I include -I include/imgui -I include/backends -I include/glad -I include/GLFW -I include/glm -I include/stb -I include/tiny_gltf -I render -L lib -lglfw3dll -lopengl32 -lgdi32 -luser32 -lshell32 -lkernel32 icon.res -o togl_demo.exe
```
## License
```
MIT License!


Copyright (c) 2025 theurg1st

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
```
## Authors

- [@theurg1st](https://www.github.com/theurg1st)

