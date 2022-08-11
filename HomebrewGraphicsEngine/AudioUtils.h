#pragma once

#include "openAL/al.h"
#include "openAL/alc.h"
#include <iostream>


#define alCall(function, ...) alCallImpl(__FILE__, __LINE__, function, __VA_ARGS__)
#define alcCall(function, device, ...) alcCallImpl(__FILE__, __LINE__, function, device, __VA_ARGS__)

namespace Hogra {
	class AudioUtils
	{
	public:

        // Al:

        template<typename alFunction, typename... Params>
        static auto alCallImpl(const char* filename,
            const std::uint_fast32_t line,
            alFunction function,
            Params... params)
            ->typename std::enable_if_t<!std::is_same_v<void, decltype(function(params...))>, decltype(function(params...))>;

        template<typename alFunction, typename... Params>
        static auto alCallImpl(const char* filename,
            const std::uint_fast32_t line,
            alFunction function,
            Params... params)
            ->typename std::enable_if_t<std::is_same_v<void, decltype(function(params...))>, bool>;

        // Alc:


        template<typename alcFunction, typename... Params>
        static auto alcCallImpl(const char* filename,
            const std::uint_fast32_t line,
            alcFunction function,
            ALCdevice* device,
            Params... params)
            ->typename std::enable_if_t<std::is_same_v<void, decltype(function(params...))>, bool>;

        template<typename alcFunction, typename ReturnType, typename... Params>
        static auto alcCallImpl(const char* filename,
            const std::uint_fast32_t line,
            alcFunction function,
            ReturnType& returnValue,
            ALCdevice* device,
            Params... params)
            ->typename std::enable_if_t<!std::is_same_v<void, decltype(function(params...))>, bool>;

    private:
        static bool CheckALErrors(const std::string& filename, const std::uint_fast32_t line);
        static bool CheckALCErrors(const std::string& filename, const std::uint_fast32_t line, ALCdevice* device);
    };
}

