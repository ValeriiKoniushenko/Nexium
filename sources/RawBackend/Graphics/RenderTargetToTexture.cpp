// Nexium
// Copyright 2018-2026 Valerii Koniushenko
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0

#include "RenderTargetToTexture.h"

#include "Foundation/BaseLog.h"

using namespace Core;

namespace NX
{
    void RenderTargetToTexture::generate()
    {
        if (_isGenerated)
        {
            return;
        }

        glGenFramebuffers(1, &_fbo);
        glBindFramebuffer(GL_FRAMEBUFFER, _fbo);

        glGenTextures(1, &_tex);
        glBindTexture(GL_TEXTURE_2D, _tex);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, _size.width, _size.height, 0, GL_RGBA,
                     GL_UNSIGNED_BYTE, nullptr);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, _tex, 0);

        glGenRenderbuffers(1, &_rbo);
        glBindRenderbuffer(GL_RENDERBUFFER, _rbo);
        glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, _size.width, _size.height);
        glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER,
                                  _rbo);

        // Check completeness
        if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        {
            gGlobalLog.criticalLog("Framebuffer for editor render is not complete!");
        }
        glBindFramebuffer(GL_FRAMEBUFFER, 0);

        _isGenerated = true;
    }

    void RenderTargetToTexture::callMePreDraw() const
    {
        glBindFramebuffer(GL_FRAMEBUFFER, _fbo);
    }

    void RenderTargetToTexture::callMeAfterDraw() const
    {
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }

    void RenderTargetToTexture::setRenderSize(ISize2 size)
    {
        destroy();
        _size = size;
        generate();
    }

    void RenderTargetToTexture::clearCanvas() const
    {
        GLint previousFbo = 0;
        glGetIntegerv(GL_FRAMEBUFFER_BINDING, &previousFbo);

        glBindFramebuffer(GL_FRAMEBUFFER, _fbo);
        glViewport(0, 0, _size.width, _size.height);

        glClearColor(0.45f, 0.55f, 0.60f, 1.00f);
        constexpr int clearBits = GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT;

        glClear(clearBits);

        glBindFramebuffer(GL_FRAMEBUFFER, static_cast<GLuint>(previousFbo));
    }

    void RenderTargetToTexture::destroy()
    {
        glDeleteFramebuffers(1, &_fbo);
        glDeleteTextures(1, &_tex);
        glDeleteRenderbuffers(1, &_rbo);

        _fbo = _tex = _rbo = 0;
        _isGenerated = false;
    }
} // namespace NX