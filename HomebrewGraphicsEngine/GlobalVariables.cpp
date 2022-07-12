#include "GlobalInclude.h"
namespace hograengine {

	GLFWwindow* GlobalVariables::window = nullptr;
	int GlobalVariables::windowWidth = DEFAULT_WINDOW_WIDTH;
	int GlobalVariables::windowHeight = DEFAULT_WINDOW_HEIGHT;
	int GlobalVariables::renderResolutionWidth = 1920;
	int GlobalVariables::renderResolutionHeight = 1080;
	bool GlobalVariables::fullScreenMode = false;
}