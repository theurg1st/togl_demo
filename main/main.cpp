#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <iostream>
#include <sstream>
#include <vector>
#include <string>
#include <cstddef>
#include <stdexcept>
#include <fstream>
#include <chrono>
#include <iomanip>
#include <algorithm>

#include "shader.h"
#include "camera.h"
#include "model.h"
#include "render/MSAA.h"

#include <stb/stb_image.h>

// imgui
#include "imgui/imgui.h"
#include "backends/imgui_impl_glfw.h"
#include "backends/imgui_impl_opengl3.h"

// global
GLFWwindow* window = nullptr;
int g_MSAA = 4;
bool showConsole = false;
float lastTime = 0.0f;

Shader* phong = nullptr;
Shader* skyboxShader = nullptr;
Shader* screenShader = nullptr;
Model* model = nullptr;
unsigned int cubemap = 0;
unsigned int skyVAO = 0, skyVBO = 0;

// MSAA FBO
MSAA_FBO* msaa = nullptr;

// fullscreen quad
unsigned int screenVAO = 0, screenVBO = 0;

// logfile
std::ofstream logFile;

// timestamp
std::string GetCurrentTimeStamp() {
    auto now = std::chrono::system_clock::now();
    auto time_t = std::chrono::system_clock::to_time_t(now);
    auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(now.time_since_epoch()) % 1000;

    std::stringstream ss;
    ss << std::put_time(std::localtime(&time_t), "%Y-%m-%d %H:%M:%S");
    ss << "." << std::setfill('0') << std::setw(3) << ms.count();
    return ss.str();
}

// log init
void InitializeLogFile() {
    std::string filename = "togl_demo_log_" + GetCurrentTimeStamp() + ".txt";
    std::replace(filename.begin(), filename.end(), ':', '-');
    std::replace(filename.begin(), filename.end(), ' ', '_');

    logFile.open(filename, std::ios::out | std::ios::app);
    if (!logFile.is_open()) {
        std::cerr << "t_error_786: Cannot open log file: " << filename << std::endl;
        exit(1);
    }

    logFile << "=== log started at " << GetCurrentTimeStamp() << " ===" << std::endl;
    logFile.flush();
}

// generic log
void LogToFile(const std::string& level, const std::string& message) {
    if (logFile.is_open()) {
        logFile << "[" << GetCurrentTimeStamp() << "] [" << level << "] " << message << std::endl;
        logFile.flush();
    }

    if (level == "ERROR") {
        std::cerr << "[" << level << "] " << message << std::endl;
    } else {
        std::cout << "[" << level << "] " << message << std::endl;
    }
}

void LogInfo(const std::string& m)  { LogToFile("INFO",    m); }
void LogError(const std::string& m) { LogToFile("ERROR",   m); }
void LogWarning(const std::string& m){LogToFile("WARNING", m);}

// glfw error
void LogGLFWError(int error, const char* desc) {
    LogError("GLFW error " + std::to_string(error) + ": " + desc);
}

// gl error checker
void CheckGLError(const std::string& where) {
    GLenum e = glGetError();
    if (e != GL_NO_ERROR) {
        std::string msg;

        switch (e) {
            case GL_INVALID_ENUM: msg = "GL_INVALID_ENUM"; break;
            case GL_INVALID_VALUE: msg = "GL_INVALID_VALUE"; break;
            case GL_INVALID_OPERATION: msg = "GL_INVALID_OPERATION"; break;
            case GL_OUT_OF_MEMORY: msg = "GL_OUT_OF_MEMORY"; break;
            case GL_INVALID_FRAMEBUFFER_OPERATION: msg = "GL_INVALID_FRAMEBUFFER_OPERATION"; break;
            default: msg = "UNKNOWN_ERROR_" + std::to_string(e); break;
        }

        LogError("OpenGL error at " + where + ": " + msg);
    }
}

// print sysinfo
void LogSystemInfo() {
    LogInfo("=== system info ===");
    LogInfo("Application: togl_demo");

#ifdef _DEBUG
    LogInfo("Build: Debug");
#else
    LogInfo("Build: Release");
#endif

#ifdef _WIN32
    LogInfo("Platform: Windows");
#elif __linux__
    LogInfo("Platform: Linux");
#elif __APPLE__
    LogInfo("Platform: macOS");
#endif
}

// fullscreen quad initialization
void InitScreenQuad() {
    float quadVertices[] = {
        //   pos      uv
        -1, -1,   0, 0,
         1, -1,   1, 0,
         1,  1,   1, 1,

        -1, -1,   0, 0,
         1,  1,   1, 1,
        -1,  1,   0, 1
    };

    glGenVertexArrays(1, &screenVAO);
    glGenBuffers(1, &screenVBO);

    glBindVertexArray(screenVAO);

    glBindBuffer(GL_ARRAY_BUFFER, screenVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), quadVertices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));
    glEnableVertexAttribArray(1);
}

// cleanup
void CleanupResources() {
    LogInfo("cleanup…");

    delete phong;
    delete skyboxShader;
    delete screenShader;
    delete model;
    delete msaa;

    if (skyVAO) glDeleteVertexArrays(1, &skyVAO);
    if (skyVBO) glDeleteBuffers(1, &skyVBO);
    if (cubemap) glDeleteTextures(1, &cubemap);
    if (screenVAO) glDeleteVertexArrays(1, &screenVAO);
    if (screenVBO) glDeleteBuffers(1, &screenVBO);

    phong = nullptr;
    skyboxShader = nullptr;
    screenShader = nullptr;
    model = nullptr;
    msaa = nullptr;

    skyVAO = skyVBO = cubemap = 0;
    screenVAO = screenVBO = 0;
}

// skybox setup
void SetupSkybox() {
    float vertices[] = {
        -1,-1, 1,  1,-1, 1,  1,1,1,
        1,1,1,  -1,1,1, -1,-1,1,

        -1,-1,-1, 1,-1,-1, 1,1,-1,
        1,1,-1, -1,1,-1, -1,-1,-1,

        -1,1,-1, -1,1,1, -1,-1,1,
        -1,-1,1, -1,-1,-1, -1,1,-1,

        1,1,-1, 1,1,1, 1,-1,1,
        1,-1,1, 1,-1,-1, 1,1,-1,

        -1,-1,-1, 1,-1,-1, 1,-1,1,
        1,-1,1, -1,-1,1, -1,-1,-1,

        -1,1,-1, 1,1,-1, 1,1,1,
        1,1,1, -1,1,1, -1,1,-1
    };

    glGenVertexArrays(1, &skyVAO);
    glGenBuffers(1, &skyVBO);
    glBindVertexArray(skyVAO);

    glBindBuffer(GL_ARRAY_BUFFER, skyVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3*sizeof(float), (void*)0);

    LogInfo("skybox ok");
}

// cubemap
unsigned int LoadCubemap(const std::vector<std::string>& faces) {
    unsigned int tex;
    glGenTextures(1, &tex);
    glBindTexture(GL_TEXTURE_CUBE_MAP, tex);

    int w, h, ch;

    for (unsigned int i=0; i<faces.size(); i++) {
        unsigned char* data = stbi_load(faces[i].c_str(), &w, &h, &ch, 3);

        if (!data) {
            LogError("cubemap fail: " + faces[i]);
            throw std::runtime_error("cubemap load failed");
        }

        glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0,
                     GL_RGB, w, h, 0, GL_RGB, GL_UNSIGNED_BYTE, data);

        stbi_image_free(data);
    }

    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    return tex;
}

// runtime MSAA
void ApplyMSAA(int samples) {
    g_MSAA = samples;
    
    if (msaa) {
        msaa->recreate(samples);
        LogInfo("Recreated MSAA FBO with " + std::to_string(samples) + " samples");
    }
}

// init resources
void InitializeResources() {
    try {
        phong = new Shader("shaders/phong.vert","shaders/phong.frag");
        if (!phong) throw std::runtime_error("phong = nullptr");

        skyboxShader = new Shader("shaders/skybox.vert","shaders/skybox.frag");
        if (!skyboxShader) throw std::runtime_error("skyboxShader = nullptr");

        // screen shader for MSAA resolve
        screenShader = new Shader("shaders/screen.vert", "shaders/screen.frag");
        if (!screenShader) throw std::runtime_error("screenShader = nullptr");

        model = new Model("assets/glb/model_nvidia.glb");
        if (!model) throw std::runtime_error("model = nullptr");

        std::vector<std::string> faces = {
            "assets/textures/skybox/right.png",
            "assets/textures/skybox/left.png",
            "assets/textures/skybox/top.png",
            "assets/textures/skybox/bottom.png",
            "assets/textures/skybox/front.png",
            "assets/textures/skybox/back.png"
        };

        cubemap = LoadCubemap(faces);
        SetupSkybox();

        // initialize screen quad for MSAA resolve
        InitScreenQuad();

        // create MSAA FBO
        int width, height;
        glfwGetFramebufferSize(window, &width, &height);
        msaa = new MSAA_FBO(width, height, g_MSAA);

        glEnable(GL_DEPTH_TEST);

    } catch(const std::exception& e) {
        LogError("resource init failed: " + std::string(e.what()));
        CleanupResources();
        throw;
    }
}

// command handler
void ExecuteCommand(const std::string& cmd) {
    LogInfo("cmd: " + cmd);

    std::stringstream ss(cmd);
    std::string name;
    ss >> name;

    if (name == "t_msaa") {
        int x;
        ss >> x;

        if (x==0 || x==2 || x==4 || x==8) {
            ApplyMSAA(x);
        } else {
            LogWarning("invalid msaa");
        }
        return;
    }

    if (name == "info") {
        std::cout << "OpenGL: " << glGetString(GL_VERSION) << "\n";
        std::cout << "GPU: " << glGetString(GL_RENDERER) << "\n";
        return;
    }

    if (name == "help") {
        std::cout << "commands: help, info, t_msaa X\n";
        return;
    }

    LogWarning("unknown cmd");
}

// GLFW init
bool InitializeGLFW() {
    LogInfo("init GLFW...");

    if (!glfwInit()) {
        LogError("glfw init fail");
        return false;
    }

    glfwSetErrorCallback(LogGLFWError);

    glfwWindowHint(GLFW_SAMPLES, g_MSAA);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    return true;
}

bool CreateInitialWindow() {

    glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE); // disable resizing

    window = glfwCreateWindow(1280,720,"togl_demo",NULL,NULL);
    if (!window) {
        LogError("window create fail");
        return false;
    }

    glfwMakeContextCurrent(window);

    GLFWimage icon;
    int channels = 0;

    icon.pixels = stbi_load("assets/ico/icon.png",
        &icon.width,
        &icon.height,
        &channels,
        4);

    if (icon.pixels) {
        glfwSetWindowIcon(window, 1, &icon);
        stbi_image_free(icon.pixels);
    } else {
        std::cerr << "failed to load window icon!" << std::endl;
    }

    return true;
}



// opengl
bool InitializeOpenGL() {
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        LogError("glad fail");
        return false;
    }

    glEnable(GL_MULTISAMPLE);

    return true;
}

// imgui
bool InitializeImGui() {
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGui::StyleColorsDark();

    if (!ImGui_ImplGlfw_InitForOpenGL(window,true)) return false;
    if (!ImGui_ImplOpenGL3_Init("#version 330")) return false;

    return true;
}

// main
int main() {
    InitializeLogFile();
    LogSystemInfo();

    try {
        if (!InitializeGLFW()) throw std::runtime_error("glfw init fail");
        if (!CreateInitialWindow()) throw std::runtime_error("window fail");
        if (!InitializeOpenGL()) throw std::runtime_error("opengl fail");
        if (!InitializeImGui()) throw std::runtime_error("imgui fail");

        InitializeResources();

        Camera cam(1280,720);
        float rotationX = 0;

        // toggle fix
        bool f1Held = false;

        while (!glfwWindowShouldClose(window)) {
            float t = glfwGetTime();
            float dt = t - lastTime;
            lastTime = t;

            glfwPollEvents();

            // toggle console (fixed)
            if (glfwGetKey(window, GLFW_KEY_F1) == GLFW_PRESS) {
                if (!f1Held) {
                    showConsole = !showConsole;
                    f1Held = true;
                }
            } else {
                f1Held = false;
            }

            glBindFramebuffer(GL_FRAMEBUFFER, msaa->fbo_msaa);
            glClearColor(0.1f,0.1f,0.2f,1.0f);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

            // animate model
            rotationX += dt * 0.5f;

            glm::mat4 view = cam.getViewMatrix();
            glm::mat4 proj = cam.getProjectionMatrix();

            // model
            if (phong && model) {
                try {
                    phong->use();
                    phong->setMat4("view", view);
                    phong->setMat4("projection", proj);
                    phong->setMat4("model", model->getModelMatrix(rotationX));
                    phong->setVec3("lightPos", glm::vec3(0,2,2));
                    phong->setVec3("lightColor", glm::vec3(1,1,1));
                    phong->setFloat("ambientStrength", 0.4f);
                    phong->setVec3("viewPos", cam.position);
                    model->draw();
                } catch (...) {
                    LogError("model render fail");
                }
            }

            // skybox
            try {
                glDepthFunc(GL_LEQUAL);
                skyboxShader->use();

                glm::mat4 viewNoTrans = glm::mat4(glm::mat3(view));
                skyboxShader->setMat4("view", viewNoTrans);
                skyboxShader->setMat4("projection", proj);

                glBindVertexArray(skyVAO);
                glActiveTexture(GL_TEXTURE0);
                glBindTexture(GL_TEXTURE_CUBE_MAP, cubemap);

                glDrawArrays(GL_TRIANGLES, 0, 36);

                glDepthFunc(GL_LESS);
            } catch (...) {
                LogError("skybox fail");
            }

            msaa->resolve();

            glBindFramebuffer(GL_FRAMEBUFFER, 0);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

            screenShader->use();
            glBindVertexArray(screenVAO);
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, msaa->tex_resolved);
            glDrawArrays(GL_TRIANGLES, 0, 6);

            // imgui
            ImGui_ImplOpenGL3_NewFrame();
            ImGui_ImplGlfw_NewFrame();
            ImGui::NewFrame();

            if (showConsole) {
                ImGui::Begin("Console", NULL, ImGuiWindowFlags_AlwaysAutoResize);
                static char buf[256];

                if (ImGui::InputText("cmd", buf, 256, ImGuiInputTextFlags_EnterReturnsTrue)) {
                    ExecuteCommand(buf);
                    buf[0] = 0;
                }

                ImGui::Text("FPS = %.1f", 1.0f / dt);
                ImGui::Text("FrameTime = %.3f ms", dt * 1000.0f);
                ImGui::Text("MSAA = %dx", g_MSAA);

                ImGui::End();
            }

            ImGui::Render();
            ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

            glfwSwapBuffers(window);
            CheckGLError("MainLoop");
        }

    } catch(const std::exception& e) {
        LogError("fatal: " + std::string(e.what()));
    }

    // shutdown
    CleanupResources();

    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    if (window) glfwDestroyWindow(window);
    glfwTerminate();

    if (logFile.is_open()) {
        logFile << "=== log ended at " << GetCurrentTimeStamp() << " ===" << std::endl;
        logFile.close();
    }

    return 0;
}

/*  

Author: theurg1st  
Website: https://theurg1st.github.io

========================================
License
========================================

This project is released under the MIT License.  
You may use, modify, or redistribute the source code with attribution.

========================================
Credits
========================================

Made by theurg1st

*/