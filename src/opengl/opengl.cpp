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
#include <scraps/opengl/opengl.h>

namespace scraps {
namespace opengl {

GLint MajorVersion() {
    GLint ret = -1;
#if GL_MAJOR_VERSION
    SCRAPS_GL_ERROR_CHECK();
    glGetIntegerv(GL_MAJOR_VERSION, &ret);
    glGetError(); // GL_MAJOR_VERSION may not be supported yet
#endif
    return ret;
}

bool HasExtension(std::string extension) {
    auto extensions = std::string{(const char*)glGetString(GL_EXTENSIONS)};
    return extensions.find(std::move(extension)) != std::string::npos;
}

std::string CommonShaderHeader(GLint majorVersion, std::vector<std::string> extensions = {}) {
    std::string ret;
    if (kIsOpenGLES) {
        if (majorVersion >= 3) {
            ret += "#version 300 es\n";
        }
    } else {
        ret += "#version 140\n";
    }
    for (auto& extension : extensions) {
        ret += "#extension "s + extension + " : enable\n";
    }
    if (kIsOpenGLES) {
        ret += "precision highp float;\n";
    }
    if (kIsOpenGLES && majorVersion < 3) {
        ret +=
            "#define VARYING_IN varying\n"
            "#define VARYING_OUT varying\n"
            "#define ATTRIBUTE_IN attribute\n"
            "#define SAMPLE texture2D\n"
            "#define SAMPLE_3D texture3D\n"
        ;
    } else {
        ret +=
            "#define VARYING_IN in\n"
            "#define VARYING_OUT out\n"
            "#define ATTRIBUTE_IN in\n"
            "#define SAMPLE texture\n"
            "#define SAMPLE_3D texture\n"
        ;
    }
    return ret;
}

std::string CommonVertexShaderHeader(std::vector<std::string> extensions) {
    return CommonShaderHeader(MajorVersion(), extensions);
}

std::string CommonFragmentShaderHeader(std::vector<std::string> extensions) {
    auto majorVersion = MajorVersion();
    std::string ret = CommonShaderHeader(majorVersion, extensions);
    if (kIsOpenGLES && MajorVersion() < 3) {
        ret += "#define COLOR_OUT gl_FragColor\n";
    } else {
        ret += "out vec4 COLOR_OUT;\n";
    }
    return ret;
}

}} // namespace scraps::opengl
