/*
* ~~~~~~~~~~~~~~~~~~~~~~~~~
* Homebrew Graphics Engine
* Zoltan Simon 2023
* Repository: https://github.com/TheFlyingPiano99/HomebrewGraphicsEngine
* ~~~~~~~~~~~~~~~~~~~~~~~~~
*/

#pragma once
#include <iostream>
#include <fstream>
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

        inline constexpr bool logToFile = true;

#if PRINT_PRIORITY >= P_WARNING_AND_ERROR
        inline void PrintWarning(const std::string& context, const std::string& message)
        {
            const auto now = std::chrono::system_clock::now();
            std::cerr << "WARNING: {\n\tContext: " << context << "\n\tTime: " << now << "\n\tMsg: " << message << "\n}" << std::endl;
            if (logToFile) {
                std::ofstream stream(AssetFolderPathManager::getInstance()->getSavesFolderPath().append("log.txt"), std::ios_base::app);
                if (stream.is_open()) {
                    stream << "WARNING: {\n\tContext: " << context << "\n\tTime: " << now << "\n\tMsg: " << message << "\n}" << std::endl;
                    stream.close();
                }
            }
        }
#else
        inline void PrintWarning(const std::string& context, const std::string& message) { /* Empty implementation */ }
#endif

#if PRINT_PRIORITY >= P_ERROR_ONLY
        inline void PrintError(const std::string& context, const std::string& message)
        {
            const auto now = std::chrono::system_clock::now();
            std::cerr << "ERROR: {\n\tContext: " << context << "\n\tTime: " << now << "\n\tMsg: " << message << "\n}" << std::endl;
            if (logToFile) {
                std::ofstream stream(AssetFolderPathManager::getInstance()->getSavesFolderPath().append("log.txt"), std::ios_base::app);
                if (stream.is_open()) {
                    stream << "ERROR: {\n\tContext: " << context << "\n\tTime: " << now << "\n\tMsg: " << message << "\n}" << std::endl;
                    stream.close();
                }
            }
        }
#else
        inline void PrintError(const std::string& context, const std::string& message) { /* Empty implementation */ }
#endif


#if PRINT_PRIORITY == P_ALL
        inline void PrintMsg(const std::string& context, const std::string& message)
        {
            const auto now = std::chrono::system_clock::now();
            std::cout << "MESSAGE: {\n\tContext: " << context << "\n\tTime: " << now << "\n\tMsg: " << message << "\n}" << std::endl;
            if (logToFile) {
                std::ofstream stream(AssetFolderPathManager::getInstance()->getSavesFolderPath().append("log.txt"), std::ios_base::app);
                if (stream.is_open()) {
                    stream << "MESSAGE: {\n\tContext: " << context << "\n\tTime: " << now << "\n\tMsg: " << message << "\n}" << std::endl;
                    stream.close();
                }
            }
        }


        inline void PrintTextureInfo(const std::string& context, const Texture2D& texture)
        {
            const auto now = std::chrono::system_clock::now();
            std::cout << "INFO: {\n\tContext: " << context << "\n\tTime: " << now << "\n\tTextureInfo: { ID: " << texture.glID << ", Unit: " << texture.unit << ", W: " << texture.GetDimensions().x << ", H: " << texture.GetDimensions().y << " }\n}" << std::endl;
            if (logToFile) {
                std::ofstream stream(AssetFolderPathManager::getInstance()->getSavesFolderPath().append("log.txt"), std::ios_base::app);
                if (stream.is_open()) {
                    stream << "INFO: {\n\tContext: " << context << "\n\tTime: " << now << "\n\tTextureInfo: { ID: " << texture.glID << ", Unit: " << texture.unit << ", W: " << texture.GetDimensions().x << ", H: " << texture.GetDimensions().y << " }\n}" << std::endl;
                    stream.close();
                }
            }
        }

        inline void PrintCameraInfo(const std::string& context, const Camera& cam)
        {
            const auto now = std::chrono::system_clock::now();
            std::cout << "INFO: {\n\tContext: " << context << "\n\tTime: " << now << "\n\tCameraInfo: { AspectRatio: " << cam.GetAspectRatio() << ", FOV: " << cam.GetFOV() << ", EyePos: " << cam.GetPosition() << ", LookDir: " << cam.GetLookDir() << ", PrefUp: " << cam.getPreferedUp() << " }\n}" << std::endl;
            if (logToFile) {
                std::ofstream stream(AssetFolderPathManager::getInstance()->getSavesFolderPath().append("log.txt"), std::ios_base::app);
                if (stream.is_open()) {
                    stream << "INFO: {\n\tContext: " << context << "\n\tTime: " << now << "\n\tCameraInfo: { AspectRatio: " << cam.GetAspectRatio() << ", FOV: " << cam.GetFOV() << ", EyePos: " << cam.GetPosition() << ", LookDir: " << cam.GetLookDir() << ", PrefUp: " << cam.getPreferedUp() << " }\n}" << std::endl;
                    stream.close();
                }
            }
        }
#else
        inline void PrintMsg(const std::string& context, const std::string& message) { /* Empty implementation */ }
        inline void PrintTextureInfo(const std::string& context, const Texture2D& texture) { /* Empty implementation */ }
        inline void PrintCameraInfo(const std::string& context, const Camera& cam) { /* Empty implementation */ }
#endif
    }
}


