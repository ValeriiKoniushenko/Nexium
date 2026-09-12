// Nexium
// Copyright 2018-2027 Valerii Koniushenko
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0

#include "ShaderProgram.h"

#include "../PrivateModuleInfo.h"
#include "ShaderProgramMeta.h"

#include <array>

namespace NX
{
    ShaderProgram::ShaderProgram(ShaderProgram&& other) noexcept
        : _uniforms(std::move(other._uniforms)),
          _ubos(std::move(other._ubos)),
          _setupVertexAttribute(std::move(other._setupVertexAttribute)),
          _setEvent(std::move(other._setEvent)),
          _name(std::move(other._name)),
          _vertexShaderId(other._vertexShaderId),
          _fragmentShaderId(other._fragmentShaderId),
          _shaderProgramId(other._shaderProgramId)
    {
        other._vertexShaderId = 0;
        other._fragmentShaderId = 0;
        other._shaderProgramId = 0;
    }

    ShaderProgram& ShaderProgram::operator=(ShaderProgram&& other) noexcept
    {
        if (this == &other) [[likely]]
        {
            return *this;
        }

        ShaderProgram temp(std::move(other));
        swap(*this, temp);
        return *this;
    }

    void ShaderProgram::setName(const Core::StringAtom& name)
    {
        _name = name;

        if (!_name.isStatic())
        {
            _name.shrinkToFit();
#ifdef NEXIUM_GRAPHICS_DEBUG
            warnLog(
                "The shader's '{}' vairable '_name' is not atom. Make it atom for best performance."_f
                << _name);
#endif
        }
    }

    void ShaderProgram::create(const Core::StringAtom& shaderName)
    {
        clearOnlyShaderProgram();

        debugLog("Creating of the shader program '{}' is started."_f << shaderName);

        setName(shaderName);

        if (_vertexShaderId == 0)
        {
            criticalThrowingLog("Vertex shader is empty. Impossible to create the shader program."_f
                                << shaderName);
        }
        if (_fragmentShaderId == 0)
        {
            criticalThrowingLog(
                "Fragment shader is empty. Impossible to create the shader program."_f
                << shaderName);
        }

        _shaderProgramId = glCreateProgram();
        if (_shaderProgramId == 0)
        {
            criticalThrowingLog("glCreateProgram to create a shader program was failed for '{}'"_f
                                << shaderName);
        }

        glAttachShader(_shaderProgramId, _vertexShaderId);
        glAttachShader(_shaderProgramId, _fragmentShaderId);
        glLinkProgram(_shaderProgramId);

        glDeleteShader(_vertexShaderId);
        glDeleteShader(_fragmentShaderId);

        int success = 0;
        glGetProgramiv(_shaderProgramId, GL_LINK_STATUS, &success);
        if (!success)
        {
            clear();

            constexpr auto size = 512;
            std::array<char, size> infoLog{};
            glGetProgramInfoLog(_shaderProgramId, static_cast<GLsizei>(infoLog.size()), nullptr,
                                infoLog.data());
            criticalThrowingLog("Shader program compilation error: {}"_f << infoLog.data());
        }

        infoLog("The shader program '{}' linked successfully."_f << shaderName);
    }

    void ShaderProgram::clear()
    {
        clearOnlyShaderProgram();
        _name.clear();
        _vertexShaderId = 0;
        _fragmentShaderId = 0;
    }

    spdlog::logger* ShaderProgram::getLogger() const
    {
        return NxSubsystems::getLogger();
    }

    void ShaderProgram::setDataFromMeta(const ShaderProgramMeta& meta)
    {
        for (const auto& u : meta.getUniforms())
        {
            _uniforms[u.name] = u.location;
        }

        for (const auto& u : meta.getUBOs())
        {
            _ubos[u.name] = u.binding;
        }
    }

    void ShaderProgram::setupVertexAttribute()
    {
        if (!_setupVertexAttribute)
        {
            return;
        }

        _setupVertexAttribute();
    }

    void ShaderProgram::setVertexAttributeCallback(std::function<void()>&& func)
    {
        _setupVertexAttribute = std::move(func);
    }

    void ShaderProgram::callSetEvent(Event event)
    {
        if (_setEvent) [[likely]]
        {
            _setEvent(event);
        }
    }

    void ShaderProgram::setSetEventCallback(std::function<void(Event)>&& func)
    {
        _setEvent = std::move(func);
    }

    void ShaderProgram::clearOnlyShaderProgram()
    {
        if (_shaderProgramId != 0)
        {
            glDeleteProgram(_shaderProgramId);
            _shaderProgramId = 0;
        }
    }

    void ShaderProgram::debugUniform(const Core::StringAtom& name) const
    {
#ifdef NEXIUM_GRAPHICS_DEBUG
        Assert(name.isStatic(), "Use atomic string. Add _atom to your uniform's str");
        Assert(_uniforms.contains(name));
        Assert(_uniforms.at(name) != -1);
#endif
    }
} // namespace NX
