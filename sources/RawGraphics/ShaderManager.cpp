// MIT License
//
// Copyright (c) 2019-2025 Valerii Koniushenko
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.

#include "ShaderManager.h"

namespace SW
{

    void ShaderManager::loadShader(const std::filesystem::path& inputPath)
    {
        _shaderMetas.clear();
        _validShaders = 0;
        _failedShaders = 0;

        for (const auto& entry : std::filesystem::recursive_directory_iterator(inputPath))
        {
            if (!entry.is_regular_file())
            {
                continue;
            }

            const auto filePath = entry.path();
            const auto extension = filePath.extension().generic_string();
            if (!_suitableExtensions.contains(extension))
            {
                continue;
            }

            const auto baseName = filePath.stem().generic_string();

            ShaderProgramMeta shader;
            shader.setShaderName(baseName);

            // here we are fetching only separated shaders. Let's join it.
            const ShaderType type = _suitableExtensions[extension];
            if (type == ShaderType::Vertex)
            {
                shader.setShader(VertexShader(filePath));
            }
            else if (type == ShaderType::Fragment)
            {
                shader.setShader(FragmentShader(filePath));
            }
            else
            {
                errorLog("Can't detect ShaderType with provided one file extension for that: '{}'"_f
                         << extension);
                return;
            }

            // const auto vertexPath = path / (baseName + _vertexExt);
            // const auto fragmentPath = path / (baseName + _fragmentExt);

            // if (!std::filesystem::exists(vertexPath) || !std::filesystem::exists(fragmentPath))
            // {
            //     continue;
            // }

            // try
            // {
            //     ShaderProgramMeta meta(VertexShader(vertexPath), FragmentShader(fragmentPath),
            //                            baseName // имя напрямую как string
            //     );

            //     _shaderMetas.emplace_back(std::move(meta));
            //     ++_validShaders;
            // }
            // catch (const std::exception& ex)
            // {
            //     spdlog::error("[ShaderManager] Failed to load shader '{}': {}", baseName,
            //                   ex.what());
            //     ++_failedShaders;
            // }
        }
    }

    ShaderProgram ShaderManager::getShaderProgram(const Core::StringAtom& shaderName)
    {
        return {};
    }

} // namespace SW
