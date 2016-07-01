#pragma once

#include "scraps/platform.h"

#if SCRAPS_IOS || SCRAPS_TVOS
    #include <OpenGLES/ES2/gl.h>
    #include <OpenGLES/ES2/glext.h>
    #define OPENGL_ES 1
#elif defined(__APPLE__)
    #include <OpenGL/OpenGL.h>
    #include <OpenGL/gl3.h>
#elif defined(__ANDROID__)
    #include <GLES2/gl2.h>
    #include <GLES2/gl2ext.h>
    #define OPENGL_ES 1
#elif defined(__linux__)
    #if !defined(GL_GLEXT_PROTOTYPES)
        #define GL_GLEXT_PROTOTYPES
    #endif
    #include <GL/gl.h>
#endif

#if OPENGL_ES
#define SCRAPS_COMMON_SHADER_HEADER \
    "precision highp float;\n" \
    "#define VARYING_IN varying\n" \
    "#define VARYING_OUT varying\n" \
    "#define ATTRIBUTE_IN attribute\n" \
    "#define SAMPLE texture2D\n" \

#define SCRAPS_VERTEX_SHADER_HEADER SCRAPS_COMMON_SHADER_HEADER
#define SCRAPS_FRAGMENT_SHADER_HEADER SCRAPS_COMMON_SHADER_HEADER \
    "#define COLOR_OUT gl_FragColor\n"

#else
#define SCRAPS_COMMON_SHADER_HEADER \
    "#version 140\n" \
    "#define VARYING_IN in\n" \
    "#define VARYING_OUT out\n" \
    "#define ATTRIBUTE_IN in\n" \
    "#define SAMPLE texture\n" \

#define SCRAPS_VERTEX_SHADER_HEADER SCRAPS_COMMON_SHADER_HEADER
#define SCRAPS_FRAGMENT_SHADER_HEADER SCRAPS_COMMON_SHADER_HEADER \
    "out vec4 COLOR_OUT;\n"

#endif

namespace scraps {

constexpr const char* getGLErrorType(GLenum err) {
    switch (err) {
        case GL_INVALID_OPERATION:              return "INVALID_OPERATION";
        case GL_INVALID_ENUM:                   return "INVALID_ENUM";
        case GL_INVALID_VALUE:                  return "INVALID_VALUE";
        case GL_OUT_OF_MEMORY:                  return "OUT_OF_MEMORY";
        case GL_INVALID_FRAMEBUFFER_OPERATION:  return "INVALID_FRAMEBUFFER_OPERATION";
        default:                                return "unknown";
    };
}

}

#if !NDEBUG
#define SCRAPS_GL_ERROR_CHECK() \
    { \
        auto err = glGetError(); \
        if (err != GL_NO_ERROR) { \
            SCRAPS_LOG_WARNING("opengl error 0x{:x} {}", err, scraps::getGLErrorType(err)); \
        } \
    }
#else
#define SCRAPS_GL_ERROR_CHECK()
#endif
