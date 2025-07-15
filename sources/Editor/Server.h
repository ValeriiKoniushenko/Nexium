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
#include "HttpLib/httplib.h"
#include "ModuleInfo.h"

#include <thread>

namespace SW
{

    class EditorServer : public Core::StrictSingleton<EditorServer>, public BaseLog
    {
        SINGLETONS_FRIEND(EditorServer)

    public:
        static constexpr const char* editorPath = "editor/dist";
        static constexpr int defaultPort = 61005;

    public:
        ~EditorServer() override;

        void setPort(int port);
        [[nodiscard]] bool isRunning() const;

        void initialize();
        void start();
        void stop();

        [[nodiscard]] spdlog::logger* getLogger() const final { return Editor::getLogger(); }
        [[nodiscard]] const char* getPrefix() const override { return "Server"; }

    private:
        void processGetRequest(const httplib::Request& req, httplib::Response& res);

    private:
        std::thread _thread;
        httplib::Server _server;
        int _port = defaultPort;
    };

    inline EditorServer& GetEditorServer()
    {
        return EditorServer::instance();
    }

} // namespace SW
