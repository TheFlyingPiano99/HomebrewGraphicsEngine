#pragma once

#include "openAL/al.h"
#include "openAL/alc.h"

#include <vector>
#include <string>
#include <iostream>
#include <fstream>

namespace Hogra {

	class AudioBuffer
	{
	public:

		static AudioBuffer* Instantiate();

		static void Deallocate(AudioBuffer* toDelete);

		static void DeallocateAll();

		void Init(std::string& path);

		~AudioBuffer();

		/*
		* Return OpenAL buffer id
		*/
		ALuint Get() const;

	private:
		ALuint ID;
		static std::vector<AudioBuffer*> heapAllocatedInstances;

		/*
		* The implementation of this function is from the article "The Complete Guide to OpenAL with C++ – Part 1: Playing a Sound"
		* Published on the website indiegamedev.net
		* URL: https://indiegamedev.net/2020/02/15/the-complete-guide-to-openal-with-c-part-1-playing-a-sound/
		*/
		std::int32_t ConvertToInt(char const* buffer, std::size_t len);

		/*
		* The implementation of this function is from the article "The Complete Guide to OpenAL with C++ – Part 1: Playing a Sound"
		* Published on the website indiegamedev.net
		* URL: https://indiegamedev.net/2020/02/15/the-complete-guide-to-openal-with-c-part-1-playing-a-sound/
		*/
		bool LoadWavFileHeader(std::ifstream& file,
            std::uint8_t& channels,
            std::int32_t& sampleRate,
            std::uint8_t& bitsPerSample,
            ALsizei& size);

		/*
		* The implementation of this function is from the article "The Complete Guide to OpenAL with C++ – Part 1: Playing a Sound"
		* Published on the website indiegamedev.net
		* URL: https://indiegamedev.net/2020/02/15/the-complete-guide-to-openal-with-c-part-1-playing-a-sound/
		*/
		bool LoadWav(const std::string& path,
            std::uint8_t& channels,
            std::int32_t& sampleRate,
            std::uint8_t& bitsPerSample,
            std::vector<char>& soundData);
    };
}

