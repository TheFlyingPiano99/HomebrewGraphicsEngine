#pragma once
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <string>


#define DEFAULT_WINDOW_WIDTH 1280
#define DEFAULT_WINDOW_HEIGHT 720

#define ALBEDO_MAP_UNIT 0
#define NORMAL_MAP_UNIT 1
#define ROUGHNESS_METALLIC_AO_MAP_UNIT 2
// (ROUGHNESS, METALLIC, AO)
#define SKYBOX_UNIT 5
#define SHADOW_MAP_UNIT 6

#define SHADOW_MAP_WIDTH 4096
#define SHADOW_MAP_HEIGHT 4096

#define CAMERA_UBO_BINDING 0
#define LIGHTS_UBO_BINDING 1
#define SHADOWCASTER_UBO_BINDING 2

#define INSTANCED_MODEL_MATRIX_LOCATION 5
#define INSTANCED_INV_MODEL_MATRIX_LOCATION 9

#define INSTANCED_LIGHT_LOCATION 9
#define INSTANCED_LIGHT_POWER_DENSITY_LOCATION 10

/* pi */
# define M_PI 3.14159265358979323846

#define MAX_LIGHT_COUNT 2048

namespace Hogra {
	struct GlobalVariables {
		static GLFWwindow* window;
		static int windowWidth;
		static int windowHeight;
		static bool fullScreenMode;
		static bool hideConsoleWindow;
		static bool hideCursor;

		static int renderResolutionWidth;
		static int renderResolutionHeight;

		static std::string windowIcon;
	};
}