#pragma once
#include "GlobalInclude.h"
#include <string>
#include "AudioContext.h"

namespace Hogra {
	class HoGraEngineApplication
	{
	public:
		HoGraEngineApplication() = default;

		int Init(const char* windowName, int argc = 0, char* argv[] = {});

		void Loop();

		void Destroy();

	private:

		void SetFullScreenMode(GLFWwindow*& window, bool isFullScreenMode);

		void SetIcon();

		void PrintWelcomeConsoleText();

		GLFWwindow* window = nullptr;
		AudioContext audioContext;
		std::string windowName;
	};
}

