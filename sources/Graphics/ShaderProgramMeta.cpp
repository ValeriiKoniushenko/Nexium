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

#include "ShaderProgramMeta.h"

#include "ModuleInfo.h"
#include "Utils/Functions.h"

#include <spdlog/async_logger.h>

namespace Core
{
    std::size_t ShaderProgramMeta::Hasher::operator()(const ShaderProgramMeta& self) const
    {
        return self._shaderName.makeHash();
    }

    bool ShaderProgramMeta::operator==(const ShaderProgramMeta& other)
    {
        return _shaderName == other._shaderName;
    }

    void ShaderProgramMeta::create(const std::filesystem::path& vertexShaderPath,
                                   const std::filesystem::path& fragmentShaderPath)
    {
        generateShaderId();
        readSourceShaderFile(vertexShaderPath, fragmentShaderPath);
        compileShader();

        _shaderProgram.create(_shaderName);
        reflectShaderVariablesFor(_shaderProgram.getShaderProgramId());
        reflectShaderUniformBlocksFor(_shaderProgram.getShaderProgramId());
        _shaderProgram.setDataFromMeta(*this);

        _vertexShaderPath = vertexShaderPath;
        _fragmentShaderPath = fragmentShaderPath;
    }

    void ShaderProgramMeta::compileShader()
    {
        glCompileShader(_shaderProgram.getVertexShader());
        glCompileShader(_shaderProgram.getFragmentShader());
        requireNoCompileErrors();
    }

    void ShaderProgramMeta::requireNoCompileErrors()
    {
        checkShaderCompileStatus(_shaderProgram.getVertexShader(), "Vertex");
        checkShaderCompileStatus(_shaderProgram.getFragmentShader(), "Fragment");
    }

    void ShaderProgramMeta::generateShaderId()
    {
        _shaderProgram.clear();

        auto vertexShaderId = glCreateShader(static_cast<GLenum>(ShaderType::Vertex));
        if (vertexShaderId == 0)
        {
            criticalThrowingLog("Can't create gl vertex shader.");
        }
        _shaderProgram.setVertexShaderId(vertexShaderId);

        auto fragmentShaderId = glCreateShader(static_cast<GLenum>(ShaderType::Fragment));
        if (fragmentShaderId == 0)
        {
            criticalThrowingLog("Can't create gl fragment shader.");
        }
        _shaderProgram.setFragmentShaderId(fragmentShaderId);
    }

    void ShaderProgramMeta::readSourceShaderFile(const std::filesystem::path& vertexShaderPath,
                                                 const std::filesystem::path& fragmentShaderPath)
    {
        // VERTEX SHADER
        const auto vertexSources = Utils::GetTextFileContentAs<std::string>(vertexShaderPath);
        if (vertexSources.empty())
        {
            return;
        }
        const auto* vertexRaw = vertexSources.data();
        glShaderSource(_shaderProgram.getVertexShader(), 1, &vertexRaw, nullptr);

        // FRAGMENT SAHDER
        const auto fragmentSources = Utils::GetTextFileContentAs<std::string>(fragmentShaderPath);
        if (fragmentSources.empty())
        {
            return;
        }
        const auto* fragmentRaw = fragmentSources.data();
        glShaderSource(_shaderProgram.getFragmentShader(), 1, &fragmentRaw, nullptr);
    }

    void ShaderProgramMeta::setShaderName(const StringAtom& name)
    {
        _shaderName = StringAtom::Intern(name);
    }

    spdlog::logger* ShaderProgramMeta::getLogger() const
    {
        return Graphics::getLogger();
    }

    void ShaderProgramMeta::setShaderName(const std::string& name)
    {
        _shaderName = StringAtom::Intern(name);
    }

    void ShaderProgramMeta::checkShaderCompileStatus(GLuint shaderId, const std::string& shaderType)
    {
        constexpr GLsizei logSize = 512;
        GLint success = 0;

        glGetShaderiv(shaderId, GL_COMPILE_STATUS, &success);
        if (!success)
        {
            char infoLog[logSize]{};
            glGetShaderInfoLog(shaderId, logSize, nullptr, infoLog);
            std::string msg = shaderType + " shader compilation error: ";
            msg += infoLog;
            throw std::runtime_error(msg);
        }
    }

    void ShaderProgramMeta::reflectShaderVariablesFor(GLuint shaderProgramId)
    {
        if (!glGetProgramInterfaceiv)
        {
            warnLog("The function: glGetProgramInterfaceiv - is unavailable.");
            return;
        }

        _uniforms.clear();
        _inputs.clear();
        _outputs.clear();

        struct Group
        {
            GLenum interfaceType;
            std::unordered_set<ShaderVariable, ShaderVariable::Hasher>& output;
        };

        std::vector<Group> groups = { { GL_UNIFORM, _uniforms },
                                      { GL_PROGRAM_INPUT, _inputs },
                                      { GL_PROGRAM_OUTPUT, _outputs } };

        constexpr GLenum props[] = { GL_NAME_LENGTH, GL_TYPE, GL_LOCATION };

        for (const auto& [interfaceType, output] : groups)
        {
            GLint count = 0;
            glGetProgramInterfaceiv(shaderProgramId, interfaceType, GL_ACTIVE_RESOURCES, &count);

            for (GLint i = 0; i < count; ++i)
            {
                GLint values[3] = {};
                glGetProgramResourceiv(shaderProgramId, interfaceType, i, 3, props, 3, nullptr,
                                       values);

                const GLint nameLen = values[0];
                const GLenum type = values[1];
                const GLint location = values[2];

                if (location == -1)
                {
                    continue;
                }

                std::string name(nameLen, '\0');
                glGetProgramResourceName(shaderProgramId, interfaceType, i, nameLen, nullptr,
                                         name.data());
                if (!name.empty() && name.back() == '\0')
                {
                    name.pop_back();
                }

                output.insert(ShaderVariable{ StringAtom::Intern(name), type, { location } });
            }
        }
    }

    void ShaderProgramMeta::reflectShaderUniformBlocksFor(GLuint shaderProgramId)
    {
        if (!glGetProgramInterfaceiv)
        {
            warnLog("The function: glGetProgramInterfaceiv - is unavailable.");
            return;
        }

        _uniformBufferObjects.clear();

        constexpr GLenum props[] = { GL_NUM_ACTIVE_VARIABLES, GL_ACTIVE_VARIABLES,
                                     GL_BUFFER_BINDING, GL_BUFFER_DATA_SIZE };
        constexpr GLenum lineProps[] = { GL_NAME_LENGTH, GL_TYPE, GL_OFFSET };

        GLint count = 0;
        glGetProgramInterfaceiv(shaderProgramId, GL_UNIFORM_BLOCK, GL_ACTIVE_RESOURCES, &count);

        for (GLint i = 0; i < count; ++i)
        {
            ShaderUBO outData;

            // =========== Getting main data ===========
            GLint values[4] = {};
            glGetProgramResourceiv(shaderProgramId, GL_UNIFORM_BLOCK, i, 4, props, 4, nullptr,
                                   values);

            outData.vars.resize(values[0]);
            outData.binding = values[2];
            outData.size = values[3];

            // =========== Getting name ===========
            GLint nameLen = 0;
            constexpr GLenum nameLenEnum = GL_NAME_LENGTH;
            glGetProgramResourceiv(shaderProgramId, GL_UNIFORM_BLOCK, i, 1, &nameLenEnum, 1,
                                   nullptr, &nameLen);
            std::string blockName(nameLen, '\0');
            glGetProgramResourceName(shaderProgramId, GL_UNIFORM_BLOCK, i, nameLen, nullptr,
                                     blockName.data());
            if (!blockName.empty() && blockName.back() == '\0')
            {
                blockName.pop_back();
            }
            outData.name = StringAtom::Intern(blockName);

            // =========== Getting fields/vars ===========
            std::vector<GLint> vars(outData.vars.size());
            constexpr GLenum activeVarsEnum = GL_ACTIVE_VARIABLES;
            glGetProgramResourceiv(shaderProgramId, GL_UNIFORM_BLOCK, i, 1, &activeVarsEnum,
                                   outData.vars.size(), nullptr, vars.data());

            std::size_t varIndex = 0;
            for (const GLint varId : vars)
            {
                ShaderVariable var;

                GLint uboContent[3] = {};
                glGetProgramResourceiv(shaderProgramId, GL_UNIFORM, varId, 3, lineProps, 3, nullptr,
                                       uboContent);

                var.name.resize(uboContent[0]);
                glGetProgramResourceName(shaderProgramId, GL_UNIFORM, varId, uboContent[0], nullptr,
                                         var.name.data());
                if (!var.name.isEmpty() && var.name.back() == '\0')
                {
                    var.name.popBack();
                }

                var.type = uboContent[1];
                var.offset = uboContent[2];

                outData.vars.at(varIndex++) = std::move(var);
            }

            std::ranges::sort(outData.vars, [](const ShaderVariable& a, const ShaderVariable& b)
                              { return a.offset < b.offset; });

            _uniformBufferObjects.insert(std::move(outData));
        }
    }

    void ShaderProgramMeta::recreateFromSources()
    {
        if (_fragmentShaderPath.empty() || _vertexShaderPath.empty())
        {
            warnLog("Can't recreate a shader '{}', due to invalid paths"_f << _shaderName);
            return;
        }

        create(_vertexShaderPath, _fragmentShaderPath);
    }

    bool ShaderProgramMeta::safeRecreateFromSources()
    {
        try
        {
            recreateFromSources();
            return true;
        }
        catch (std::exception& err)
        {
            errorLog("Can't recreate a shader '{}' due to compile error[s]: {}"_f << _shaderName
                                                                                  << err.what());
            return false;
        }
    }
} // namespace Core