#include "UIElement.h"
#include "glm/gtx/transform.hpp"
#include "../DebugUtils.h"
#include "../SceneEventManager.h"

namespace Hogra {

	bool Hogra::UIElement::IsVisible() const {
		return isVisible;
	}

	void Hogra::UIElement::SetIsVisible(bool b) {
		isVisible = b;
	}

	void UIElement::CalculateLayoutFromRoot()
	{
		// Recursion (Calculate children position and sizes):
		CalculateWidthHeight();

		// For the whole window:
		switch (horizontalAlignment)
		{
		case Hogra::UIElement::HorizontalAlignment::centered:
			topLeftPosRelativeToParent.x = (GlobalVariables::windowWidth - widthHeight.x) * 0.5f;
			break;
		case Hogra::UIElement::HorizontalAlignment::left:
			topLeftPosRelativeToParent.x = marginLeftRightTopBottom.x;
			break;
		case Hogra::UIElement::HorizontalAlignment::right:
			topLeftPosRelativeToParent.x = GlobalVariables::windowWidth - widthHeight.x - marginLeftRightTopBottom.y;
			break;
		default:
			break;
		}
		switch (verticalAlignment)
		{
		case Hogra::UIElement::VerticalAlignment::centered:
			topLeftPosRelativeToParent.y = (GlobalVariables::windowHeight - widthHeight.y) * 0.5f;
			break;
		case Hogra::UIElement::VerticalAlignment::top:
			topLeftPosRelativeToParent.y = marginLeftRightTopBottom.z;
			break;
		case Hogra::UIElement::VerticalAlignment::bottom:
			topLeftPosRelativeToParent.y = GlobalVariables::windowHeight - widthHeight.y - marginLeftRightTopBottom.w;
			break;
		default:
			break;
		}

		UpdateMatrix({0, 0});
	}

	void UIElement::UpdateMatrix(const glm::ivec2& parentTopLeftScPos)
	{
		// Calculate absolute position:
		topLeftScreenPos = parentTopLeftScPos + topLeftPosRelativeToParent;
		bottomRightScreenPos = parentTopLeftScPos + topLeftPosRelativeToParent + widthHeight;

		// Matrix:
		glm::vec3 pos = { glm::vec2(topLeftScreenPos) + glm::vec2(widthHeight) * 0.5f, 0.0 };
		pos.y = GlobalVariables::windowHeight - pos.y;
		glm::vec3 scale = { glm::vec2(widthHeight) * 0.5f, 1.0f };
		modelMatrix = /* rotation * */ glm::translate(pos) * glm::scale(scale);

		// Recursion:
		for (auto child : children) {
			child->UpdateMatrix(topLeftScreenPos);
		}
	}

	glm::ivec2& UIElement::CalculateWidthHeight()
	{
		if (isFixedSize) {
			for (auto child : children) {
				child->CalculateWidthHeight();
			}
			return widthHeight;
		}

		int maxWidth = 0;
		int maxHeight = 0;

		// Recursion:
		for (auto child : children) {
			auto wh = child->CalculateWidthHeight();
			auto childMargin = child->GetMarginLeftRightTopBottom();
			float w = wh.x + std::max(borderLeftRightTopBottom.x, childMargin.x) + std::max(borderLeftRightTopBottom.y, childMargin.y);
			if (maxWidth < w) {
				maxWidth = w;
			}
			float h = wh.y + std::max(borderLeftRightTopBottom.z, childMargin.z) + std::max(borderLeftRightTopBottom.w, childMargin.w);
			if (maxHeight < h) {
				maxHeight = h;
			}
		}

		for (int i = 0; i < children.size(); i++) {
			switch (contentFloating)
			{

			case Hogra::UIElement::Floating::leftToRight:
			{
				widthHeight.y = maxHeight;
				int w = 0;
				if (0 == i) {
					w += std::max(children[i]->GetMarginLeftRightTopBottom().x, borderLeftRightTopBottom.x);
				}
				auto childWH = children[i]->GetWidthHeight();
				switch (children[i]->verticalAlignment)
				{
				case VerticalAlignment::centered:
				{
					children[i]->SetTopLeftPosRelativeToParent({ w, (maxHeight - childWH.y) * 0.5f });
					break;
				}
				case VerticalAlignment::top:
				{
					children[i]->SetTopLeftPosRelativeToParent(
						{ w,  std::max(children[i]->GetMarginLeftRightTopBottom().z, borderLeftRightTopBottom.z) }
					);
					break;
				}
				case VerticalAlignment::bottom:
				{
					children[i]->SetTopLeftPosRelativeToParent(
						{ w, maxHeight - childWH.y - std::max(children[i]->GetMarginLeftRightTopBottom().w, borderLeftRightTopBottom.w) }
					);
					break;
				}
				default:
					break;
				}

				w += childWH.x;
				if (children.size() - 1 == i) {
					w += std::max(children[i]->GetMarginLeftRightTopBottom().y, borderLeftRightTopBottom.y);
				}
				if (children.size() - 1 > i) {
					w += std::max(children[i]->GetMarginLeftRightTopBottom().y, children[i + 1]->GetMarginLeftRightTopBottom().x);
				}
				widthHeight.x = w;
				break;
			}
			case Hogra::UIElement::Floating::topToBottom:
			{
				widthHeight.x = maxWidth;
				int h = 0;
				for (int i = 0; i < children.size(); i++) {
					if (0 == i) {
						h += std::max(children[i]->GetMarginLeftRightTopBottom().z, borderLeftRightTopBottom.z);
					}
					auto childWH = children[i]->GetWidthHeight();
					switch (children[i]->horizontalAlignment)
					{
					case HorizontalAlignment::centered:
					{
						children[i]->SetTopLeftPosRelativeToParent({ (maxWidth - childWH.x) * 0.5f, h});
						break;
					}
					case HorizontalAlignment::left:
					{
						children[i]->SetTopLeftPosRelativeToParent({ std::max(children[i]->GetMarginLeftRightTopBottom().x, borderLeftRightTopBottom.x), h});
						break;
					}
					case HorizontalAlignment::right:
					{
						children[i]->SetTopLeftPosRelativeToParent(
							{ maxWidth - childWH.x - std::max(children[i]->GetMarginLeftRightTopBottom().y, borderLeftRightTopBottom.y), h}
						);
						break;
					}
					default:
						break;
					}

					h += childWH.y;
					if (children.size() - 1 == i) {
						h += std::max(children[i]->GetMarginLeftRightTopBottom().w, borderLeftRightTopBottom.w);
					}
					if (children.size() - 1 > i) {
						h += std::max(children[i]->GetMarginLeftRightTopBottom().w, children[i + 1]->GetMarginLeftRightTopBottom().z);
					}
				}
				widthHeight.y = h;
				break;
			}
			default:
				break;
			}
		}
		return widthHeight;
	}

	void UIElement::SetParent(UIElement* _parent)
	{
		parent = _parent;
	}

	void UIElement::AddChild(UIElement* child)
	{
		if (auto iter = std::find(children.begin(), children.end(), child); children.end() == iter) {
			child->SetParent(this);
			children.push_back(child);
		}
	}

	void UIElement::RemoveChild(UIElement* child)
	{
		if (auto iter = std::find(children.begin(), children.end(), child); children.end() != iter) {
			child->SetParent(nullptr);
			children.erase(iter);
		}
	}

	bool UIElement::OnHover(const glm::ivec2& screenMousePos)
	{
		if (
			screenMousePos.x >= topLeftScreenPos.x
			&& screenMousePos.x <= bottomRightScreenPos.x
			&& screenMousePos.y >= topLeftScreenPos.y
			&& screenMousePos.y <= bottomRightScreenPos.y
			) {
			// Cursor is inside element
			isHovered = true;
			if (!isVisible) {
				return true;
			}
			for (auto child : children) {
				if (child->OnHover(screenMousePos)) {
					break;
				}
			}
			return true;
		}
		return false;
	}

	bool UIElement::OnClick(const glm::ivec2& screenMousePos)
	{
		if (
			screenMousePos.x >= topLeftScreenPos.x
			&& screenMousePos.x <= bottomRightScreenPos.x
			&& screenMousePos.y >= topLeftScreenPos.y
			&& screenMousePos.y <= bottomRightScreenPos.y
			) {
			// Cursor is inside element
			if (!isVisible) {
				return true;
			}
			ExecuteClickAction(screenMousePos);
			for (auto child : children) {
				if (child->OnClick(screenMousePos)) {
					break;
				}
			}
			return true;
		}
		return false;
	}

	void UIElement::ResetHoverFlag()
	{
		isHovered = false;
		for (auto child : children) {
			child->ResetHoverFlag();
		}
	}

}