/*
* ~~~~~~~~~~~~~~~~~~~~~~~~~
* Homebrew Graphics Engine
* Zoltan Simon 2023
* Repository: https://github.com/TheFlyingPiano99/HomebrewGraphicsEngine
* ~~~~~~~~~~~~~~~~~~~~~~~~~
*/
#pragma once
#include "UIElement.h"
#include "Font.h"
#include <functional>
#include "../DebugUtils.h"

namespace Hogra {

    class MenuButton : public UIElement
    {
    public:
        void Init(const std::wstring& text, Font* font);

        void Draw() const override;

        void SetFillColor(const glm::vec4& color) {
            fillColor = color;
        }

        void SetBorderColor(const glm::vec4& color) {
            borderColor = color;
        }

        void SetHoverFillColor(const glm::vec4& color) {
            hoverFillColor = color;
        }

        void SetHoverBorderColor(const glm::vec4& color) {
            hoverBorderColor = color;
        }

        void SetAction(const std::function<void(const glm::ivec2& screenCursorPos)>& _action)
        {
            action = _action;
        }

        void SetBackgroundTexture(Texture2D* texture)
        {
            backgroundTexture = texture;
        }

    private:

        virtual void ExecuteClickAction(const glm::ivec2& screenCursorPos);

        glm::vec4 fillColor = glm::vec4(0.02f, 0.02f, 0.02f, 0.6f);
        glm::vec4 borderColor = glm::vec4(0.1f, 0.1f, 0.1f, 0.6f);
        glm::vec4 hoverFillColor = glm::vec4(0.06f, 0.06f, 0.06f, 0.6f);
        glm::vec4 hoverBorderColor = glm::vec4(0.2f, 0.2f, 0.2f, 0.6f);
        Texture2D* backgroundTexture = nullptr;
        std::function<void(const glm::ivec2& screenCursorPos)> action = [](const glm::ivec2& screenCursorPos) { DebugUtils::PrintWarning("MenuButton", "Pressed button with no action!"); };
    };

}