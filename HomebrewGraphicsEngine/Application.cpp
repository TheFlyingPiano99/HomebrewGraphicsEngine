

#include "Callbacks.h"
#include "Scene.h"
#include "GlobalInclude.h"
#include "GUI.h"
#include <iostream>
#include "ControlActionManager.h"

#define PROJECT_NAME "HomebrewGraphicsEngine"

void setFullScreenMode(GLFWwindow*& window, bool isFullScreenMode) {
	if (isFullScreenMode) {
		GLFWmonitor* monitor = glfwGetPrimaryMonitor();
		const GLFWvidmode* mode = glfwGetVideoMode(monitor);
		glfwSetWindowMonitor(window, monitor, 0, 0, mode->width, mode->height, 60);
	}
	else {
		glfwSetWindowMonitor(window, nullptr, 50, 50, DEFAULT_WINDOW_WIDTH, DEFAULT_WINDOW_HEIGHT, 60);
	}
	glfwGetWindowSize(window, &GlobalVariables::windowWidth, &GlobalVariables::windowHeight);
}


int initWindow(GLFWwindow*& window) {
	// Initialize GLFW
	glfwInit();

	// Tell GLFW what version of OpenGL we are using 
	// In this case we are using OpenGL 4.2
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
	// Tell GLFW we are using the CORE profile
	// So that means we only have the modern functions
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	window = glfwCreateWindow(DEFAULT_WINDOW_WIDTH, DEFAULT_WINDOW_HEIGHT, PROJECT_NAME, NULL, NULL);
	// Error check if the window fails to create
	if (window == NULL)
	{
		std::cout << "Failed to create GLFW window!" << std::endl;
		glfwTerminate();
		return -1;
	}
	GlobalVariables::window = window;
	// Introduce the window into the current context
	glfwMakeContextCurrent(window);

	setFullScreenMode(window, GlobalVariables::fullScreenMode);

	Callbacks::setCallbacks(window);
	return 0;
}

int main()
{
	GLFWwindow* window;
	if (-1 == initWindow(window)) {
		return -1;
	}

	//Load GLAD so it configures OpenGL
	gladLoadGL();

	Callbacks::onWindowInit(window);

	// Variables to create periodic event for FPS displaying
	double prevTime = 0.0;
	double crntTime = 0.0;
	double timeDiff;
	double prevIterTime = 0.0f;
	// Keeps track of the amount of frames in timeDiff
	unsigned int frameCounter = 0;

	const double dtLimit = 10.0;

	glfwSetInputMode(window, GLFW_STICKY_KEYS, GLFW_TRUE);

	// Main while loop
	while (!glfwWindowShouldClose(window))
	{
		// Updates counter and times
		crntTime = glfwGetTime();
		timeDiff = crntTime - prevTime;
		frameCounter++;

		if (frameCounter >= 50)
		{
			// Creates new title
			std::string FPS = std::to_string((1.0 / timeDiff) * frameCounter);
			std::string ms = std::to_string((timeDiff / frameCounter) * 1000.0);
			std::string newTitle = std::string(PROJECT_NAME) + " (" + FPS + " FPS / " + ms + " ms)";
			glfwSetWindowTitle(window, newTitle.c_str());

			// Resets times and counter
			prevTime = crntTime;
			frameCounter = 0;
		}

		double dt = 0.0;
		double realDelta = (crntTime - prevIterTime) * 1000;
		//Scene::getInstance()->getCamera()->Inputs(window);		<<-- temp solution only!!!
		while (realDelta > 0.0) {
			if (realDelta > dtLimit) {
				dt = dtLimit;
				realDelta -= dtLimit;
			}
			else {
				dt = realDelta;
				realDelta = 0.0;
			}
			ControlActionManager::getInstance()->queueTriggeringActions();
			Scene::getInstance()->control(dt);
			Scene::getInstance()->animate(dt);
		}
		prevIterTime = crntTime;

		Callbacks::onWindowRefresh(window);

		// Take care of all GLFW events
		glfwPollEvents();
	}

	GUI::getInstance()->destroyGUI();

	// Delete window before ending the program
	glfwDestroyWindow(window);
	// Terminate GLFW before ending the program
	glfwTerminate();
	return 0;
}