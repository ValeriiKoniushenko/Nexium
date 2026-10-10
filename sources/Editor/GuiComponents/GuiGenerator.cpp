// Nexium
// Copyright 2018-2026 Valerii Koniushenko
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0

// Module: Nexium::Editor

#include "GuiGenerator.h"

namespace NX::Gui
{

    Core::StringAtom GuiGenerator::NormalizeLabelName(const Core::StringAtom& name)
    {
        static auto isUpper
            = [](char c) { return std::isupper(static_cast<unsigned char>(c)) != 0; };
        static auto isLower
            = [](char c) { return std::islower(static_cast<unsigned char>(c)) != 0; };
        static auto isAlnum
            = [](char c) { return std::isalnum(static_cast<unsigned char>(c)) != 0; };

        // 1. Split into words
        std::vector<std::string> words;
        std::string current;

        auto flush = [&]
        {
            if (!current.empty())
            {
                words.push_back(std::move(current));
                current.clear();
            }
        };

        for (size_t i = 0; i < name.size(); ++i)
        {
            const char c = name[i];

            // Any non-alphanumeric character (_, -, space, ...) is a separator
            if (!isAlnum(c))
            {
                flush();
                continue;
            }

            if (!current.empty() && isUpper(c))
            {
                const char prev = current.back();
                const bool nextIsLower = (i + 1 < name.size()) && isLower(name[i + 1]);

                // "helloWorld" -> hello|World
                // "HTTPServer" -> HTTP|Server (upper followed by lower ends an acronym)
                if (isLower(prev) || (isUpper(prev) && nextIsLower))
                {
                    flush();
                }
            }

            current += c;
        }
        flush();

        // 2. Join: lowercase every word, except acronyms (2+ letters, all uppercase)
        std::string result;
        for (const std::string& word : words)
        {
            size_t letters = 0;
            bool allUpper = true;
            for (char c : word)
            {
                if (std::isalpha(static_cast<unsigned char>(c)))
                {
                    ++letters;
                    if (!isUpper(c))
                    {
                        allUpper = false;
                    }
                }
            }
            const bool isAcronym = allUpper && letters >= 2;

            if (!result.empty())
            {
                result += ' ';
            }

            for (char c : word)
            {
                result += isAcronym
                              ? c
                              : static_cast<char>(std::tolower(static_cast<unsigned char>(c)));
            }
        }

        // 3. Capitalize the first letter
        if (!result.empty())
        {
            result[0] = static_cast<char>(std::toupper(static_cast<unsigned char>(result[0])));
        }

        return Core::StringAtom(result);
    }

    void GuiGenerator::despawnEverything()
    {
        removeAllChildren();
    }

    void GuiGenerator::setLabelWidth(float v) noexcept
    {
        _labelWidth = std::max(5.f, v);
    }

} // namespace NX::Gui