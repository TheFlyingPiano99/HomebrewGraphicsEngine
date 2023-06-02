#pragma once
#include <iostream>
#include <chrono>
#include <type_traits>
#include "AssetFolderPathManager.h"
#include "Texture2D.h"
#include "Camera.h"
#include<glm/glm.hpp>

#define P_NONE 0
#define P_WARNING_AND_ERROR 1
#define P_ERROR_ONLY 2
#define P_ALL 3

// Set this defined value to select Print-priority:
#define PRINT_PRIORITY P_ALL


namespace Hogra {

	inline std::ostream& operator<<(std::ostream& stream, const glm::vec2& v) {
		return stream << "(" << v.x << ", " << v.y << ")";
	}

	inline std::ostream& operator<<(std::ostream& stream, const glm::vec3& v) {
		return stream << "(" << v.x << ", " << v.y << ", " << v.z << ")";
	}

	inline std::ostream& operator<<(std::ostream& stream, const glm::vec4& v) {
		return stream << "(" << v.x << ", " << v.y << ", " << v.z << ", " << v.w << ")";
	}

	inline std::ostream& operator<<(std::ostream& stream, const glm::mat2& m) {
		return stream << "(" << m[0] << ", " << m[1] << ")";
	}

	inline std::ostream& operator<<(std::ostream& stream, const glm::mat3& m) {
		return stream << "(" << m[0] << ", " << m[1] << ", " << m[2] << ")";
	}

	inline std::ostream& operator<<(std::ostream& stream, const glm::mat4& m) {
		return stream << "(" << m[0] << ", " << m[1] << ", " << m[2] << ", " << m[3] << ")";
	}

	inline std::ostream& operator<<(std::ostream& stream, const glm::ivec2& v) {
		return stream << "(" << v.x << ", " << v.y << ")";
	}

	inline std::ostream& operator<<(std::ostream& stream, const glm::ivec3& v) {
		return stream << "(" << v.x << ", " << v.y << ", " << v.z << ")";
	}

	inline std::ostream& operator<<(std::ostream& stream, const glm::ivec4& v) {
		return stream << "(" << v.x << ", " << v.y << ", " << v.z << ", " << v.w << ")";
	}

	inline std::ostream& operator<<(std::ostream& stream, const glm::uvec2& v) {
		return stream << "(" << v.x << ", " << v.y << ")";
	}

	inline std::ostream& operator<<(std::ostream& stream, const glm::uvec3& v) {
		return stream << "(" << v.x << ", " << v.y << ", " << v.z << ")";
	}

	inline std::ostream& operator<<(std::ostream& stream, const glm::uvec4& v) {
		return stream << "(" << v.x << ", " << v.y << ", " << v.z << ", " << v.w << ")";
	}

	namespace DebugUtils {

		constexpr bool logToFile = false;

#if PRINT_PRIORITY >= P_WARNING_AND_ERROR
		inline void PrintWarning(const std::string& context, const std::string& message)
		{
			const auto now = std::chrono::system_clock::now();
			std::cerr << "WARNING: { Context: " << context << ", Time: " << now << ", Msg: " << message << " }" << std::endl;
			if (logToFile) {

			}
		}
#else
		inline void PrintWarning(const std::string& context, const std::string& message) { /* Empty implementation */ }
#endif

#if PRINT_PRIORITY >= P_ERROR_ONLY
		inline void PrintError(const std::string& context, const std::string& message)
		{
			const auto now = std::chrono::system_clock::now();
			std::cerr << "ERROR: { Context: " << context << ", Time: " << now << ", Msg: " << message << " }" << std::endl;
			if (logToFile) {

			}
		}
#else
		inline void PrintError(const std::string& context, const std::string& message) { /* Empty implementation */ }
#endif


#if PRINT_PRIORITY == P_ALL
		inline void PrintMsg(const std::string& context, const std::string& message)
		{
			const auto now = std::chrono::system_clock::now();
			std::cout << "MESSAGE: { Context: " << context << ", Time: " << now << ", Msg: " << message << " }" << std::endl;
			if (logToFile) {

			}
		}


		inline void PrintTextureInfo(const std::string& context, const Texture2D& texture)
		{
			const auto now = std::chrono::system_clock::now();
			std::cout << "Info: { Context: " << context << ", Time: " << now << ", TextureInfo: { ID: " << texture.glID << ", Unit: " << texture.unit << ", W: " << texture.GetDimensions().x << ", H: " << texture.GetDimensions().y << " }" << std::endl;
			if (logToFile) {

			}
		}

		inline void PrintCameraInfo(const std::string& context, const Camera& cam)
		{
			const auto now = std::chrono::system_clock::now();
			std::cout << "Info: { Context: " << context << ", Time: " << now << ", CameraInfo: { AspectRatio: " << cam.GetAspectRatio() << ", FOV: " << cam.GetFOV() << ", EyePos: " << cam.GetPosition() << ", LookDir: " << cam.GetLookDir() << ", PrefUp: " << cam.getPreferedUp() << " }" << std::endl;
			if (logToFile) {

			}
		}
#else
		inline void PrintMsg(const std::string& context, const std::string& message) { /* Empty implementation */ }
		inline void PrintTextureInfo(const std::string& context, const Texture2D& texture) { /* Empty implementation */ }
		inline void PrintCameraInfo(const std::string& context, const Camera& cam) { /* Empty implementation */ }
#endif
	}
}


