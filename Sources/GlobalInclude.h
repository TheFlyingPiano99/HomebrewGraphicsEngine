#pragma once
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <string>


#define DEFAULT_WINDOW_WIDTH 1280
#define DEFAULT_WINDOW_HEIGHT 720

#define DEPTH_MAP_UNIT 1

#define POSITION_MAP_UNIT 0
#define ALBEDO_MAP_UNIT 1
#define NORMAL_MAP_UNIT 2
#define ROUGHNESS_METALLIC_AO_MAP_UNIT 3

#define ENVIRONMENT_MAP_UNIT 5
#define IRRADIANCE_MAP_UNIT 6
#define PREFILTER_MAP_UNIT 7
#define BRDF_LUT_UNIT 8
#define SHADOW_MAP_UNIT 14

#define SHADOW_MAP_WIDTH 4096
#define SHADOW_MAP_HEIGHT 4096

#define CAMERA_UBO_BINDING 0
#define LIGHTS_UBO_BINDING 1
#define SHADOWCASTER_UBO_BINDING 2
#define CONTEXT_UBO_BINDING 3


#define INSTANCED_MODEL_MATRIX_LOCATION 5
#define INSTANCED_INV_MODEL_MATRIX_LOCATION 9

#define INSTANCED_LIGHT_LOCATION 9
#define INSTANCED_LIGHT_POWER_DENSITY_LOCATION 10
#define INSTANCED_LIGHT_SHADOW_MAP_IDX_LOCATION 11	// -1 if no mapping

#define DIR_SHADOW_MAP_NEAR_PLANE 1.0f
#define DIR_SHADOW_MAP_FAR_PLANE 100.0f

#define OMNI_DIR_SHADOW_MAP_NEAR_PLANE 0.1f
#define OMNI_DIR_SHADOW_MAP_FAR_PLANE 100.0f


/* pi */
# define M_PI 3.14159265358979323846

#define MAX_LIGHT_COUNT 2048
#define MAX_SHADOW_MAP_COUNT 16

namespace Hogra {
    struct GlobalVariables {
        static GLFWwindow* window;
        static int windowWidth;
        static int windowHeight;
        static bool fullScreenMode;
        static bool hideConsoleWindow;
        static bool hideCursor;

        static std::string windowIcon;
    };
}