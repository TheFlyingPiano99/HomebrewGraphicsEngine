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


	void Callbacks::SetCallbacks(GLFWwindow* window) {
		glfwSetWindowRefreshCallback(window, Callbacks::OnWindowRefresh);
		glfwSetWindowCloseCallback(window, Callbacks::OnWindowClose);
		glfwSetWindowSizeCallback(window, Callbacks::OnWindowSize);
		glfwSetKeyCallback(window, Callbacks::OnKey);
		glfwSetCursorPosCallback(window, Callbacks::OnMouseMove);
		glfwSetScrollCallback(window, Callbacks::OnMouseScroll);
		glfwSetMouseButtonCallback(window, Callbacks::OnMouseClick);
	}
	
	void Callbacks::OnWindowRefresh(GLFWwindow* window)
	{
		GUI::getInstance()->preDrawInit();

		SceneManager::getInstance()->Draw();

		GUI::getInstance()->Draw();

		glfwSwapBuffers(window);
	}


	void Callbacks::OnKey(GLFWwindow* window, int key, int scancode, int action, int mods)
	{
		if (action == GLFW_PRESS || action == GLFW_REPEAT) {
			ControlActionManager::getInstance()->OnPress(key, scancode, mods);
		}
		else if (action == GLFW_RELEASE) {
			ControlActionManager::getInstance()->OnRelease(key, scancode, mods);
		}
	}

	void Callbacks::OnMouseMove(GLFWwindow* window, double xpos, double ypos)
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

	void Callbacks::OnMouseScroll(GLFWwindow* window, double xoffset, double yoffset)
	{
		ControlActionManager::getInstance()->OnMouseScroll((float)yoffset);
	}

	void Callbacks::OnMouseClick(GLFWwindow* window, int button, int action, int mods)
	{
		double xpos, ypos;
		glfwGetCursorPos(window, &xpos, &ypos);
		glm::vec2 pixPos(xpos, ypos);
		if (button == GLFW_MOUSE_BUTTON_LEFT)
		{
			if (GLFW_PRESS == action) {
				ControlActionManager::getInstance()->OnMouseButtonPress(GLFW_MOUSE_BUTTON_LEFT, pixPos);
			}
			else if (GLFW_RELEASE == action) {
				ControlActionManager::getInstance()->OnMouseButtonRelease(GLFW_MOUSE_BUTTON_LEFT, pixPos);
			}
		}
		if (button == GLFW_MOUSE_BUTTON_RIGHT)
		{
			if (GLFW_PRESS == action) {
				ControlActionManager::getInstance()->OnMouseButtonPress(GLFW_MOUSE_BUTTON_RIGHT, pixPos);
			}
			else if (GLFW_RELEASE == action) {
				ControlActionManager::getInstance()->OnMouseButtonRelease(GLFW_MOUSE_BUTTON_RIGHT, pixPos);
			}
		}
	}

	void Callbacks::OnWindowSize(GLFWwindow* window, int width, int height)
	{
		GlobalVariables::windowWidth = width;
		GlobalVariables::windowHeight = height;
		SceneManager::getInstance()->OnWindowResize(width, height);
		std::cout << "W: " << width << " H: " << height << std::endl;
	}

	void Callbacks::ToggleFullScreen()
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

	void Callbacks::OnWindowClose(GLFWwindow* window)
	{
		// Delete all the objects we've created


	}
}
