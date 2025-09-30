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

    const char* glTypeToString(GLenum value)
    {
        static std::unordered_map<GLenum, const char*> types
            = { { GL_FLOAT, "float" },
                { GL_FLOAT_VEC2, "vec2" },
                { GL_FLOAT_VEC3, "vec3" },
                { GL_FLOAT_VEC4, "vec4" },
                { GL_DOUBLE, "double" },
                { GL_DOUBLE_VEC2, "dvec2" },
                { GL_DOUBLE_VEC3, "dvec3" },
                { GL_DOUBLE_VEC4, "dvec4" },
                { GL_INT, "int" },
                { GL_INT_VEC2, "ivec2" },
                { GL_INT_VEC3, "ivec3" },
                { GL_INT_VEC4, "ivec4" },
                { GL_UNSIGNED_INT, "uint" },
                { GL_UNSIGNED_INT_VEC2, "uvec2" },
                { GL_UNSIGNED_INT_VEC3, "uvec3" },
                { GL_UNSIGNED_INT_VEC4, "uvec4" },
                { GL_BOOL, "bool" },
                { GL_BOOL_VEC2, "bvec2" },
                { GL_BOOL_VEC3, "bvec3" },
                { GL_BOOL_VEC4, "bvec4" },
                { GL_FLOAT_MAT2, "mat2" },
                { GL_FLOAT_MAT3, "mat3" },
                { GL_FLOAT_MAT4, "mat4" },
                { GL_FLOAT_MAT2x3, "mat2x3" },
                { GL_FLOAT_MAT3x2, "mat3x2" },
                { GL_FLOAT_MAT2x4, "mat2x4" },
                { GL_FLOAT_MAT4x2, "mat4x2" },
                { GL_FLOAT_MAT3x4, "mat3x4" },
                { GL_FLOAT_MAT4x3, "mat4x3" },
                { GL_SAMPLER_2D, "sampler2D" },
                { GL_SAMPLER_CUBE, "samplerCube" },
                { GL_SAMPLER_3D, "sampler3D" },
                { GL_SAMPLER_2D_SHADOW, "sampler2DShadow" },
                { GL_SAMPLER_CUBE_SHADOW, "samplerCubeShadow" },
                { GL_SAMPLER_1D, "sampler1D" },
                { GL_SAMPLER_1D_SHADOW, "sampler1DShadow" },
                { GL_SAMPLER_2D_ARRAY, "sampler2DArray" },
                { GL_SAMPLER_2D_ARRAY_SHADOW, "sampler2DArrayShadow" },
                { GL_SAMPLER_BUFFER, "samplerBuffer" },
                { GL_SAMPLER_2D_MULTISAMPLE, "sampler2DMS" },
                { GL_SAMPLER_2D_MULTISAMPLE_ARRAY, "sampler2DMSArray" },
                { GL_SAMPLER_CUBE_MAP_ARRAY, "samplerCubeArray" },
                { GL_SAMPLER_CUBE_MAP_ARRAY_SHADOW, "samplerCubeArrayShadow" },
                { GL_INT_SAMPLER_2D, "isampler2D" },
                { GL_INT_SAMPLER_3D, "isampler3D" },
                { GL_INT_SAMPLER_CUBE, "isamplerCube" },
                { GL_INT_SAMPLER_BUFFER, "isamplerBuffer" },
                { GL_UNSIGNED_INT_SAMPLER_2D, "usampler2D" },
                { GL_UNSIGNED_INT_SAMPLER_3D, "usampler3D" },
                { GL_UNSIGNED_INT_SAMPLER_CUBE, "usamplerCube" },
                { GL_UNSIGNED_INT_SAMPLER_BUFFER, "usamplerBuffer" } };

        DEBUG_ASSERT(types.find(value) != types.end(), "Was passed invalid GL type.");

        return types[value];
    }
} // namespace Core