#pragma once
#include "GlobalInclude.h"
#include <string>

namespace Hogra {
	class HoGraEngineApplication
	{
	public:
		HoGraEngineApplication() = default;

		int Init(const char* windowName);

		void loop();

		void Destroy();

	private:

		void setFullScreenMode(GLFWwindow*& window, bool isFullScreenMode);

		GLFWwindow* window = nullptr;
		std::string windowName;
	};
}

