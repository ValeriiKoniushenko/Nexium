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

#pragma once

#include "Core/Singleton.h"
#include "ModuleInfo.h"
#include "ShaderProgramMeta.h"

#include <unordered_set>

namespace SW
{
    class ShaderManager final : public Core::StrictSingleton<ShaderManager>, public BaseLog
    {
    public:
        inline static const char* const defaultVertexFileExtension = ".vert";
        inline static const char* const defaultFragmentFileExtension = ".frag";

        void loadShaders(const std::filesystem::path& path);

        void pushSuitableFileExtension(std::string ext, ShaderType type);

        [[nodiscard]] std::unordered_set<std::string>& getSuitableFragFileExtensions()
        {
            return _suitableFragExtensions;
        }
        [[nodiscard]] std::unordered_set<std::string>& getSuitableVertFileExtensions()
        {
            return _suitableVertExtensions;
        }

        [[nodiscard]] ShaderProgram* getShaderProgram(const Core::StringAtom& shaderName);

        [[nodiscard]] size_t countOfShaders() const { return _shaderMetas.size(); };
        [[nodiscard]] size_t countOfValidShaders() const;
        [[nodiscard]] size_t countOfFailedShaders() const { return _failedShaders.size(); }
        [[nodiscard]] const std::unordered_set<std::string>& getFailedShaders() const
        {
            return _failedShaders;
        }

        [[nodiscard]] spdlog::logger* getLogger() const override
        {
            return SW::RawGraphics::getLogger();
        }
        [[nodiscard]] const char* getPrefix() const override { return "ShaderManager"; }

    private:
        [[nodiscard]] std::filesystem::path getPathToShaderBasedOn(
            const std::unordered_set<std::string>& set, std::filesystem::path path) const;

    private:
        std::unordered_map<Core::StringAtom, ShaderProgramMeta> _shaderMetas;

        std::unordered_set<std::string> _suitableFragExtensions = { defaultFragmentFileExtension };
        std::unordered_set<std::string> _suitableVertExtensions = { defaultVertexFileExtension };

        std::unordered_set<std::string> _failedShaders;
    };

    inline ShaderManager& GetShaderManager()
    {
        return ShaderManager::instance();
    }
} // namespace SW
