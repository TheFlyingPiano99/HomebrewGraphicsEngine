#include "AudioUtils.h"

namespace Hogra {
    
    bool AudioUtils::CheckALCErrors(const std::string& filename, const std::uint_fast32_t line, ALCdevice* device)
    {
        ALCenum error = alcGetError(device);
        if (error != ALC_NO_ERROR)
        {
            std::cerr << "***ERROR*** (" << filename << ": " << line << ")\n";
            switch (error)
            {
            case ALC_INVALID_VALUE:
                std::cerr << "ALC_INVALID_VALUE: an invalid value was passed to an OpenAL function";
                break;
            case ALC_INVALID_DEVICE:
                std::cerr << "ALC_INVALID_DEVICE: a bad device was passed to an OpenAL function";
                break;
            case ALC_INVALID_CONTEXT:
                std::cerr << "ALC_INVALID_CONTEXT: a bad context was passed to an OpenAL function";
                break;
            case ALC_INVALID_ENUM:
                std::cerr << "ALC_INVALID_ENUM: an unknown enum value was passed to an OpenAL function";
                break;
            case ALC_OUT_OF_MEMORY:
                std::cerr << "ALC_OUT_OF_MEMORY: an unknown enum value was passed to an OpenAL function";
                break;
            default:
                std::cerr << "UNKNOWN ALC ERROR: " << error;
            }
            std::cerr << std::endl;
            return false;
        }
        return true;
    }
    inline bool AudioUtils::CheckALErrors(const std::string& filename, const std::uint_fast32_t line)
    {
        ALenum error = alGetError();
        if (error != AL_NO_ERROR)
        {
            std::cerr << "***ERROR*** (" << filename << ": " << line << ")\n";
            switch (error)
            {
            case AL_INVALID_NAME:
                std::cerr << "AL_INVALID_NAME: a bad name (ID) was passed to an OpenAL function";
                break;
            case AL_INVALID_ENUM:
                std::cerr << "AL_INVALID_ENUM: an invalid enum value was passed to an OpenAL function";
                break;
            case AL_INVALID_VALUE:
                std::cerr << "AL_INVALID_VALUE: an invalid value was passed to an OpenAL function";
                break;
            case AL_INVALID_OPERATION:
                std::cerr << "AL_INVALID_OPERATION: the requested operation is not valid";
                break;
            case AL_OUT_OF_MEMORY:
                std::cerr << "AL_OUT_OF_MEMORY: the requested operation resulted in OpenAL running out of memory";
                break;
            default:
                std::cerr << "UNKNOWN AL ERROR: " << error;
            }
            std::cerr << std::endl;
            return false;
        }
        return true;
    }

    template<typename alFunction, typename ...Params>
    auto AudioUtils::alCallImpl(const char* filename, const std::uint_fast32_t line, alFunction function, Params ...params) -> typename std::enable_if_t<!std::is_same_v<void, decltype(function(params ...))>, decltype(function(params ...))>
    {
        auto ret = function(std::forward<Params>(params)...);
        CheckALErrors(filename, line);
        return ret;
    }

    template<typename alFunction, typename ...Params>
    auto AudioUtils::alCallImpl(const char* filename, const std::uint_fast32_t line, alFunction function, Params ...params) -> typename std::enable_if_t<std::is_same_v<void, decltype(function(params ...))>, bool>
    {
        function(std::forward<Params>(params)...);
        return CheckALErrors(filename, line);
    }

    template<typename alcFunction, typename ...Params>
    auto AudioUtils::alcCallImpl(const char* filename, const std::uint_fast32_t line, alcFunction function, ALCdevice* device, Params ...params) -> typename std::enable_if_t<std::is_same_v<void, decltype(function(params ...))>, bool>
    {
        function(std::forward<Params>(params)...);
        return CheckALCErrors(filename, line, device);
    }

    template<typename alcFunction, typename ReturnType, typename ...Params>
    auto AudioUtils::alcCallImpl(const char* filename, const std::uint_fast32_t line, alcFunction function, ReturnType& returnValue, ALCdevice* device, Params ...params) -> typename std::enable_if_t<!std::is_same_v<void, decltype(function(params ...))>, bool>
    {
        returnValue = function(std::forward<Params>(params)...);
        return CheckALCErrors(filename, line, device);
    }

}
