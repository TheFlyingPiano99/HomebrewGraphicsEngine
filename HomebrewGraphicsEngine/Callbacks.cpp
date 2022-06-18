#include "Callbacks.h"
#include "Scene.h"
#include "ControlActionManager.h"
#include "SingletonManager.h"
#include "GlobalInclude.h"
#include "GUI.h"
#include <iostream>

namespace hograengine {

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
		ControlActionManager::getInstance()->registerDefault();

		Scene::init(width, height);
		GUI::getInstance()->initGUI(window);
	}


	void Callbacks::onWindowRefresh(GLFWwindow* window)
	{
		GUI::getInstance()->preDrawInit();

		Scene::getInstance()->draw();

		GUI::getInstance()->configToScene(*Scene::getInstance());
		GUI::getInstance()->draw();

		glfwSwapBuffers(window);
	}


	void Callbacks::onKey(GLFWwindow* window, int key, int scancode, int action, int mods)
	{
		if (action == GLFW_PRESS || action == GLFW_REPEAT) {
			ControlActionManager::getInstance()->onPress(key, scancode, mods);
		}
		else if (action == GLFW_RELEASE) {
			ControlActionManager::getInstance()->onRelease(key, scancode, mods);
		}
	}

	void Callbacks::onMouseMove(GLFWwindow* window, double xpos, double ypos)
	{

		// Handles mouse inputs
		if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_PRESS)
		{

			// Hides mouse cursor
			glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);

			// Prevents camera from jumping on the first click
			if (firstClick)
			{
				glfwSetCursorPos(window, (GlobalVariables::windowWidth / 2), (GlobalVariables::windowHeight / 2));
				firstClick = false;
			}
			// Stores the coordinates of the cursor
			double mouseX;
			double mouseY;
			// Fetches the coordinates of the cursor
			glfwGetCursorPos(window, &mouseX, &mouseY);

			float ndcDeltaX = (float)mouseX / (float)GlobalVariables::windowWidth * 2.0f - 1.0f;
			float ndcDeltaY = (float)mouseY / (float)GlobalVariables::windowHeight * 2.0f - 1.0f;
			auto* avatarControl = Scene::getInstance()->getAvatarControl();
			if (avatarControl != nullptr) {
				avatarControl->rotate(-ndcDeltaX, -ndcDeltaY);
			}

			// Sets mouse cursor to the middle of the screen so that it doesn't end up roaming around
			glfwSetCursorPos(window, (GlobalVariables::windowWidth / 2), (GlobalVariables::windowHeight / 2));
		}
		else if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_RELEASE)
		{
			// Unhides cursor since camera is not looking around anymore
			glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
			// Makes sure the next time the camera looks around it doesn't jump
			firstClick = true;
		}
	}

	void Callbacks::onMouseScroll(GLFWwindow* window, double xoffset, double yoffset)
	{
		Scene::getInstance()->getCamera()->moveForward(yoffset * 10.0f);
	}

	void Callbacks::onMouseClick(GLFWwindow* window, int button, int action, int mods)
	{
		if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS)
		{
			double xpos, ypos;
			glfwGetCursorPos(window, &xpos, &ypos);
			glm::vec2 ndcCoords;
			ndcCoords.x = (float)xpos / (float)GlobalVariables::windowWidth * 2.0f - 1.0f;
			ndcCoords.y = 1.0f - (float)ypos / (float)GlobalVariables::windowHeight * 2.0f;
			Scene::getInstance()->pokeObject(ndcCoords);
		}
	}

	void Callbacks::onWindowSize(GLFWwindow* window, int width, int height)
	{
		GlobalVariables::windowWidth = width;
		GlobalVariables::windowHeight = height;
		glViewport(0, 0, width, height);
		Scene::getInstance()->onContextResize(width, height);
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

		Singleton::destroyAllInstances();
	}
}
