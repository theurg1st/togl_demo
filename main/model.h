#pragma once
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <tiny_gltf.h>
#include <vector>
#include <string>
#include <iostream>

class Model {
public:
    unsigned int VAO, VBO, EBO;
    size_t indexCount = 0;
    float rotationY = 0.0f;

    Model(const std::string& path) {
        loadModel(path);
    }

    void update(float dt) {
        rotationY += dt * 0.5f;  // auto-turn
    }

    void draw() {
        glBindVertexArray(VAO);
        glDrawElements(GL_TRIANGLES, indexCount, GL_UNSIGNED_INT, 0);
        glBindVertexArray(0);
    }

glm::mat4 getModelMatrix(float rotationX) {
    glm::mat4 m(1.0f);

    m = glm::rotate(m, glm::radians(-90.0f), glm::vec3(1,0,0));
    m = glm::rotate(m, glm::radians(90.0f),  glm::vec3(0,1,0));

    m = glm::rotate(m, rotationX, glm::vec3(1,0,0));

    // scale
    m = glm::scale(m, glm::vec3(0.1f));

    return m;
}


private:
    void loadModel(const std::string& path) {
        tinygltf::Model gltfModel;
        tinygltf::TinyGLTF loader;
        std::string err, warn;

        bool ok = loader.LoadBinaryFromFile(&gltfModel, &err, &warn, path);
        if(!warn.empty()) std::cout << "GLTF Warning: " << warn << "\n";
        if(!err.empty())  std::cout << "GLTF Error:   " << err << "\n";
        if(!ok) {
            std::cout << "Failed to load GLB: " << path << "\n";
            return;
        }

        const tinygltf::Mesh& mesh = gltfModel.meshes[0];
        const tinygltf::Primitive& primitive = mesh.primitives[0];

        const tinygltf::Accessor& posAccessor   = gltfModel.accessors[primitive.attributes.at("POSITION")];
        const tinygltf::BufferView& posView     = gltfModel.bufferViews[posAccessor.bufferView];
        const tinygltf::Buffer& posBuffer       = gltfModel.buffers[posView.buffer];

        const tinygltf::Accessor* normalAccessor = nullptr;
        if (primitive.attributes.find("NORMAL") != primitive.attributes.end()) {
            normalAccessor = &gltfModel.accessors[primitive.attributes.at("NORMAL")];
        }

        const tinygltf::Accessor* uvAccessor = nullptr;
        if (primitive.attributes.find("TEXCOORD_0") != primitive.attributes.end()) {
            uvAccessor = &gltfModel.accessors[primitive.attributes.at("TEXCOORD_0")];
        }

        const tinygltf::Accessor& indexAccessor = gltfModel.accessors[primitive.indices];
        const tinygltf::BufferView& indexView   = gltfModel.bufferViews[indexAccessor.bufferView];
        const tinygltf::Buffer& indexBuffer     = gltfModel.buffers[indexView.buffer];

        indexCount = indexAccessor.count;

        struct Vertex { glm::vec3 pos; glm::vec3 normal; glm::vec2 uv; };
        std::vector<Vertex> vertices(posAccessor.count);

        for (size_t i = 0; i < posAccessor.count; i++) {
            float* pos = (float*)(&posBuffer.data[posView.byteOffset + posAccessor.byteOffset + i * sizeof(glm::vec3)]);
            vertices[i].pos = glm::vec3(pos[0], pos[1], pos[2]);

            if (normalAccessor) {
                const tinygltf::BufferView& nView = gltfModel.bufferViews[normalAccessor->bufferView];
                const tinygltf::Buffer& nBuffer   = gltfModel.buffers[nView.buffer];
                float* normal = (float*)(&nBuffer.data[nView.byteOffset + normalAccessor->byteOffset + i * sizeof(glm::vec3)]);
                vertices[i].normal = glm::vec3(normal[0], normal[1], normal[2]);
            } else {
                vertices[i].normal = glm::vec3(0,1,0);
            }

            if (uvAccessor) {
                const tinygltf::BufferView& uvView = gltfModel.bufferViews[uvAccessor->bufferView];
                const tinygltf::Buffer& uvBuffer   = gltfModel.buffers[uvView.buffer];
                float* uv = (float*)(&uvBuffer.data[uvView.byteOffset + uvAccessor->byteOffset + i * sizeof(glm::vec2)]);
                vertices[i].uv = glm::vec2(uv[0], uv[1]);
            } else {
                vertices[i].uv = glm::vec2(0,0);
            }
        }


        std::vector<unsigned int> indices(indexCount);
        memcpy(indices.data(), &indexBuffer.data[indexView.byteOffset + indexAccessor.byteOffset], indexCount * sizeof(unsigned int));


        glGenVertexArrays(1, &VAO);
        glGenBuffers(1, &VBO);
        glGenBuffers(1, &EBO);

        glBindVertexArray(VAO);

        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), vertices.data(), GL_STATIC_DRAW);

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), indices.data(), GL_STATIC_DRAW);


        glEnableVertexAttribArray(0); // pos
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);

        glEnableVertexAttribArray(1); // normal
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, normal));

        glEnableVertexAttribArray(2); // uv
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, uv));

        glBindVertexArray(0);

        std::cout << "GLB Loaded: " << path << "\n";
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