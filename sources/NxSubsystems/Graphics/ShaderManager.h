// Nexium
// Copyright 2018-2026 Valerii Koniushenko
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0

#pragma once

#include "ShaderProgramMeta.h"

#include <unordered_set>

namespace NX
{
    class ShaderManager final : public Foundation::BaseLog, public Core::Singleton<ShaderManager>
    {
        SINGLETONS_FRIEND(ShaderManager);

    public:
        ~ShaderManager() override = default;

        inline static const char* const defaultVertexFileExtension = ".vert";
        inline static const char* const defaultFragmentFileExtension = ".frag";

        void loadShaders(const std::filesystem::path& inputPath);

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

        [[nodiscard]] const std::unordered_map<Core::StringAtom, ShaderProgramMeta>&
            getShaderMetas() const noexcept
        {
            return _shaderMetas;
        }

        [[nodiscard]] std::unordered_map<Core::StringAtom, ShaderProgramMeta>&
            getShaderMetas() noexcept
        {
            return _shaderMetas;
        }

        [[nodiscard]] size_t countOfShaders() const { return _shaderMetas.size(); }

        [[nodiscard]] size_t countOfValidShaders() const;

        [[nodiscard]] size_t countOfFailedShaders() const { return _failedShaders.size(); }

        [[nodiscard]] const std::unordered_set<std::string>& getFailedShaders() const
        {
            return _failedShaders;
        }

        [[nodiscard]] const std::filesystem::path& getInputDir() const noexcept
        {
            return _inputPath;
        }

        [[nodiscard]] spdlog::logger* getLogger() const override;
        [[nodiscard]] const char* getPrefix() const override { return "ShaderManager"; }

    private:
        [[nodiscard]] std::filesystem::path getPathToShaderBasedOn(
            const std::unordered_set<std::string>& set, std::filesystem::path path) const;

    private:
        std::unordered_map<Core::StringAtom, ShaderProgramMeta> _shaderMetas;

        std::unordered_set<std::string> _suitableFragExtensions = { defaultFragmentFileExtension };
        std::unordered_set<std::string> _suitableVertExtensions = { defaultVertexFileExtension };

        std::unordered_set<std::string> _failedShaders;

        std::filesystem::path _inputPath;
    };

    [[nodiscard]] ShaderManager& GetShaderManager();

} // namespace NX
