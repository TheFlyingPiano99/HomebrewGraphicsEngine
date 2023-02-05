#include "GlobalInclude.h"
namespace Hogra {

	GLFWwindow* GlobalVariables::window = nullptr;

	int GlobalVariables::windowWidth = DEFAULT_WINDOW_WIDTH;
	int GlobalVariables::windowHeight = DEFAULT_WINDOW_HEIGHT;

	bool GlobalVariables::fullScreenMode = false;
	bool GlobalVariables::hideConsoleWindow = false;
	bool GlobalVariables::hideCursor = false;

	std::string GlobalVariables::windowIcon = "HoGraEngineLogo_small.png";
}