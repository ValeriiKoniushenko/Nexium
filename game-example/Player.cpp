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

#include "Player.h"

#include "Core/Size.h"
#include "NxSubsystems/AssetsManager/AssetsManager.h"
#include "NxWorld/Framework/GameInstance.h"
#include "NxWorld/Scene/Rectangle.h"
#include "Platform/Keyboard.h"

using namespace Core;
using namespace NX;
using Platform::Keyboard;

ECS_IMPL(Player);

void Player::onInitialize()
{
    SceneObj::RectangleAnimated::onInitialize();

    _animator = findFirstChildOf<Animation::FrameByFrameAnimator>();
}

void Player::spawnTree()
{
    auto component = GetAssetsManager()->getUniqueEcsAsset("data/assets/AnimatedRectangle.nx"_atom);

    auto tree = DynamicCast<SceneObj::RectangleAnimated>(component);
    if (!tree)
    {
        errorLog("Can't load AnimatedRectangle asset");
        return;
    }

    tree->setComponentName("Tree"_atom);
    tree->setPosition(getPosition());
    tree->setTexture("Tree_2.png"_atom);

    gGameInstance->gameScene.addObjectToScene(std::move(tree));

    infoLog("Tree spawned");
}

void Player::onTick(float delta)
{
    RectangleAnimated::onTick(delta);
    const bool enterPressed = Keyboard::IsKeyPressed(Keyboard::Key::Enter);
    if (enterPressed && !_enterWasPressed)
    {
        spawnTree();
    }
    _enterWasPressed = enterPressed;

    if (_isGrounded)
    {
        if (!_animator->getAnimation("Santa_run")->isPlaying())
        {
            _animator->startAnimation("Santa_run");
        }
    }
    else
    {
        // TODO: Add animation "santa_jump"
    }

    if (_isGrounded && Keyboard::IsKeyPressed(Keyboard::Key::Space))
    {
        _isGrounded = false;
        _velocityY = _jumpForce;
    }

    _velocityY += _gravity * delta;

    auto pos = getPosition();
    pos.y += _velocityY * delta;

    setPosition(pos);

    const float textureLocalPosY = pos.y + getLocalTextureRect()->getLeftBottom().y * getScale().y;

    if (_velocityY <= 0.f && textureLocalPosY <= _groundCoords)
    {
        pos.y += _groundCoords - textureLocalPosY;
        _velocityY = 0.f;
        _isGrounded = true;
    }
    else
    {
        _isGrounded = false;
    }

    setPosition(pos);
}
