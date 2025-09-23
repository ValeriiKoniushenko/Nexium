/*
 * MIT License
 *
 * Copyright (c) 2018-2025 Valerii Koniushenko
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#include "OpenGL.h"

namespace Core
{

    void UpdateGlViewport(FSize2 originalSize, float aspectRatio, ViewportMode mode)
    {
        FSize2 view = originalSize;
        glm::vec2 pos = {};

        if (mode.cast() == ViewportMode::Default)
        {
            view.height = view.width / aspectRatio;

            if (view.height > originalSize.height)
            {
                view.height = originalSize.height;
                view.width = originalSize.height * aspectRatio;
            }

            pos.x = (originalSize.width - view.width) / 2.f;
            pos.y = (originalSize.height - view.height) / 2.f;
        }
        else
        {
            view.width = view.height * aspectRatio;

            if (view.width < originalSize.width)
            {
                view.width = originalSize.width;
                view.height = view.width / aspectRatio;
            }
            pos.x = (originalSize.width - view.width) / 2.f;
            pos.y = 0;
        }

        glViewport(static_cast<GLint>(pos.x), static_cast<GLint>(pos.y),
                   static_cast<GLsizei>(view.width), static_cast<GLsizei>(view.height));
    }

    StringAtom glTypeToString(GLenum value)
    {
        switch (value)
        {
            case GL_FLOAT:
                return "float"_atom;
            case GL_FLOAT_VEC2:
                return "vec2"_atom;
            case GL_FLOAT_VEC3:
                return "vec3"_atom;
            case GL_FLOAT_VEC4:
                return "vec4"_atom;
            case GL_DOUBLE:
                return "double"_atom;
            case GL_DOUBLE_VEC2:
                return "dvec2"_atom;
            case GL_DOUBLE_VEC3:
                return "dvec3"_atom;
            case GL_DOUBLE_VEC4:
                return "dvec4"_atom;
            case GL_INT:
                return "int"_atom;
            case GL_INT_VEC2:
                return "ivec2"_atom;
            case GL_INT_VEC3:
                return "ivec3"_atom;
            case GL_INT_VEC4:
                return "ivec4"_atom;
            case GL_UNSIGNED_INT:
                return "uint"_atom;
            case GL_UNSIGNED_INT_VEC2:
                return "uvec2"_atom;
            case GL_UNSIGNED_INT_VEC3:
                return "uvec3"_atom;
            case GL_UNSIGNED_INT_VEC4:
                return "uvec4"_atom;
            case GL_BOOL:
                return "bool"_atom;
            case GL_BOOL_VEC2:
                return "bvec2"_atom;
            case GL_BOOL_VEC3:
                return "bvec3"_atom;
            case GL_BOOL_VEC4:
                return "bvec4"_atom;
            case GL_FLOAT_MAT2:
                return "mat2"_atom;
            case GL_FLOAT_MAT3:
                return "mat3"_atom;
            case GL_FLOAT_MAT4:
                return "mat4"_atom;
            case GL_FLOAT_MAT2x3:
                return "mat2x3"_atom;
            case GL_FLOAT_MAT3x2:
                return "mat3x2"_atom;
            case GL_FLOAT_MAT2x4:
                return "mat2x4"_atom;
            case GL_FLOAT_MAT4x2:
                return "mat4x2"_atom;
            case GL_FLOAT_MAT3x4:
                return "mat3x4"_atom;
            case GL_FLOAT_MAT4x3:
                return "mat4x3"_atom;
            case GL_SAMPLER_2D:
                return "sampler2D"_atom;
            case GL_SAMPLER_CUBE:
                return "samplerCube"_atom;
            case GL_SAMPLER_3D:
                return "sampler3D"_atom;
            case GL_SAMPLER_2D_SHADOW:
                return "sampler2DShadow"_atom;
            case GL_SAMPLER_CUBE_SHADOW:
                return "samplerCubeShadow"_atom;
            case GL_SAMPLER_1D:
                return "sampler1D"_atom;
            case GL_SAMPLER_1D_SHADOW:
                return "sampler1DShadow"_atom;
            case GL_SAMPLER_2D_ARRAY:
                return "sampler2DArray"_atom;
            case GL_SAMPLER_2D_ARRAY_SHADOW:
                return "sampler2DArrayShadow"_atom;
            case GL_SAMPLER_BUFFER:
                return "samplerBuffer"_atom;
            case GL_SAMPLER_2D_MULTISAMPLE:
                return "sampler2DMS"_atom;
            case GL_SAMPLER_2D_MULTISAMPLE_ARRAY:
                return "sampler2DMSArray"_atom;
            case GL_SAMPLER_CUBE_MAP_ARRAY:
                return "samplerCubeArray"_atom;
            case GL_SAMPLER_CUBE_MAP_ARRAY_SHADOW:
                return "samplerCubeArrayShadow"_atom;
            case GL_INT_SAMPLER_2D:
                return "isampler2D"_atom;
            case GL_INT_SAMPLER_3D:
                return "isampler3D"_atom;
            case GL_INT_SAMPLER_CUBE:
                return "isamplerCube"_atom;
            case GL_INT_SAMPLER_BUFFER:
                return "isamplerBuffer"_atom;
            case GL_UNSIGNED_INT_SAMPLER_2D:
                return "usampler2D"_atom;
            case GL_UNSIGNED_INT_SAMPLER_3D:
                return "usampler3D"_atom;
            case GL_UNSIGNED_INT_SAMPLER_CUBE:
                return "usamplerCube"_atom;
            case GL_UNSIGNED_INT_SAMPLER_BUFFER:
                return "usamplerBuffer"_atom;
            default:
                return "unknown"_atom;
        }
    }

} // namespace Core