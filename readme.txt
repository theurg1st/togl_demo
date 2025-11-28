  __  __           _           _                _   _                          _     _   
 |  \/  | __ _  __| | ___     | |__  _   _     | |_| |__   ___ _   _ _ __ __ _/ |___| |_ 
 | |\/| |/ _` |/ _` |/ _ \    | '_ \| | | |    | __| '_ \ / _ \ | | | '__/ _` | / __| __|
 | |  | | (_| | (_| |  __/    | |_) | |_| |    | |_| | | |  __/ |_| | | | (_| | \__ \ |_ 
 |_|  |_|\__,_|\__,_|\___|    |_.__/ \__, |     \__|_| |_|\___|\__,_|_|  \__, |_|___/\__|
                                     |___/                               |___/           

OpenGL Demo (C++ / GLFW / GLAD / imgui / glm / tinygltf)
Author: theurg1st  
Website: https://theurg1st.github.io

========================================
Project structure
========================================

- main/ ............. Main code (main.cpp + camera/shader/model)
- render/ ........... MSAA FBO
- include/
    - glad/ ......... 
    - GLFW/ ......... 
    - glm/ .......... 
    - imgui/ ........ 
    - backends/ ..... 
    - stb/ .......... 
    - tiny_gltf/ .... 
- assets/
    - glb/ .......... 
    - textures/ ..... 
- lib/ .............. 

========================================
Required libraries
========================================

**All libraries and include folders MUST be downloaded manually.  
They are NOT included with this project.**

You must provide:

- GLFW
- GLAD
- GLM
- stb_image.h
- tinygltf
- imgui sources + backends:
  - `imgui_impl_glfw.cpp`
  - `imgui_impl_opengl3.cpp`

========================================
How to build (MinGW g++)
========================================

```
g++ -std=c++17 main/main.cpp src/glad.c include/tiny_gltf/tiny_gltf.cc include/imgui/imgui.cpp include/imgui/imgui_draw.cpp include/imgui/imgui_tables.cpp include/imgui/imgui_widgets.cpp include/imgui/imgui_demo.cpp include/backends/imgui_impl_glfw.cpp include/backends/imgui_impl_opengl3.cpp -I include -I include/imgui -I include/backends -I include/glad -I include/GLFW -I include/glm -I include/stb -I include/tiny_gltf -I render -L lib -lglfw3dll -lopengl32 -lgdi32 -luser32 -lshell32 -lkernel32 icon.res -o togl_demo.exe
```

========================================
Runtime console commands
========================================

Inside the ImGui console:

- `help`  
  Shows available commands  

- `info`  
  Prints GPU + OpenGL version  

- `t_msaa X`  
  Changes MSAA sample count (0, 2, 4, 8)  
  Recreates MSAA framebuffers at runtime  

========================================
License
========================================

This project is released under the MIT License.  
You may use, modify, or redistribute the source code with attribution.

========================================
Credits
========================================

Made by theurg1st
