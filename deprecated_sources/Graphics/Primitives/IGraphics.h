/*
 * MIT License
 *
 * Copyright (c) 2018-2026 Valerii Koniushenko
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

#pragma once

#include <cstddef>
#include <cstdint>

namespace Graphics
{
    using DebugCallback = void (*)(uint32_t source, uint32_t type, uint32_t id, uint32_t severity,
                                   int32_t length, const char* message, const void* userParam);

    // ============================================================================
    class IGraphics
    {
    public:
        virtual ~IGraphics() = default;

        IGraphics() = default;

        IGraphics(const IGraphics&) = delete;

        IGraphics& operator=(const IGraphics&) = delete;

        IGraphics(IGraphics&&) = default;

        IGraphics& operator=(IGraphics&&) = default;

        // ── Textures ─────────────────────────────────────────────────────────────
        virtual void activeTexture(uint32_t texture) = 0;

        virtual void bindTexture(uint32_t target, uint32_t texture) = 0;

        virtual void genTextures(int32_t n, uint32_t* textures) = 0;

        virtual void deleteTextures(int32_t n, const uint32_t* textures) = 0;

        virtual void generateMipmap(uint32_t target) = 0;

        virtual void pixelStorei(uint32_t pname, int32_t param) = 0;

        virtual void texImage2D(uint32_t target, int32_t level, int32_t internalformat,
                                int32_t width, int32_t height, int32_t border, uint32_t format,
                                uint32_t type, const void* pixels) = 0;

        virtual void texSubImage2D(uint32_t target, int32_t level, int32_t xoffset, int32_t yoffset,
                                   int32_t width, int32_t height, uint32_t format, uint32_t type,
                                   const void* pixels) = 0;

        virtual void texParameteri(uint32_t target, uint32_t pname, int32_t param) = 0;

        // ── Buffers ──────────────────────────────────────────────────────────────
        virtual void genBuffers(int32_t n, uint32_t* buffers) = 0;

        virtual void deleteBuffers(int32_t n, const uint32_t* buffers) = 0;

        virtual void bindBuffer(uint32_t target, uint32_t buffer) = 0;

        virtual void bindBufferBase(uint32_t target, uint32_t index, uint32_t buffer) = 0;

        virtual void bufferData(uint32_t target, int64_t size, const void* data, uint32_t usage)
            = 0;

        virtual void bufferSubData(uint32_t target, int64_t offset, int64_t size, const void* data)
            = 0;

        // ── Vertex Arrays ────────────────────────────────────────────────────────
        virtual void genVertexArrays(int32_t n, uint32_t* arrays) = 0;

        virtual void deleteVertexArrays(int32_t n, const uint32_t* arrays) = 0;

        virtual void bindVertexArray(uint32_t array) = 0;

        virtual void enableVertexAttribArray(uint32_t index) = 0;

        virtual void vertexAttribPointer(uint32_t index, int32_t size, uint32_t type,
                                         bool normalized, int32_t stride, const void* pointer) = 0;

        // ── Framebuffers ─────────────────────────────────────────────────────────
        virtual void genFramebuffers(int32_t n, uint32_t* framebuffers) = 0;

        virtual void deleteFramebuffers(int32_t n, const uint32_t* framebuffers) = 0;

        virtual void bindFramebuffer(uint32_t target, uint32_t framebuffer) = 0;

        virtual void framebufferTexture2D(uint32_t target, uint32_t attachment, uint32_t textarget,
                                          uint32_t texture, int32_t level) = 0;

        virtual void framebufferRenderbuffer(uint32_t target, uint32_t attachment,
                                             uint32_t renderbuffertarget, uint32_t renderbuffer)
            = 0;

        virtual uint32_t checkFramebufferStatus(uint32_t target) = 0;

        // ── Renderbuffers ────────────────────────────────────────────────────────
        virtual void genRenderbuffers(int32_t n, uint32_t* renderbuffers) = 0;

        virtual void deleteRenderbuffers(int32_t n, const uint32_t* renderbuffers) = 0;

        virtual void bindRenderbuffer(uint32_t target, uint32_t renderbuffer) = 0;

        virtual void renderbufferStorage(uint32_t target, uint32_t internalformat, int32_t width,
                                         int32_t height) = 0;

        // ── Shaders ──────────────────────────────────────────────────────────────
        virtual uint32_t createShader(uint32_t type) = 0;

        virtual void deleteShader(uint32_t shader) = 0;

        virtual void shaderSource(uint32_t shader, int32_t count, const char* const* string,
                                  const int32_t* length) = 0;

        virtual void compileShader(uint32_t shader) = 0;

        virtual void getShaderiv(uint32_t shader, uint32_t pname, int32_t* params) = 0;

        virtual void getShaderInfoLog(uint32_t shader, int32_t bufSize, int32_t* length,
                                      char* infoLog) = 0;

        // ── Programs ─────────────────────────────────────────────────────────────
        virtual uint32_t createProgram() = 0;

        virtual void deleteProgram(uint32_t program) = 0;

        virtual void attachShader(uint32_t program, uint32_t shader) = 0;

        virtual void linkProgram(uint32_t program) = 0;

        virtual void useProgram(uint32_t program) = 0;

        virtual void getProgramiv(uint32_t program, uint32_t pname, int32_t* params) = 0;

        virtual void getProgramInfoLog(uint32_t program, int32_t bufSize, int32_t* length,
                                       char* infoLog) = 0;

        virtual void getProgramInterfaceiv(uint32_t program, uint32_t programInterface,
                                           uint32_t pname, int32_t* params) = 0;

        virtual void getProgramResourceName(uint32_t program, uint32_t programInterface,
                                            uint32_t index, int32_t bufSize, int32_t* length,
                                            char* name) = 0;

        virtual void getProgramResourceiv(uint32_t program, uint32_t programInterface,
                                          uint32_t index, int32_t propCount, const uint32_t* props,
                                          int32_t count, int32_t* length, int32_t* params) = 0;

        // ── Uniforms ─────────────────────────────────────────────────────────────
        virtual void uniform1f(int32_t location, float v0) = 0;

        virtual void uniform1i(int32_t location, int32_t v0) = 0;

        virtual void uniform1ui(int32_t location, uint32_t v0) = 0;

        virtual void uniform2f(int32_t location, float v0, float v1) = 0;

        virtual void uniform2i(int32_t location, int32_t v0, int32_t v1) = 0;

        virtual void uniform2ui(int32_t location, uint32_t v0, uint32_t v1) = 0;

        virtual void uniform3f(int32_t location, float v0, float v1, float v2) = 0;

        virtual void uniform3i(int32_t location, int32_t v0, int32_t v1, int32_t v2) = 0;

        virtual void uniform3ui(int32_t location, uint32_t v0, uint32_t v1, uint32_t v2) = 0;

        virtual void uniform4f(int32_t location, float v0, float v1, float v2, float v3) = 0;

        virtual void uniform4i(int32_t location, int32_t v0, int32_t v1, int32_t v2, int32_t v3)
            = 0;

        virtual void uniform4ui(int32_t location, uint32_t v0, uint32_t v1, uint32_t v2,
                                uint32_t v3) = 0;

        virtual void uniformBlockBinding(uint32_t program, uint32_t uniformBlockIndex,
                                         uint32_t uniformBlockBinding) = 0;

        virtual void uniformMatrix2fv(int32_t location, int32_t count, bool transpose,
                                      const float* value) = 0;

        virtual void uniformMatrix2x3fv(int32_t location, int32_t count, bool transpose,
                                        const float* value) = 0;

        virtual void uniformMatrix2x4fv(int32_t location, int32_t count, bool transpose,
                                        const float* value) = 0;

        virtual void uniformMatrix3fv(int32_t location, int32_t count, bool transpose,
                                      const float* value) = 0;

        virtual void uniformMatrix3x2fv(int32_t location, int32_t count, bool transpose,
                                        const float* value) = 0;

        virtual void uniformMatrix3x4fv(int32_t location, int32_t count, bool transpose,
                                        const float* value) = 0;

        virtual void uniformMatrix4fv(int32_t location, int32_t count, bool transpose,
                                      const float* value) = 0;

        virtual void uniformMatrix4x2fv(int32_t location, int32_t count, bool transpose,
                                        const float* value) = 0;

        virtual void uniformMatrix4x3fv(int32_t location, int32_t count, bool transpose,
                                        const float* value) = 0;

        // ── Draw Calls ───────────────────────────────────────────────────────────
        virtual void drawArrays(uint32_t mode, int32_t first, int32_t count) = 0;

        virtual void drawElements(uint32_t mode, int32_t count, uint32_t type, const void* indices)
            = 0;

        virtual void readPixels(int32_t x, int32_t y, int32_t width, int32_t height,
                                uint32_t format, uint32_t type, void* data) = 0;

        // ── Render State ─────────────────────────────────────────────────────────
        virtual void enable(uint32_t cap) = 0;

        virtual void disable(uint32_t cap) = 0;

        virtual void blendFunc(uint32_t sfactor, uint32_t dfactor) = 0;

        virtual void depthFunc(uint32_t func) = 0;

        virtual void stencilFunc(uint32_t func, int32_t ref, uint32_t mask) = 0;

        virtual void stencilMask(uint32_t mask) = 0;

        virtual void stencilOp(uint32_t sfail, uint32_t dpfail, uint32_t dppass) = 0;

        virtual void viewport(int32_t x, int32_t y, int32_t width, int32_t height) = 0;

        // ── Clear ────────────────────────────────────────────────────────────────
        virtual void clear(uint32_t mask) = 0;

        virtual void clearColor(float red, float green, float blue, float alpha) = 0;

        // ── Sync ─────────────────────────────────────────────────────────────────
        virtual void finish() = 0;

        virtual void flush() = 0;

        // ── Debug / Query ────────────────────────────────────────────────────────
        virtual void debugMessageCallback(DebugCallback callback, const void* userParam) = 0;

        virtual const uint8_t* getString(uint32_t name) = 0;
    };
} // namespace Graphics