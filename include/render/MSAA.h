#pragma once
#include <glad/glad.h>
#include <iostream>

class MSAA_FBO {
public:
    int width, height;
    int samples;

    GLuint fbo_msaa = 0;
    GLuint color_msaa = 0;
    GLuint depth_msaa = 0;

    GLuint fbo_resolve = 0;
    GLuint tex_resolved = 0;

    MSAA_FBO(int w, int h, int s) {
        width = w;
        height = h;
        samples = s;
        create();
    }

    ~MSAA_FBO() {
        destroy();
    }

    void recreate(int newSamples) {
        samples = newSamples;
        destroy();
        create();
    }

    void create() {
        // multisample fbo
        glGenFramebuffers(1, &fbo_msaa);
        glBindFramebuffer(GL_FRAMEBUFFER, fbo_msaa);

        // color
        glGenRenderbuffers(1, &color_msaa);
        glBindRenderbuffer(GL_RENDERBUFFER, color_msaa);
        glRenderbufferStorageMultisample(GL_RENDERBUFFER, samples, GL_RGBA8, width, height);
        glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_RENDERBUFFER, color_msaa);

        // depth
        glGenRenderbuffers(1, &depth_msaa);
        glBindRenderbuffer(GL_RENDERBUFFER, depth_msaa);
        glRenderbufferStorageMultisample(GL_RENDERBUFFER, samples, GL_DEPTH24_STENCIL8, width, height);
        glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, depth_msaa);

        if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
            std::cerr << "MSAA FBO incomplete!" << std::endl;
        }

        // resolve fbo
        glGenFramebuffers(1, &fbo_resolve);
        glBindFramebuffer(GL_FRAMEBUFFER, fbo_resolve);

        glGenTextures(1, &tex_resolved);
        glBindTexture(GL_TEXTURE_2D, tex_resolved);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0,
                     GL_RGBA, GL_UNSIGNED_BYTE, nullptr);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
                               GL_TEXTURE_2D, tex_resolved, 0);

        if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
            std::cerr << "Resolve FBO incomplete!" << std::endl;
        }

        // restore default
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }

    void destroy() {
        if (color_msaa) glDeleteRenderbuffers(1, &color_msaa);
        if (depth_msaa) glDeleteRenderbuffers(1, &depth_msaa);
        if (fbo_msaa) glDeleteFramebuffers(1, &fbo_msaa);

        if (tex_resolved) glDeleteTextures(1, &tex_resolved);
        if (fbo_resolve) glDeleteFramebuffers(1, &fbo_resolve);

        color_msaa = depth_msaa = fbo_msaa = 0;
        tex_resolved = fbo_resolve = 0;
    }

    void resolve() {
        glBindFramebuffer(GL_READ_FRAMEBUFFER, fbo_msaa);
        glBindFramebuffer(GL_DRAW_FRAMEBUFFER, fbo_resolve);

        glBlitFramebuffer(
            0, 0, width, height,
            0, 0, width, height,
            GL_COLOR_BUFFER_BIT,
            GL_LINEAR
        );

        glBindFramebuffer(GL_FRAMEBUFFER, 0);
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