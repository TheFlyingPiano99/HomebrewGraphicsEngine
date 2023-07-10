#include "UIElement.h"

namespace Hogra {

	bool Hogra::UIElement::IsVisible() const {
		return isVisible;
	}

	void Hogra::UIElement::SetIsVisible(bool b) {
		isVisible = b;
	}

	void UIElement::UpdateFromRoot()
	{
		// Recursion (Calculate children position and sizes):
		for (UIElement* child : children) {
			child->CalculateWidthHeight();
		}

		UpdateMatrix({0, 0});
	}

	void UIElement::UpdateMatrix(const glm::ivec2& parentTopLeftScPos)
	{
		topLeftScreenPos = parentTopLeftScPos + topLeftPosRelativeToParent;
		bottomRightScreenPos = parentTopLeftScPos + topLeftPosRelativeToParent + widthHeight;

		// Matrix:
		glm::vec3 pos = { topLeftScreenPos + widthHeight * 0.5f, 0.0 };
		pos.y = GlobalVariables.y - pos.y;
		glm::vec3 scale = { widthHeight * 0.5f, 1.0f };		
		modelMatrix = /* rotation * */ glm::translate(pos) * glm::scale(scale);

		for (auto child : children) {
			UpdateMatrix(topLeftScreenPos);
		}
	}

	glm::ivec2& UIElement::CalculateWidthHeight()
	{
		int maxWidth = 0;
		int maxHeight = 0;

		// Recursion:
		for (auto child : children) {
			auto wh = CalculateWidthHeight();
			auto childMargin = child->GetMarginLeftRightTopBottom();
			w = wh.x + std::max(border.x, childMargin.x) + std::max(border.y, childMargin.y);
			if (maxWidth < w) {
				maxWidth = w;
			}
			h = wh.y + std::max(border.z, childMargin.z) + std::max(border.w, childMargin.w);
			if (maxHeight < h) {
				maxHeight = h;
			}
		}
		switch (floating)
		{

		case Hogra::UIElement::Floating::leftToRight:
		{
			widthHeight.y = maxHeight;
			int w = 0;
			for (int i = 0; i < children.size(); i++) {
				if (0 == i) {
					w += std::max(children[i]->GetMarginLeftRightTopBottom().x, border.x));
				}
				auto childWH = children[i]->GetWidthHeight();
				switch (horizontalAlignment)
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

				w += .x;
				if (children.size() - 1 == i) {
					w += std::max(children[i]->GetMarginLeftRightTopBottom().y, border.y) );
				}
				if (children.size() - 1 > i) {
					w += std::max(children[i]->GetMarginLeftRightTopBottom().y, children[i + 1]->GetMarginLeftRightTopBottom().x));
				}
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
					h += std::max(children[i]->GetMarginLeftRightTopBottom().z, border.z));
				}
				auto childWH = children[i]->GetWidthHeight();
				switch (horizontalAlignment)
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
					h += std::max(children[i]->GetMarginLeftRightTopBottom().w, border.w) );
				}
				if (children.size() - 1 > i) {
					h += std::max(children[i]->GetMarginLeftRightTopBottom().w, children[i + 1]->GetMarginLeftRightTopBottom().z));
				}
			}
			widthHeight.y = h;
			break;
		}
		default:
			break;
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
			children.push_back(child);
		}
	}

	void UIElement::RemoveChild(UIElement* child)
	{
		if (auto iter = std::find(children.begin(), children.end(), child); children.end() != iter) {
			children.erase(iter);
		}
	}

}