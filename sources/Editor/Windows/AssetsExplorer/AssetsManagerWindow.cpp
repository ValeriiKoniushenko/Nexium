/*
 * MIT License
 *
 * Copyright (c) 2018-2027 Valerii Koniushenko
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

#include "AssetsManagerWindow.h"

#include "../ModalCreateBlueprint.h"
#include "Editor/GuiComponents/Button.h"
#include "Editor/GuiComponents/Input.h"
#include "Editor/GuiComponents/Spacer.h"
#include "Editor/Windows/ModalPopUp.h"
#include "GameplaySystem/Framework/GameInstance.h"
#include "Misc/Configs.h"
#include "Misc/IconsFontAwesome.h"
#include "RenamePopUpWindow.h"
#include "ThumbnailFile.h"

#include <algorithm>
#include <cctype>
#include <format>
#include <fstream>

using NodeType = Core::AssetsManager::NodeType;

namespace
{
    [[maybe_unused]] bool IsSubpath(const std::filesystem::path& original,
                                    const std::filesystem::path& sub)
    {
        const auto rel = std::filesystem::relative(original, sub);
        return !rel.empty() && rel.native()[0] != '.';
    }

} // namespace

namespace Core
{
    R_FRIEND_IMPL(AssetsManagerWindowEWC);
    ECS_COMPONENT_IMPL(AssetsManagerWindowEWC);

    const char* AssetsManagerWindowEWC::getIcon()
    {
        return ICON_FA_FOLDER;
    }

    void AssetsManagerWindowEWC::tryOpenParentDir()
    {
        if (_openedPath.has_parent_path())
        {
            openPath(_openedPath.parent_path());
        }
    }

    void AssetsManagerWindowEWC::tryOpenPath(const std::filesystem::path& p)
    {
        if (std::filesystem::exists(p))
        {
            openPath(p);
        }
    }

    /*void AssetsManagerWindowEWC::ioFieldsUpdate(DataStream& out)
    {
        BaseFloatEWC::ioFieldsUpdate(out);

        auto stream = out.dedicatedNesting("AssetsManagerWindowEWC");

        stream.field("openedPath", _openedPath);
        if (stream.getMode() == DataStream::Mode::Input && !std::filesystem::exists(_openedPath))
        {
            _openedPath = Config::Path::projectAbsPath;
        }
    }*/

    void AssetsManagerWindowEWC::onPreInitialize()
    {
        BaseFloatEWC::onPreInitialize();
        _nodeTypesData = {
            { NodeType::Default,
              GetAssetsManager().getTexture("data/assets/baked/document.nxtex"_atom) },
            { NodeType::Code,
              GetAssetsManager().getTexture("data/assets/baked/code_document.nxtex"_atom) },
            { NodeType::Image,
              GetAssetsManager().getTexture("data/assets/baked/image_document.nxtex"_atom) },
            { NodeType::Folder,
              GetAssetsManager().getTexture("data/assets/baked/folder.nxtex"_atom) },
            { NodeType::NxFile,
              GetAssetsManager().getTexture("data/assets/baked/nxfile.nxtex"_atom) },
        };

        const auto gap = ImGui::GetStyle().WindowPadding.x;

        // Structure
        _toolbarLayout.setPaddings(glm::vec4(gap));

        _backButton = _toolbarLayout.addChildComponent<Gui::Button>(ICON_FA_CHEVRON_LEFT);
        _toolbarLayout.addChildComponent<Gui::Spacer>();
        _homeButton = _toolbarLayout.addChildComponent<Gui::Button>(ICON_FA_HOME);
        _refreshButton = _toolbarLayout.addChildComponent<Gui::Button>(ICON_FA_REFRESH);
        _pathInput = _toolbarLayout.addChildComponent<Gui::TextInput>();
        _toolbarLayout.addChildComponent<Gui::Spacer>();
        _filterInput = _toolbarLayout.addChildComponent<Gui::TextInput>();

        // Style
        _backButton->setWidth(_backButton->getHeight());
        _refreshButton->setWidth(_refreshButton->getHeight());
        _homeButton->setWidth(_homeButton->getHeight());
        _pathInput->setFlex(Gui::Flex::FlexWidth);
        _filterInput->setWidth(150.f);
        _filterInput->setPlaceholder("Filter...");

        // Events
        _subscriptionPool << _backButton->onClick->subscribeAndGetID([this]()
                                                                     { tryOpenParentDir(); });
        _subscriptionPool << _refreshButton->onClick->subscribeAndGetID([this]() { refresh(); });
        _subscriptionPool << _homeButton->onClick->subscribeAndGetID(
            [this]() { openPath(Config::Path::assets); });
        _subscriptionPool << _pathInput->onInput->subscribeAndGetID(
            [this](const char* path)
            {
                if (path)
                {
                    tryOpenPath(std::filesystem::path(path));
                }
            });
    }

    void AssetsManagerWindowEWC::onInitialize()
    {
        BaseFloatEWC::onInitialize();

        if (_openedPath.empty())
        {
            openPath(Config::Path::assets);
        }
        else
        {
            openPath(_openedPath);
        }

        refresh();
    }

    void AssetsManagerWindowEWC::onDraw()
    {
        drawExplorerTree();
        ImGui::SameLine();
        drawExplorer();
        if (ImGui::IsWindowFocused(ImGuiFocusedFlags_RootAndChildWindows)
            && !ImGui::GetIO().WantTextInput)
        {
            if (ImGui::IsKeyPressed(ImGuiKey_Backspace, false))
            {
                tryOpenParentDir();
            }
        }
    }

    void AssetsManagerWindowEWC::onUpdate()
    {
        BaseFloatEWC::onUpdate();
    }

    void AssetsManagerWindowEWC::openPath(const std::filesystem::path& path)
    {
        auto finalPath = path;

        if (finalPath.has_filename() && finalPath.has_extension())
        {
            finalPath = finalPath.remove_filename();
            selectPath(path, false);
        }

        _openedPath = finalPath;
        _pathInput->setInputtedData(finalPath.generic_string());
    }

    void AssetsManagerWindowEWC::copyFrom(const std::filesystem::path& path)
    {
        _isCopy = true;
        _selectedPath = path;
    }

    void AssetsManagerWindowEWC::cutFrom(const std::filesystem::path& path)
    {
        _isCopy = false;
        _selectedPath = path;
    }

    void AssetsManagerWindowEWC::pasteTo(std::filesystem::path path)
    {
        std::error_code er;

        if (_selectedPath.has_filename())
        {
            if (std::filesystem::exists(path / _selectedPath.filename()))
            {
                path = getExclusiveFileName(_selectedPath);
            }
        }

        std::filesystem::copy(_selectedPath, path, std::filesystem::copy_options::recursive, er);

        if (er)
        {
            errorLog("Error while copying of file: " + er.message());
        }
        else if (!_isCopy)
        {
            deleteAt(_selectedPath);
        }

        _selectedPath.clear();
        refresh();
    }

    void AssetsManagerWindowEWC::deleteAt(const std::filesystem::path& path)
    {
        std::error_code er;
        std::filesystem::remove_all(path, er);
        if (er)
        {
            errorLog("Error while deleting of file: " + er.message());
            return;
        }

        const auto normalizedPath = Normalize(path);

        if (!_selectedPath.empty() && Normalize(_selectedPath) == normalizedPath)
        {
            _selectedPath.clear();
        }

        std::erase_if(
            _selectedPaths, [&normalizedPath](const std::filesystem::path& selectedPath)
            { return AssetsManagerWindowEWC::Normalize(selectedPath) == normalizedPath; });
        refresh();
    }

    void AssetsManagerWindowEWC::createFolder(const std::filesystem::path& path)
    {
        std::error_code er;
        std::filesystem::create_directory(path, er);
        if (er)
        {
            errorLog("Error while creating of the directory: " + er.message());
        }

        refresh();
    }

    void AssetsManagerWindowEWC::createFolderAutoName(const std::filesystem::path& basePath)
    {
        if (!std::filesystem::exists(basePath))
        {
            errorLog("Attempt to create a folder with non-existing path: "
                     + basePath.generic_string());
            return;
        }

        const std::string baseName = "NewFolder_";
        std::size_t i = 0;
        while (std::filesystem::exists(basePath / (baseName + std::to_string(i++))))
        {
        }

        createFolder(basePath / (baseName + std::to_string(i)));
    }

    void AssetsManagerWindowEWC::createFile(const std::filesystem::path& path)
    {
        if (std::filesystem::exists(path))
        {
            errorLog("You are trying to create a file, but a file with such name already exists: "
                     + path.generic_string());
            return;
        }

        std::ofstream file(path);
        file.close();

        refresh();
    }

    void AssetsManagerWindowEWC::createFileAutoName(const std::filesystem::path& basePath)
    {
        if (!std::filesystem::exists(basePath))
        {
            errorLog("Attempt to create a file with non-existing path: "
                     + basePath.generic_string());
            return;
        }

        if (!std::filesystem::exists(basePath / defaultNewFileName))
        {
            createFile(basePath / defaultNewFileName);
            return;
        }

        std::size_t i = 1;
        while (std::filesystem::exists(
            basePath / (std::string(defaultNewFileName) + "_" + std::to_string(i))))
        {
            ++i;
        }

        createFile(basePath / (std::string(defaultNewFileName) + "_" + std::to_string(i)));
    }

    std::filesystem::path AssetsManagerWindowEWC::getExclusiveFileName(
        const std::filesystem::path& path) const
    {
        if (path.empty())
        {
            return path;
        }

        auto filename = path.filename().generic_string();
        auto parentPath = path.parent_path();

        while (std::filesystem::exists(parentPath / filename))
        {
            const auto num = std::atoi(filename.c_str());
            while (!filename.empty() && std::isdigit(filename[0]))
            {
                filename.erase(filename.begin());
            }

            if (!filename.empty() && filename[0] != '_')
            {
                filename = "_" + filename;
            }

            filename = std::to_string(num + 1) + filename;
        }

        return parentPath / filename;
    }

    bool AssetsManagerWindowEWC::isFiltered(const std::filesystem::path& p) const
    {
        if (!_filterInput)
        {
            return false;
        }

        auto&& filter = _filterInput->getInputtedData();

        if (filter.empty() || filter[0] == '\0')
        {
            return false;
        }

        return !p.generic_string().contains(filter);
    }

    bool AssetsManagerWindowEWC::isSelected(const std::filesystem::path& p) const
    {
        return std::find(_selectedPaths.begin(), _selectedPaths.end(), p) != _selectedPaths.end();
    }

    void AssetsManagerWindowEWC::saveThumbnailSelectionAfterRename(
        const std::filesystem::path& oldPath, const std::filesystem::path& newPath)
    {
        const auto normOld = Normalize(oldPath);
        if (Normalize(_selectedPath) == normOld)
        {
            _selectedPath = newPath;
        }
        for (auto& p : _selectedPaths)
        {
            if (Normalize(p) == normOld)
            {
                p = newPath;
            }
        }
    }

    ThumbnailActions AssetsManagerWindowEWC::buildThumbnailActions()
    {
        return ThumbnailActions{
            .removeSelected =
                [w = WeakPtr(this)]()
            {
                if (w)
                {
                    if (auto weakData = w.tryLoad())
                    {
                        weakData->deleteSelectedFiles();
                    }
                }
            },
            .cut =
                [w = WeakPtr(this)](const std::filesystem::path& p)
            {
                if (w)
                {
                    if (auto weakData = w.tryLoad())
                    {
                        weakData->cutFrom(p);
                    }
                }
            },

            .copy =
                [w = WeakPtr(this)](const std::filesystem::path& p)
            {
                if (w)
                {
                    if (auto weakData = w.tryLoad())
                    {
                        weakData->copyFrom(p);
                    }
                }
            },

            .open =
                [w = WeakPtr(this)](const std::filesystem::path& p)
            {
                if (w)
                {
                    if (auto weakData = w.tryLoad())
                    {
                        weakData->tryOpenPath(p);
                    }
                }
            },

            .openInExplorer =
                [w = WeakPtr(this)](const std::filesystem::path& p)
            {
                if (w)
                {
                    std::error_code ec;
                    const bool isDir = std::filesystem::is_directory(p, ec);

                    if (ec)
                    {
                        AssetsManager::OpenPathFromOSExplorer(p);
                        return;
                    }
                    if (auto weakData = w.tryLoad())
                    {
                        const auto pathToOpen = (w && isDir) ? p : (w ? weakData->_openedPath : p);
                        AssetsManager::OpenPathFromOSExplorer(pathToOpen);
                    }
                }
            },

            .paste =
                [w = WeakPtr(this)](const std::filesystem::path& p)
            {
                if (w)
                {
                    if (auto weakData = w.tryLoad())
                    {
                        weakData->pasteTo(p);
                    }
                }
            },

            .remove =
                [w = WeakPtr(this)](const std::filesystem::path& p)
            {
                if (w)
                {
                    if (auto weakData = w.tryLoad())
                    {
                        weakData->deleteAt(p);
                    }
                }
            },

            .rename =
                [w = WeakPtr(this)](const std::filesystem::path& p)
            {
                RenamePopUpWindow::Open(
                    "Rename file name", p,
                    [w](const std::filesystem::path& oldP, const std::filesystem::path& newP)
                    {
                        if (w)
                        {
                            if (auto weakData = w.tryLoad())
                            {
                                weakData->saveThumbnailSelectionAfterRename(oldP, newP);
                                weakData->refresh();
                            }
                        }
                    });
            },
            .isMultiSelection =
                [w = WeakPtr(this)](const std::filesystem::path& p)
            {
                if (w)
                {
                    if (auto weakData = w.tryLoad())
                    {
                        return weakData->isSelected(p) && weakData->_selectedPaths.size() > 1;
                    }
                }

                return false;
            },

            .select =
                [w = WeakPtr(this)](const std::filesystem::path& p, bool additive)
            {
                if (w)
                {
                    if (auto weakData = w.tryLoad())
                    {
                        weakData->selectPath(p, additive);
                    }
                }
            },
        };
    }

    std::vector<std::filesystem::directory_entry> AssetsManagerWindowEWC::prepareExplorerEntries()
        const
    {
        std::vector<std::filesystem::directory_entry> entries;

        try
        {
            for (const auto& entry : std::filesystem::directory_iterator(_openedPath))
            {
                if (!isFiltered(entry.path()))
                {
                    entries.push_back(entry);
                }
            }
        }
        catch (const std::filesystem::filesystem_error& e)
        {
            criticalLog("Scan error in '{}': {}"_f << _openedPath.generic_string() << e.what());
            return {};
        }

        std::ranges::sort(entries,
                          [](const auto& a, const auto& b)
                          {
                              const bool aDir = a.is_directory();
                              const bool bDir = b.is_directory();

                              if (aDir != bDir)
                              {
                                  return aDir; // folders first
                              }

                              auto an = a.path().filename().string();
                              auto bn = b.path().filename().string();

                              std::ranges::transform(an, an.begin(), ::tolower);
                              std::ranges::transform(bn, bn.begin(), ::tolower);

                              return an < bn;
                          });

        return entries;
    }

    void AssetsManagerWindowEWC::selectPath(const std::filesystem::path& path, const bool additive)
    {
        _selectedPath = path;

        if (additive)
        {
            toggleSelection(path);
            return;
        }

        _selectedPaths.clear();
        _selectedPaths.push_back(path);
    }

    void AssetsManagerWindowEWC::drawExplorerContextMenu()
    {
        if (ImGui::BeginPopupContextWindow("ExplorerContextMenu",
                                           ImGuiPopupFlags_MouseButtonRight
                                               | ImGuiPopupFlags_NoOpenOverItems))
        {
            if (ImGui::MenuItem(ICON_FA_CHEVRON_LEFT " Back"))
            {
                tryOpenParentDir();
            }
            if (!_selectedPath.empty() && ImGui::MenuItem(ICON_FA_ARROW_DOWN " Paste"))
            {
                pasteTo(_openedPath);
            }
            if (ImGui::MenuItem(ICON_FA_REFRESH " Refresh"))
            {
                refresh();
            }
            if (ImGui::MenuItem(ICON_FA_PLUS " Create a folder"))
            {
                createFolderAutoName(_openedPath);
            }
            if (ImGui::MenuItem(ICON_FA_PLUS " Create a file"))
            {
                createFileAutoName(_openedPath);
            }

            ImGui::Separator();

            if (ImGui::MenuItem("Open in explorer"))
            {
                AssetsManager::OpenPathFromOSExplorer(_openedPath);
            }

            ImGui::EndPopup();
        }
    }

    void AssetsManagerWindowEWC::drawAssetsContextMenu(
        const std::filesystem::directory_entry& entry, bool& invalidate, bool& needOpen)
    {
        const auto& path = entry.path();
        if (ImGui::BeginPopup(path.filename().generic_string().c_str()))
        {
            if (ImGui::MenuItem(ICON_FA_FOLDER_OPEN_O " Open"))
            {
                needOpen = true;
            }
            if (ImGui::MenuItem(ICON_FA_FILES_O " Copy"))
            {
                copyFrom(path);
            }
            if (ImGui::MenuItem(ICON_FA_SCISSORS " Cut"))
            {
                cutFrom(path);
            }
            if (!_selectedPath.empty() && entry.is_directory()
                && ImGui::MenuItem(ICON_FA_ARROW_DOWN " Paste"))
            {
                pasteTo(path);
            }

            if (ImGui::MenuItem(ICON_FA_PENCIL " Rename"))
            {
                RenamePopUpWindow::Open("Rename file name", path,
                                        [this](const std::filesystem::path& oldPath,
                                               const std::filesystem::path& newPath)
                                        {
                                            saveThumbnailSelectionAfterRename(oldPath, newPath);
                                            refresh();
                                        });
            }
            if (ImGui::MenuItem(ICON_FA_TRASH " Delete"))
            {
                ModalPopUp::Open("Do you really want to delete the {}: {}?"_f
                                     << (entry.is_directory() ? "directory" : "file")
                                     << path.generic_string(),
                                 [this, path](const bool isOk)
                                 {
                                     if (isOk)
                                     {
                                         deleteAt(path);
                                     }
                                 });

                invalidate = true;
            }

            ImGui::Separator();

            if (ImGui::MenuItem("Open in explorer"))
            {
                AssetsManager::OpenPathFromOSExplorer(entry.is_directory() ? path : _openedPath);
            }

            const auto weakAsset = GetAssetsManager().getWeakEcsAssetByPath(path.generic_string());
            if (auto asset = weakAsset.tryLoad(); asset && (asset->getTags() & Tag_WorldObject))
            {
                if (ImGui::MenuItem("Spawn on scene"))
                {
                    gGameInstance->gameScene.addBlueprintObjectToScene(asset, "");
                }
            }

            ImGui::EndPopup();
        }
    }

    void AssetsManagerWindowEWC::drawExplorerTree()
    {
        if (ImGui::BeginChild("Explorer tree", glm::vec2(200.0f, 0), ImGuiChildFlags_ResizeX))
        {
            ImGui::Dummy({});
            ImGui::Dummy({});
            ImGui::SameLine();

            float const w
                = ImGui::GetContentRegionAvail().x - (ImGui::GetStyle().ItemSpacing.x * 1.f);

            if (ImGui::Button("Create Blueprint", glm::vec2(w, 0)))
            {
                ModalCreateBlueprintEWC::Open("Create Blueprint");
            }
            ImGui::Dummy({});

            bool isSelected = false;
            drawOneLevel(_rootCacheNode, isSelected);
            ImGui::Dummy({});
        }
        ImGui::EndChild();
    }

    void AssetsManagerWindowEWC::drawExplorer()
    {
        const auto& style = ImGui::GetStyle();
        const auto defaultSpace = style.ItemSpacing;
        const auto padding = style.WindowPadding.x;

        if (!ImGui::BeginChild("Explorer"))
        {
            return;
        }

        _toolbarLayout.tick(GetWorld().timeDelta);

        const auto availX = ImGui::GetContentRegionAvail().x - (padding * 2.f);

        const int maxCountPerWidth
            = static_cast<int>(availX / (_thumbnailSize.x + (defaultSpace.x * 2.f)));

        drawExplorerContextMenu();

        ImGui::SetCursorPosX(ImGui::GetCursorPosX() + padding);
        ImGui::PushStyleVar(ImGuiStyleVar_FrameBorderSize, 0.0f);

        const auto entries = prepareExplorerEntries();
        const auto actions = buildThumbnailActions();

        drawExplorerItems(entries, maxCountPerWidth, actions);

        ImGui::PopStyleVar();

        if (ImGui::IsMouseClicked(ImGuiMouseButton_Left)
            && ImGui::IsWindowHovered(ImGuiHoveredFlags_ChildWindows) && !ImGui::IsAnyItemHovered())
        {
            _selectedPaths.clear();
        }

        ImGui::Dummy({});
        ImGui::EndChild();
    }

    void AssetsManagerWindowEWC::drawExplorerItems(
        const std::vector<std::filesystem::directory_entry>& entries, const int maxCountPerWidth,
        const ThumbnailActions& actions)
    {
        int i = 1;

        for (const auto& entry : entries)
        {
            const auto& path = entry.path();
            const auto fileFormat = AssetsManager::GetNodeType(entry);
            const ImTextureID texture = _nodeTypesData[fileFormat]->getData().getTextureId();

            ThumbnailFile thumbnail(actions, texture, entry, isSelected(path),
                                    !_selectedPath.empty());

            thumbnail.draw();

            if (maxCountPerWidth != 0 && i % maxCountPerWidth != 0)
            {
                ImGui::SameLine();
            }

            ++i;
        }
    }

    void AssetsManagerWindowEWC::drawOneLevel(CacheNode& rootNode, bool& isSelected)
    {
        for (auto&& node : rootNode.children)
        {
            int flags = _commonTreeFlags;
            std::string filename = node.path.filename().generic_string();

            if (node.type != NodeType::Folder)
            {
                if (!_renderFilesInTreeView)
                {
                    continue;
                }
                const auto* icon = ICON_FA_FILE;
                if (node.type == NodeType::Code)
                {
                    icon = ICON_FA_FILE_CODE_O;
                }
                else if (node.type == NodeType::Image)
                {
                    icon = ICON_FA_FILE_IMAGE_O;
                }
                filename = icon + (" " + filename);
                flags |= ImGuiTreeNodeFlags_Leaf;
            }

            filename = ICON_FA_FOLDER + (" " + filename);

            auto atLeastOneFolder = std::ranges::any_of(node.children, [](const CacheNode& n)
                                                        { return n.type == NodeType::Folder; });

            if (!atLeastOneFolder)
            {
                flags |= ImGuiTreeNodeFlags_Leaf;
            }

            const bool isOpened = ImGui::TreeNodeEx(filename.c_str(), flags);

            // Check for LMB click to check content
            if (node.type == NodeType::Folder && ImGui::IsItemHovered()
                && ImGui::IsMouseClicked(ImGuiMouseButton_Left))
            {
                openPath(node.path);
                isSelected = true;
            }

            if (isOpened)
            {
                if (node.type == NodeType::Folder)
                {
                    drawOneLevel(node, isSelected);
                }

                ImGui::TreePop();
            }
        }
    }

    void AssetsManagerWindowEWC::deleteSelectedFiles()
    {
        const auto selected = _selectedPaths;

        for (const auto& path : selected)
        {
            deleteAt(path);
        }

        _selectedPaths.clear();
        refresh();
    }

    void AssetsManagerWindowEWC::toggleSelection(const std::filesystem::path& path)
    {
        const auto it = std::find(_selectedPaths.begin(), _selectedPaths.end(), path);

        if (it == _selectedPaths.end())
        {
            _selectedPaths.push_back(path);
        }
        else
        {
            _selectedPaths.erase(it);
        }
    }

    void AssetsManagerWindowEWC::rescanPhysicalDrive(CacheNode& node)
    {
        try
        {
            for (auto&& entry : std::filesystem::directory_iterator(node.path))
            {
                if (!entry.is_regular_file() && !entry.is_directory())
                {
                    continue;
                }

                const auto nodeType = AssetsManager::GetNodeType(entry);

                CacheNode tmp;
                tmp.path = entry.path();
                tmp.type = nodeType;
                node.children.push_back(std::move(tmp));

                if (nodeType == AssetsManager::NodeType::Folder)
                {
                    rescanPhysicalDrive(node.children.back());
                }
            }
        }
        catch (std::filesystem::filesystem_error& e)
        {
            criticalLog("Got a error while scanning a folder '{}' for assets. Details: {}"_f
                        << node.path.generic_string() << e.what());
        }
    }

    void AssetsManagerWindowEWC::refresh()
    {
        _rootCacheNode = {};

        _rootCacheNode.path = Config::Path::assets;
        rescanPhysicalDrive(_rootCacheNode);
        GetAssetsManager().refreshFilesSystem();
    }
} // namespace Core
