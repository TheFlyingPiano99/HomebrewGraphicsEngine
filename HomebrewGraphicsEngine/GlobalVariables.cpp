#include "GlobalInclude.h"
namespace hograengine {

	GLFWwindow* GlobalVariables::window = nullptr;
	int GlobalVariables::windowWidth = DEFAULT_WINDOW_WIDTH;
	int GlobalVariables::windowHeight = DEFAULT_WINDOW_HEIGHT;
	int GlobalVariables::renderResolutionWidth = 1280;
	int GlobalVariables::renderResolutionHeight = 720;
	bool GlobalVariables::fullScreenMode = false;
}