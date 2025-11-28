#pragma once
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>

#include <glad/glad.h>
#include <glm/glm.hpp>

class Shader {
public:
    unsigned int ID;

    Shader(const char* vertexPath, const char* fragmentPath) {
        std::string vCode, fCode;

        std::ifstream vFile(vertexPath);
        std::stringstream vStream;
        vStream << vFile.rdbuf();
        vCode = vStream.str();

        std::ifstream fFile(fragmentPath);
        std::stringstream fStream;
        fStream << fFile.rdbuf();
        fCode = fStream.str();

        const char* vShaderCode = vCode.c_str();
        const char* fShaderCode = fCode.c_str();

        unsigned int vertex = glCreateShader(GL_VERTEX_SHADER);
        glShaderSource(vertex, 1, &vShaderCode, NULL);
        glCompileShader(vertex);
        checkCompileErrors(vertex, "VERTEX");

        unsigned int fragment = glCreateShader(GL_FRAGMENT_SHADER);
        glShaderSource(fragment, 1, &fShaderCode, NULL);
        glCompileShader(fragment);
        checkCompileErrors(fragment, "FRAGMENT");

        ID = glCreateProgram();
        glAttachShader(ID, vertex);
        glAttachShader(ID, fragment);
        glLinkProgram(ID);
        checkCompileErrors(ID, "PROGRAM");

        glDeleteShader(vertex);
        glDeleteShader(fragment);
    }

    void use() const {
        glUseProgram(ID);
    }

    void setInt(const std::string &name, int value) const {
        glUniform1i(glGetUniformLocation(ID, name.c_str()), value);
    }

    void setFloat(const std::string &name, float value) const {
        glUniform1f(glGetUniformLocation(ID, name.c_str()), value);
    }

    void setVec3(const std::string &name, const glm::vec3 &value) const {
        glUniform3fv(glGetUniformLocation(ID, name.c_str()), 1, &value[0]);
    }

    void setMat4(const std::string &name, const glm::mat4 &mat) const {
        glUniformMatrix4fv(glGetUniformLocation(ID, name.c_str()), 1, GL_FALSE, &mat[0][0]);
    }

private:
    static void checkCompileErrors(unsigned int shader, const std::string &type) {
        int success;
        char log[1024];

        if (type != "PROGRAM") {
            glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
            if (!success) {
                glGetShaderInfoLog(shader, 1024, NULL, log);
                std::cout << "SHADER COMPILATION FAILED [" << type << "]\n" 
                          << log << "\n";
            }
        } else {
            glGetProgramiv(shader, GL_LINK_STATUS, &success);
            if (!success) {
                glGetProgramInfoLog(shader, 1024, NULL, log);
                std::cout << "PROGRAM LINKING FAILED\n" 
                          << log << "\n";
            }
        }
    }
};

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