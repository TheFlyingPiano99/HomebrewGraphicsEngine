#include "Callbacks.h"
#include "ControlActionManager.h"
#include "MemoryManager.h"
#include "GlobalInclude.h"
#include "GUI.h"
#include "SceneManager.h"
#include <iostream>

namespace Hogra {

	// Prevents the camera from jumping around when first clicking left click
	bool firstClick = true;


	void Callbacks::setCallbacks(GLFWwindow* window) {
		glfwSetWindowRefreshCallback(window, Callbacks::onWindowRefresh);
		glfwSetWindowCloseCallback(window, Callbacks::onWindowClose);
		glfwSetKeyCallback(window, Callbacks::onKey);
		glfwSetCursorPosCallback(window, Callbacks::onMouseMove);
		glfwSetScrollCallback(window, Callbacks::onMouseScroll);
		glfwSetMouseButtonCallback(window, Callbacks::onMouseClick);
		glfwSetWindowSizeCallback(window, Callbacks::onWindowSize);
	}
	
	void Callbacks::onWindowInit(GLFWwindow* window)
	{
		int width, height;
		glfwGetWindowSize(window, &width, &height);
		glViewport(0, 0, width, height);
		glfwSetInputMode(window, GLFW_STICKY_KEYS, GLFW_TRUE);

		// Set control layout
		ControlActionManager::getInstance()->RegisterDefaultForVoxelObserving();

		SceneManager::getInstance()->Init(GlobalVariables::renderResolutionWidth, GlobalVariables::renderResolutionHeight);
		GUI::getInstance()->InitGUI(window);
	}


	void Callbacks::onWindowRefresh(GLFWwindow* window)
	{
		GUI::getInstance()->preDrawInit();

		SceneManager::getInstance()->Draw();

		GUI::getInstance()->Draw();

		glfwSwapBuffers(window);
	}


	void Callbacks::onKey(GLFWwindow* window, int key, int scancode, int action, int mods)
	{
		if (action == GLFW_PRESS || action == GLFW_REPEAT) {
			ControlActionManager::getInstance()->OnPress(key, scancode, mods);
		}
		else if (action == GLFW_RELEASE) {
			ControlActionManager::getInstance()->OnRelease(key, scancode, mods);
		}
	}

	void Callbacks::onMouseMove(GLFWwindow* window, double xpos, double ypos)
	{
		static bool prevCentered = false;
		bool isFirst = false;
		// Check if this is the first call with centered camera:
		if (GlobalVariables::hideCursor && !prevCentered) {
			isFirst = true;
			glfwSetCursorPos(window, (GlobalVariables::windowWidth / 2), (GlobalVariables::windowHeight / 2));
		}
		if (GlobalVariables::hideCursor) {
			// Sets mouse cursor to the middle of the screen so that it doesn't end up roaming around
			glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
			prevCentered = true;
		}
		else {
			glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
			prevCentered = false;
		}
		double pixMouseX;
		double pixMouseY;
		// Fetches the coordinates of the cursor
		glfwGetCursorPos(window, &pixMouseX, &pixMouseY);
		ControlActionManager::getInstance()->OnMouseMove(glm::vec2(pixMouseX, pixMouseY), isFirst);
	}

	void Callbacks::onMouseScroll(GLFWwindow* window, double xoffset, double yoffset)
	{
		ControlActionManager::getInstance()->OnMouseScroll((float)yoffset);
	}

	void Callbacks::onMouseClick(GLFWwindow* window, int button, int action, int mods)
	{
		double xpos, ypos;
		glfwGetCursorPos(window, &xpos, &ypos);
		glm::vec2 pixPos(xpos, ypos);
		if (button == GLFW_MOUSE_BUTTON_LEFT)
		{
			if (GLFW_PRESS == action) {
				ControlActionManager::getInstance()->OnMouseLeftButtonPress(pixPos);
			}
			else if (GLFW_RELEASE == action) {
				ControlActionManager::getInstance()->OnMouseLeftButtonRelease(pixPos);
			}
		}
		if (button == GLFW_MOUSE_BUTTON_RIGHT)
		{
			if (GLFW_PRESS == action) {
				ControlActionManager::getInstance()->OnMouseRightButtonPress(pixPos);
			}
			else if (GLFW_RELEASE == action) {
				ControlActionManager::getInstance()->OnMouseRightButtonRelease(pixPos);
			}
		}
	}

	void Callbacks::onWindowSize(GLFWwindow* window, int width, int height)
	{
		GlobalVariables::windowWidth = width;
		GlobalVariables::windowHeight = height;
		glViewport(0, 0, width, height);
	}

	void Callbacks::toggleFullScreen()
	{
		GlobalVariables::fullScreenMode = !GlobalVariables::fullScreenMode;
		if (GlobalVariables::fullScreenMode) {
			GLFWmonitor* monitor = glfwGetPrimaryMonitor();
			const GLFWvidmode* mode = glfwGetVideoMode(monitor);
			glfwSetWindowMonitor(GlobalVariables::window, monitor, 0, 0, mode->width, mode->height, 60);
		}
		else {
			glfwSetWindowMonitor(GlobalVariables::window, nullptr, 50, 50, DEFAULT_WINDOW_WIDTH, DEFAULT_WINDOW_HEIGHT, 60);
		}
		glfwGetWindowSize(GlobalVariables::window, &GlobalVariables::windowWidth, &GlobalVariables::windowHeight);
		glViewport(0, 0, GlobalVariables::windowWidth, GlobalVariables::windowHeight);
		std::cout << "Screen size: " << GlobalVariables::windowWidth << ", " << GlobalVariables::windowHeight << std::endl;
	}



	void Callbacks::onWindowClose(GLFWwindow* window)
	{
		// Delete all the objects we've created


	}
}
