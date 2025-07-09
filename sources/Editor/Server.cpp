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

#include "Server.h"

#include "Utils/Functions.h"

using namespace httplib;

namespace SW
{
    void EditorServer::initialize()
    {
        _server.set_logger(
            [this](const Request& request, const Response& response)
            {
                debugLog("Site requests: " + request.path);
            });

        _server.Get("/api/getCountObjectOnScene",
                    [](const Request& req, Response& res)
                    {
                        const std::string webPath = req.path == "/" ? "/index.html" : req.path;
                        const std::string filePath = assetsPath + webPath;
                        res.set_file_content(filePath);
                    });
    }

    void EditorServer::start()
    {
        _server.listen("localhost", 61005);
    }
} // namespace SW