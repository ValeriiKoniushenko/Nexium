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

#include "TextEditor.h"

#include "ImGui/misc/cpp/imgui_stdlib.h"
#include "Utils/Functions.h"

namespace Core
{
    ECS_REGISTER_NEW_COMPONENT_TYPE(TextEditorEWC)

    void TextEditorEWC::onInit()
    {
        BaseFloatEWC::onInit();
    }

    void TextEditorEWC::onDraw()
    {
        ImGui::PushTextWrapPos();
        ImGui::InputTextMultiline("##editor", &_fileContent,
                                  ImVec2(_innerSize.width, _innerSize.height));
        ImGui::PopTextWrapPos();
    }

    void TextEditorEWC::putArguments(const StringAtom& args)
    {
        BaseEWC::putArguments(args);

        _path = args.data();
        _fileContent = Utils::TryToGetTextFileContentAs<std::string>(_path);
        if (_fileContent.empty())
        {
            errorLog("TextEditor can't open file: {}"_f << _path.generic_string());
        }
    }
} // namespace Core
