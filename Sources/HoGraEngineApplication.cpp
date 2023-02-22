#include "HoGraEngineApplication.h"

#include "GlobalInclude.h"
#include "Callbacks.h"
#include <iostream>
#include "GUI.h"
#include "ControlActionManager.h"
#include "SceneManager.h"
#include "Material.h"
#include <Windows.h>
#include <stb/stb_image.h>
#include "AssetFolderPathManager.h"
#include "HograTime.h"
#include "DebugUtils.h"

#define HOGRA_ENGINE_LICENSE_YEAR 2023

namespace Hogra {
	void HoGraEngineApplication::SetFullScreenMode(GLFWwindow*& window, bool isFullScreenMode) {
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

	void HoGraEngineApplication::SetIcon()
	{
		GLFWimage icons[1];
		// Flips the image so it appears right side up
		stbi_set_flip_vertically_on_load(false);
		// Reads the image from a file and stores it in bytes
		int channels = 0;
		auto path = AssetFolderPathManager::getInstance()->getIconsFolderPath().append(GlobalVariables::windowIcon);
		icons[0].pixels = stbi_load(
			path.c_str(),
			&(icons[0].width),
			&(icons[0].height),
			&channels,
			4);
		glfwSetWindowIcon(window, 1, icons);
	}

	void HoGraEngineApplication::PrintWelcomeConsoleText()
	{
		std::cout << " Powered by" << std::endl;
		std::cout << "  _   _                          " << std::endl;
		std::cout << " | | | | ___   __ _ _ __ __ _    " << std::endl;
		std::cout << " | |_| |/ _ \\ / _` | '__/ _` |   " << std::endl;
		std::cout << " |  _  | (_) | (_| | | | (_| |   " << std::endl;
		std::cout << " |_|_|_|\\___/ \\__, |_|  \\__,_|   " << std::endl;
		std::cout << " | ____|_ __  |___/(_)_ __   ___ " << std::endl;
		std::cout << " |  _| | '_ \\ / _` | | '_ \\ / _ \\" << std::endl;
		std::cout << " | |___| | | | (_| | | | | |  __/" << std::endl;
		std::cout << " |_____|_| |_|\\__, |_|_| |_|\\___|" << std::endl;
		std::cout << "              |___/              " << std::endl;
		std::cout << "                                  Zoltan Simon (" << HOGRA_ENGINE_LICENSE_YEAR << ")" << std::endl;
		std::cout << "_____________________________________________________" << std::endl;
		std::cout << std::endl;
	}

	int HoGraEngineApplication::Init(const char* _windowName, int argc, char* argv[]) {

		auto consoleHandle = GetConsoleWindow();
		if (GlobalVariables::hideConsoleWindow) {
			// Hide native console
			ShowWindow(consoleHandle, SW_HIDE);
		}
		else {
			ShowWindow(consoleHandle, SW_SHOW);
			PrintWelcomeConsoleText();
		}

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
			DebugUtils::PrintError("HoGraEngineApplication", "Failed to create GLFW window!");
			glfwTerminate();
			return -1;
		}
		GlobalVariables::window = window;

		//Set window icon
		SetIcon();

		// Init audio
		audioContext.Init();
		audioContext.MakeCurrent();

		// Introduce the window into the current context
		glfwMakeContextCurrent(window);

		SetFullScreenMode(window, GlobalVariables::fullScreenMode);

		Callbacks::SetCallbacks(window);

		//Load GLAD so it configures OpenGL
		gladLoadGL();

		glfwGetWindowSize(window, &GlobalVariables::windowWidth, &GlobalVariables::windowHeight);
		glfwSetInputMode(window, GLFW_STICKY_KEYS, GLFW_TRUE);

		SceneManager::getInstance()->Init(GlobalVariables::windowWidth, GlobalVariables::windowHeight);
		GUI::getInstance()->InitGUI(window);

		return 0;
	}
	
	void HoGraEngineApplication::Loop() {
		// Variables to create periodic event for FPS displaying
		double prevTime = 0.0;
		double crntTime = 0.0;
		double timeDiff;
		double prevIterTime = 0.0;
		// Keeps track of the amount of frames in timeDiff
		unsigned int frameCounter = 0;

		constexpr double dtLimit = 0.004;

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
				// Creates title
				std::string FPS = std::to_string((1.0 / timeDiff) * frameCounter);
				std::string ms = std::to_string((timeDiff / frameCounter) * 1000.0);
				std::string newTitle = windowName + " (" + FPS + " FPS / " + ms + " ms)";
				glfwSetWindowTitle(window, newTitle.c_str());

				// Resets times and counter
				prevTime = crntTime;
				frameCounter = 0;
			}
			
			ControlActionManager::getInstance()->QueueTriggeringActions();

			// Special controll events triggering per frame
			SceneManager::getInstance()->BeforePhysicsLoopUpdate();

			double dt = 0.0;
			double realDelta = crntTime - prevIterTime;
			int retVal = 0;
			while (realDelta > 0.0) {
				if (realDelta > dtLimit) {
					dt = dtLimit;
					realDelta -= dtLimit;
				}
				else {
					dt = realDelta;
					realDelta = 0.0;
				}
				Time::dt = (float)dt;
				Time::totalTime += dt;
				retVal = SceneManager::getInstance()->PhysicsUpdate(dt);
				if (-1 == retVal) {
					break;
				}
			}

			// Special controll events triggering per frame
			SceneManager::getInstance()->AfterPhysicsLoopUpdate();

			if (-1 == retVal) {
				break;
			}
			prevIterTime = crntTime;

			Callbacks::OnWindowRefresh(window);

			// Take care of all GLFW events
			glfwPollEvents();
		}
	}
	
	void HoGraEngineApplication::Destroy() {
		GUI::getInstance()->DestroyGUI();
		SceneManager::getInstance()->UnloadCurrentScene();

		// Delete window before ending the program
		glfwDestroyWindow(window);
		// Terminate GLFW before ending the program
		glfwTerminate();

		Allocator::DeleteAll();
		auto undeletedHeapInstances = Allocator::GetAllocationCount();
		if (0 < undeletedHeapInstances) {
			std::cout << "WARNING: " << undeletedHeapInstances << " dynamically allocated instances are not deallocated!" << std::endl;
		}
	}
}