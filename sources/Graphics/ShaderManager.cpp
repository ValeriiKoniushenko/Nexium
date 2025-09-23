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

#include "ShaderManager.h"

namespace Core
{

    void ShaderManager::loadShaders(const std::filesystem::path& inputPath)
    {
        _shaderMetas.clear();
        _failedShaders.clear();
        _inputPath = inputPath;

        std::unordered_set<std::string> processedShaders;

        for (const auto& entry : std::filesystem::recursive_directory_iterator(inputPath))
        {
            if (!entry.is_regular_file())
            {
                continue;
            }

            const auto fragShaderFile
                = getPathToShaderBasedOn(_suitableFragExtensions, entry.path());
            if (fragShaderFile.empty())
            {
                continue;
            }
            const auto vertShaderFile
                = getPathToShaderBasedOn(_suitableVertExtensions, entry.path());
            if (vertShaderFile.empty())
            {
                continue;
            }

            if (vertShaderFile.stem() != fragShaderFile.stem())
            {
                warnLog("Different stem of shader files: '{}' & '{}'"_f
                        << fragShaderFile.generic_string() << vertShaderFile.generic_string());
                continue;
            }

            const auto name = std::filesystem::relative(entry.path(), inputPath)
                                  .replace_extension("")
                                  .generic_string();

            if (processedShaders.contains(name))
            {
                continue;
            }

            infoLog("Was found shader: {}"_f << name);

            try
            {
                processedShaders.emplace(name);

                ShaderProgramMeta meta;
                meta.setShaderName(name);
                meta.create(vertShaderFile, fragShaderFile);

                _shaderMetas[meta.getShaderName()] = std::move(meta);
            }
            catch (const std::exception& exception)
            {
                errorLog("Impossible to set up the shader '{}'. Details: {}"_f << name
                                                                               << exception.what());
                _failedShaders.emplace(name);
            }
        }
    }

    void ShaderManager::pushSuitableFileExtension(std::string ext, ShaderType type)
    {
        if (type == ShaderType::Fragment)
        {
            debugLog(
                "Was added mapping between Fragment shader & file content with extension '{}'."_f
                << ext);
            _suitableFragExtensions.emplace(std::move(ext));
        }
        else if (type == ShaderType::Vertex)
        {
            debugLog("Was added mapping between Vertex shader & file content with extension '{}'."_f
                     << ext);
            _suitableVertExtensions.emplace(std::move(ext));
        }
        else
        {
            errorLog(
                "Was passed incorrect shader type for mapping file extensions. Extension '{}' will be ignored"_f
                << ext);
        }
    }

    ShaderProgram* ShaderManager::getShaderProgram(const StringAtom& shaderName)
    {
        Assert(shaderName.isStatic());

        if (const auto it = _shaderMetas.find(shaderName); it != _shaderMetas.cend())
        {
            Assert(it->second.getShaderName().isStatic());
            return &it->second.getShaderProgram();
        }

        return nullptr;
    }

    size_t ShaderManager::countOfValidShaders() const
    {
        const std::size_t count = _shaderMetas.size() - _failedShaders.size();
        return std::max<std::size_t>(0, count);
    }

    std::filesystem::path ShaderManager::getPathToShaderBasedOn(
        const std::unordered_set<std::string>& set, std::filesystem::path path) const
    {
        for (auto&& ext : set)
        {
            path.replace_extension(ext);
            if (std::filesystem::exists(path))
            {
                return path;
            }
        }

        return {};
    }

} // namespace Core
