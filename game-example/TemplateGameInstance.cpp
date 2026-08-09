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

#include "TemplateGameInstance.h"

#include "Editor/Windows/GameViewport.h"

using namespace Core;

TemplateGameInstance::TemplateGameInstance(int argc, char** argv)
    : GameInstance(argc, argv)
{
}

void TemplateGameInstance::onInitializeReadCache()
{
    auto spectator = gameScene.gerFirstOf<Spectator>();
    if (!spectator)
    {
        errorLog("Spectator was not found in the loaded scene.");
        return;
    }

    auto* input = spectator->findFirstChildOf<InputController>();
    if (!input)
    {
        errorLog("InputController was not found under Spectator.");
        return;
    }

    _spectator = spectator;
    _spectatorInput = input;

    input->setActivationPredicate(
        [this]
        {
            if (renderMode == RenderMode::GameOnly)
                return true;

            const auto* viewport = gameEditor.getWindow<GameViewportEWC>();
            return viewport && viewport->isFocused();
        });

}

void TemplateGameInstance::onTick(float delta)
{
    auto spectator = _spectator.tryLoad();
    auto input = _spectatorInput.tryLoad();

    if (!spectator || !input || !input->isInputActive())
        return;

    const auto move = [&](const StringAtom& action, auto callback, float direction)
    {
        if (input->isActionPressed(action))
            callback(direction * getSpectatorSpeed(action) * delta);
    };

    move("Move forward"_atom, [&](float value) { spectator->moveForward(value); }, -1.f);
    move("Move backward"_atom, [&](float value) { spectator->moveForward(value); }, 1.f);
    move("Move right"_atom, [&](float value) { spectator->moveRight(value); }, 1.f);
    move("Move left"_atom, [&](float value) { spectator->moveRight(value); }, -1.f);
    move("Move up"_atom, [&](float value) { spectator->moveUp(value); }, 1.f);
    move("Move down"_atom, [&](float value) { spectator->moveUp(value); }, -1.f);
}

float TemplateGameInstance::getSpectatorSpeed(const StringAtom& action) const
{
    const auto spectator = _spectator.tryLoad();
    const auto input = _spectatorInput.tryLoad();
    if (!spectator || !input)
        return 0.f;

    const bool shift = (input->getActionModifiers(action) & InputModifier::Shift)
                       != InputModifier::None;
    return spectator->speed / (shift ? 8.f : 1.f);
}
