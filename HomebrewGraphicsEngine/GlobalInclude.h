#pragma once
#include <glad/glad.h>
#include <GLFW/glfw3.h>

#define DEFAULT_WINDOW_WIDTH 1024
#define DEFAULT_WINDOW_HEIGHT 768

#define SHADOW_MAP_WIDTH 4096
#define SHADOW_MAP_HEIGHT 4096
#define SHADOW_MAP_UNIT 4
#define SKYBOX_UNIT 5



#define FBO_WIDTH 1920
#define FBO_HEIGHT 1080

/* pi */
# define M_PI 3.14159265358979323846

#define MAX_LIGHT_COUNT 64

namespace hograengine {
	static struct GlobalVariables {
		static GLFWwindow* window;
		static int windowWidth;
		static int windowHeight;
		static bool fullScreenMode;
	};
}