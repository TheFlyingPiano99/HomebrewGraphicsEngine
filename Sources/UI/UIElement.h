#pragma once
#include "../GlobalInclude.h"
#include "../ShaderProgram.h"
#include "../Geometry.h"

namespace Hogra {

    class UIElement {
        ALLOCATOR_CONSTRUCTIBLE
    public:

        ~UIElement() {
            for (auto child : children) {
                Allocator::Delete(child);
            }
        }

        ShaderProgram* GetShaderProgram() {
            return shaderProgram;
        }

        virtual void Draw() const = 0;

        bool IsVisible() const;

        void SetIsVisible(bool b);

        enum class Floating {
            leftToRight,
            topToBottom
        };

        enum class HorizontalAlignment {
            centered,
            left,
            right
        };

        enum class VerticalAlignment {
            centered,
            top,
            bottom
        };

        void SetHorizontalAlignment(HorizontalAlignment alignment) {
            horizontalAlignment = alignment;
        }

        void SetVerticalAlignment(VerticalAlignment alignment) {
            verticalAlignment = alignment;
        }

        void CalculateLayoutFromRoot();

        void SetParent(UIElement* parent);

        void AddChild(UIElement* child);

        void RemoveChild(UIElement* child);

        const glm::ivec2& GetTopLeftScreenPos() const {
            return topLeftScreenPos;
        }

        const glm::ivec2& GetBottomRightScreenPos() const {
            return bottomRightScreenPos;
        }

        void SetTopLeftScreenPos(const glm::ivec2& pos) {
            topLeftScreenPos = pos;
        }

        void SetBottomRightScreenPos(const glm::ivec2& pos) {
            bottomRightScreenPos = pos;
        }

        const glm::ivec4& GetBorderLeftRightTopBottom() const {
            return borderLeftRightTopBottom;
        }

        const glm::ivec4& GetMarginLeftRightTopBottom() const {
            return marginLeftRightTopBottom;
        }

        void SetBorderLeftRightTopBottom(const glm::ivec4& border) {
            borderLeftRightTopBottom = border;
        }

        void SetMarginLeftRightTopBottom(const glm::ivec4& margin) {
            marginLeftRightTopBottom = margin;
        }

        const glm::ivec2& GetWidthHeight() const {
            return widthHeight;
        }

        void SetTopLeftPosRelativeToParent(const glm::ivec2& pos) {
            topLeftPosRelativeToParent = pos;
        }

        void SetContentFloating(Floating floating) {
            contentFloating = floating;
        }

        bool OnHover(const glm::ivec2& screenMousePos);

        bool OnClick(const glm::ivec2& screenMousePos);

        void ResetHoverFlag();

    protected:

        void UpdateMatrix(const glm::ivec2& parentTopLeftScPos);

        glm::ivec2& CalculateWidthHeight();

        virtual void ExecuteClickAction(const glm::ivec2& screenCursorPos) {}

        UIElement* parent = nullptr;
        std::vector<UIElement*> children;
        Geometry* quad = nullptr;
        ShaderProgram* shaderProgram = nullptr;
        bool isVisible = true;
        bool isHovered = false;
        glm::ivec2 topLeftScreenPos = glm::ivec2(400, 300);
        glm::ivec2 bottomRightScreenPos = glm::ivec2(600, 500);
        glm::ivec2 widthHeight = glm::ivec2(200, 200);
        glm::mat4 modelMatrix = glm::mat4(1.0f);

        VerticalAlignment verticalAlignment = VerticalAlignment::top;
        HorizontalAlignment horizontalAlignment = HorizontalAlignment::left;
        Floating contentFloating = Floating::topToBottom;
        glm::ivec4 borderLeftRightTopBottom = glm::ivec4(0, 0, 0, 0);
        glm::ivec4 marginLeftRightTopBottom = glm::ivec4(10, 10, 10, 10);

        glm::ivec2 topLeftPosRelativeToParent;
        bool isFixedSize = false;
    };
}