/*
* ~~~~~~~~~~~~~~~~~~~~~~~~~
* Homebrew Graphics Engine
* Zoltan Simon 2023
* Repository: https://github.com/TheFlyingPiano99/HomebrewGraphicsEngine
* ~~~~~~~~~~~~~~~~~~~~~~~~~
*/
#pragma once
#include "UIElement.h"
#include <string>
#include "Font.h"
#include "../Texture2D.h"
#include "../ShaderProgram.h"
#include "../ShaderProgramFactory.h"
#include "../FBO.h"
#include "../VAO.h"
#include "../VBO.h"
#include "../Geometry.h"

namespace Hogra {

    class Caption : public UIElement
    {
    public:

        void Init(const std::wstring& text, Font* font, float scale, const glm::vec4& textColor = { 0.0f, 0.0f, 0.0f, 1.0f }, const glm::vec4& highlightColor = { 0.0f, 0.0f, 0.0f, 0.0f });

        void Draw() const override;

        void UpdateText(const std::wstring& str);

        void SetTextColor(const glm::vec4& color);

        void SetHighlightColor(const glm::vec4& color);

        const std::wstring& GetText();

    private:
        std::wstring text;
        Font* font = nullptr;
        glm::vec4 textColor = { 0, 0, 0, 1 };
        glm::vec4 highlightColor = { 0, 0, 0, 0 };
        Texture2D* texture = nullptr;
        float scale = 1.0f;
    };
}

