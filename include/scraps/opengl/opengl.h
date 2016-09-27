/**
* Copyright 2016 BitTorrent Inc.
*
* Licensed under the Apache License, Version 2.0 (the "License");
* you may not use this file except in compliance with the License.
* You may obtain a copy of the License at
*
*    http://www.apache.org/licenses/LICENSE-2.0
*
* Unless required by applicable law or agreed to in writing, software
* distributed under the License is distributed on an "AS IS" BASIS,
* WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
* See the License for the specific language governing permissions and
* limitations under the License.
*/
#pragma once

#include "scraps/platform.h"

#if SCRAPS_IOS || SCRAPS_TVOS
    #include <OpenGLES/ES3/gl.h>
    #include <OpenGLES/ES3/glext.h>
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

#if !NDEBUG
#define SCRAPS_GL_ERROR_CHECK() \
    { \
        auto err = glGetError(); \
        if (err != GL_NO_ERROR) { \
            SCRAPS_LOG_WARNING("opengl error 0x{:x} {}", err, scraps::opengl::GetErrorName(err)); \
        } \
    }
#else
#define SCRAPS_GL_ERROR_CHECK()
#endif

namespace scraps {
namespace opengl {

constexpr const char* GetErrorName(GLenum err) {
    switch (err) {
        case GL_INVALID_OPERATION:              return "INVALID_OPERATION";
        case GL_INVALID_ENUM:                   return "INVALID_ENUM";
        case GL_INVALID_VALUE:                  return "INVALID_VALUE";
        case GL_OUT_OF_MEMORY:                  return "OUT_OF_MEMORY";
        case GL_INVALID_FRAMEBUFFER_OPERATION:  return "INVALID_FRAMEBUFFER_OPERATION";
        default:                                return "unknown";
    };
}

#if OPENGL_ES
constexpr bool kIsOpenGLES = true;
#else
constexpr bool kIsOpenGLES = false;
#endif

GLint MajorVersion();

/**
* @return true if the given opengl extension is present for the current context
*/
bool HasExtension(std::string extension);

/**
* Define some shader macros that let you write the rest of the shader more portably.
*/
std::string CommonVertexShaderHeader(std::vector<std::string> extensions = {});
std::string CommonFragmentShaderHeader(std::vector<std::string> extensions = {});

}}

