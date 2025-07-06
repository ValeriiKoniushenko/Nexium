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

namespace
{
    std::string getMimeType(const std::string& path)
    {
        if (path.ends_with(".html"))
        {
            return "text/html";
        }
        if (path.ends_with(".css"))
        {
            return "text/css";
        }
        if (path.ends_with(".js"))
        {
            return "application/javascript";
        }
        if (path.ends_with(".png"))
        {
            return "image/png";
        }
        if (path.ends_with(".jpg") || path.ends_with(".jpeg"))
        {
            return "image/jpeg";
        }
        if (path.ends_with(".ico"))
        {
            return "image/x-icon";
        }
        return "application/octet-stream";
    }

} // namespace

namespace SW
{
    void EditorServer::initialize()
    {
        _server.Get(".*",
                    [](const Request& req, Response& res)
                    {
                        std::string webPath = req.path == "/" ? "/index.html" : req.path;
                        std::string filePath = assetsPath + webPath;

                        if (std::filesystem::exists(filePath))
                        {
                            const auto mime = getMimeType(filePath);
                            auto content = Utils::TryToGetTextFileContentAs<std::string>(filePath);

                            res.set_content(std::move(content), mime);
                        }
                        else
                        {
                            res.status = 404;
                            res.set_content("Not Found", "text/plain");
                        }
                    });
    }

    void EditorServer::start()
    {
        _server.listen("localhost", 61005);
    }
} // namespace SW