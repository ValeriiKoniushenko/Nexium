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

#include "Map.h"

#include "AssetsManager/AssetsManager.h"
#include "Core/Rect.h"
#include "GameplaySystem/Framework/GameInstance.h"
#include "Player.h"
#include "Scene/Rectangle.h"

ECS_IMPL(Map);

void Map::onInitialize()
{
    createAllNeededRects();
}

void Map::createAllNeededRects()
{
    auto component
        = Core::GetAssetsManager()->getUniqueEcsAsset("data/assets/AnimatedRectangle.nx"_atom);
    _tree = DynamicCast<Core::SceneObj::RectangleAnimated>(component);
    if (!_tree)
    {
        errorLog("Can't load AnimatedRectangle asset for: tileRect");
        return;
    }
    _tree->setTexture("Tree_2.png"_atom);

    for (int i = 0; i < 5; ++i)
    {
        for (int j = 0; j < 25; ++j)
        {
            auto component = Core::GetAssetsManager()->getUniqueEcsAsset(
                "data/assets/AnimatedRectangle.nx"_atom);
            auto tileRect = DynamicCast<Core::SceneObj::RectangleAnimated>(component);

            if (!tileRect)
            {
                errorLog("Can't load AnimatedRectangle asset for: tileRect");
                return;
            }
            tileRect->setVisibleInSceneTree(false);

            tileRect->setTexture("Tiles"_atom, i == 0 ? "2.png"_atom : "5.png"_atom);

            const auto size = tileRect->getDrawRectSize();
            const auto posX = static_cast<float>(j) * size.width;
            const auto posY = -static_cast<float>(i + 1) * size.height;

            tileRect->setPosition({ posX, posY, 0.f });
            gGameInstance->gameScene.addObjectToScene(tileRect);
        }
    }
}

void Map::spawnNewTree()
{
    static int i = 0;
    if (i <= 0 && _tree)
    {
        _tree->setPosition({ _tree->getDrawRectSize().width * 25, 0.f, 0.f });

        gGameInstance->gameScene.addObjectToScene(_tree);
    }
    i++;
}

void Map::checkCollisions()
{
    const auto player = gGameInstance->gameScene.gerFirstOf<Player>();
    if (!player || !_tree)
    {
        return;
    }

    const auto playerRect = player->getGlobalTextureRect();
    const auto treeRect = _tree->getGlobalTextureRect();
    if (!playerRect || !treeRect)
    {
        return;
    }

    _playerTextureRect = *playerRect;

    const bool intersects = playerRect->getLeftTop().x <= treeRect->getRightBottom().x
                            && playerRect->getRightBottom().x >= treeRect->getLeftTop().x
                            && playerRect->getLeftTop().y >= treeRect->getRightBottom().y
                            && playerRect->getRightBottom().y <= treeRect->getLeftTop().y;

    if (intersects)
    {
        infoLog("player intersects with tree");
    }
}

void Map::moveTree(float delta)
{
    _tree->moveRight(-1 * (1000 * delta));

    if (_tree->getPosition().x <= -10)
    {
        _tree->setPosition({ _tree->getDrawRectSize().width * 25, 0.f, 0.f });
    }
}

void Map::onTick(float delta)
{
    spawnNewTree();
    checkCollisions();
    moveTree(delta);
}
