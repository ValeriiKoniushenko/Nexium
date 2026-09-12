// Nexium
// Copyright 2018-2027 Valerii Koniushenko
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0

#pragma once

#include "Core/String.h"
#include "Foundation/BaseLog.h"
#include "JustReflectMe/Adapter.h"
#include "ShaderProgram.h"

#include <unordered_set>

namespace NX
{
    ENUM_CLASS();
    enum class ShaderType : GLenum
    {
        None = 0,
        Fragment = GL_FRAGMENT_SHADER,
        Vertex = GL_VERTEX_SHADER,
        Geometry = GL_GEOMETRY_SHADER
    };

    class ShaderProgramMeta final : public Foundation::BaseLog
    {
    public:
        struct Hasher
        {
            [[nodiscard]] std::size_t operator()(const ShaderProgramMeta& self) const;
        };

    public:
        ShaderProgramMeta() = default;

        [[nodiscard]] bool operator==(const ShaderProgramMeta& other);

        void create(const std::filesystem::path& vertexShaderPath,
                    const std::filesystem::path& fragmentShaderPath);

        [[nodiscard]] ShaderProgram& getShaderProgram() noexcept { return _shaderProgram; }

        [[nodiscard]] const ShaderProgram& getShaderProgram() const noexcept
        {
            return _shaderProgram;
        }

        [[nodiscard]] const std::unordered_set<ShaderVariable, ShaderVariable::Hasher>&
            getUniforms() const
        {
            return _uniforms;
        }

        [[nodiscard]] const std::unordered_set<ShaderUBO, ShaderUBO::Hasher>& getUBOs() const
        {
            return _uniformBufferObjects;
        }

        [[nodiscard]] const std::unordered_set<ShaderVariable, ShaderVariable::Hasher>& getInputs()
            const
        {
            return _inputs;
        }

        [[nodiscard]] const std::unordered_set<ShaderVariable, ShaderVariable::Hasher>& getOutputs()
            const
        {
            return _outputs;
        }

        void setShaderName(const std::string& name);

        void setShaderName(const Core::StringAtom& name);

        [[nodiscard]] const Core::StringAtom& getShaderName() const { return _shaderName; }

        [[nodiscard]] spdlog::logger* getLogger() const override;

        [[nodiscard]] const char* getPrefix() const override { return "ShaderProgramMeta"; }

        void recreateFromSources();

        bool safeRecreateFromSources();

        [[nodiscard]] const std::filesystem::path& getVertexShaderPath() const noexcept
        {
            return _vertexShaderPath;
        }

        [[nodiscard]] const std::filesystem::path& getFragmentShaderPath() const noexcept
        {
            return _fragmentShaderPath;
        }

    private:
        void compileShader();

        void requireNoCompileErrors();

        void generateShaderId();

        void readSourceShaderFile(const std::filesystem::path& vertexShaderPath,
                                  const std::filesystem::path& fragmentShaderPath);

        static void checkShaderCompileStatus(GLuint shaderId, const std::string& shaderType);

        void reflectShaderVariablesFor(GLuint shaderProgramId);
        void reflectShaderUniformBlocksFor(GLuint shaderProgramId);

    private:
        ShaderProgram _shaderProgram;
        Core::StringAtom _shaderName;

        std::unordered_set<ShaderVariable, ShaderVariable::Hasher> _uniforms;
        std::unordered_set<ShaderVariable, ShaderVariable::Hasher> _inputs;
        std::unordered_set<ShaderVariable, ShaderVariable::Hasher> _outputs;
        std::unordered_set<ShaderUBO, ShaderUBO::Hasher> _uniformBufferObjects;

        std::filesystem::path _vertexShaderPath;
        std::filesystem::path _fragmentShaderPath;
    };
} // namespace NX

#include "ShaderProgramMeta.generated.h" // added by the code generator. Better don't move it.
