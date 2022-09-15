#pragma once
#include "GlobalInclude.h"
#include <string>
#include "AudioContext.h"

namespace Hogra {
	class HoGraEngineApplication
	{
	public:
		HoGraEngineApplication() = default;

		int Init(const char* windowName);

		void Loop();

		void Destroy();

	private:

		void setFullScreenMode(GLFWwindow*& window, bool isFullScreenMode);

		void SetIcon();

		GLFWwindow* window = nullptr;
		AudioContext audioContext;
		std::string windowName;
	};
}

