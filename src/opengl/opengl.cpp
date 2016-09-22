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
#include "scraps/opengl/opengl.h"

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

std::string CommonShaderHeader(GLint majorVersion) {
    std::string ret;
    if (kIsOpenGLES && majorVersion >= 3) {
        ret += "#version 300 es\n";
    } else if (!kIsOpenGLES) {
        ret += "#version 140\n";
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

std::string CommonVertexShaderHeader() {
    return CommonShaderHeader(MajorVersion());
}

std::string CommonFragmentShaderHeader() {
    auto majorVersion = MajorVersion();
    std::string ret = CommonShaderHeader(majorVersion);
    if (kIsOpenGLES && MajorVersion() < 3) {
        ret += "#define COLOR_OUT gl_FragColor\n";
    } else {
        ret += "out vec4 COLOR_OUT;\n";
    }
    return ret;
}

}} // namespace scraps::opengl
