#include "HoGraEngineApplication.h"

#include "GlobalInclude.h"
#include "Callbacks.h"
#include <iostream>
#include "GUI.h"
#include "ControlActionManager.h"
#include "SceneManager.h"
#include "Material.h"

namespace Hogra {
	void HoGraEngineApplication::setFullScreenMode(GLFWwindow*& window, bool isFullScreenMode) {
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

	int HoGraEngineApplication::Init(const char* _windowName) {
		windowName = _windowName;
		// Initialize GLFW
		glfwInit();

		// Tell GLFW what version of OpenGL we are using 
		// In this case we are using OpenGL 4.6
		glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
		glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
		// Tell GLFW we are using the CORE profile
		// So that means we only have the modern functions
		glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

		window = glfwCreateWindow(DEFAULT_WINDOW_WIDTH, DEFAULT_WINDOW_HEIGHT, _windowName, NULL, NULL);
		// Error check if the window fails to create
		if (nullptr == window)
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

		//Load GLAD so it configures OpenGL
		gladLoadGL();

		Callbacks::onWindowInit(window);
		return 0;
	}
	
	void HoGraEngineApplication::loop() {
		// Variables to create periodic event for FPS displaying
		double prevTime = 0.0;
		double crntTime = 0.0;
		double timeDiff;
		double prevIterTime = 0.0;
		// Keeps track of the amount of frames in timeDiff
		unsigned int frameCounter = 0;

		const double dtLimit = 0.004;

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
				std::string newTitle = windowName + " (" + FPS + " FPS / " + ms + " ms)";
				glfwSetWindowTitle(window, newTitle.c_str());

				// Resets times and counter
				prevTime = crntTime;
				frameCounter = 0;
			}

			double dt = 0.0;
			double realDelta = crntTime - prevIterTime;
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
				SceneManager::getInstance()->UpdateAndControl(dt);
			}
			prevIterTime = crntTime;

			Callbacks::onWindowRefresh(window);

			// Take care of all GLFW events
			glfwPollEvents();
		}

	}
	
	void HoGraEngineApplication::Destroy() {
		GUI::getInstance()->destroyGUI();

		// Delete window before ending the program
		glfwDestroyWindow(window);
		// Terminate GLFW before ending the program
		glfwTerminate();
		Material::PrintInstanceCount();
		Mesh::PrintInstanceCount();
		SceneObject::PrintInstanceCount();
	}
}