#pragma once
#include "GlobalInclude.h"
#include <string>

namespace hograengine {
	class HoGraEngineApplication
	{
	public:
		HoGraEngineApplication() = default;

		int init(const char* windowName);

		void loop();

		void destroy();

	private:

		void setFullScreenMode(GLFWwindow*& window, bool isFullScreenMode);

		GLFWwindow* window = nullptr;
		std::string windowName;
	};
}

