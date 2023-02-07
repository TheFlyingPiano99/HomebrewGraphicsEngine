#pragma once
#include "GlobalInclude.h"

namespace Hogra {

	namespace Callbacks
	{
		void SetCallbacks(GLFWwindow* window);

		void OnWindowRefresh(GLFWwindow* window);
		void OnKey(GLFWwindow* window, int key, int scancode, int action, int mods);
		void OnWindowClose(GLFWwindow* window);
		void OnMouseMove(GLFWwindow* window, double xpos, double ypos);
		void OnMouseScroll(GLFWwindow* window, double xoffset, double yoffset);
		void OnMouseClick(GLFWwindow* window, int button, int action, int mods);
		void OnWindowSize(GLFWwindow* window, int width, int height);

		void ToggleFullScreen();
	};

}