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

		std::int32_t convert_to_int(char const* buffer, std::size_t len);

        bool load_wav_file_header(std::ifstream& file,
            std::uint8_t& channels,
            std::int32_t& sampleRate,
            std::uint8_t& bitsPerSample,
            ALsizei& size);

        bool load_wav(const std::string& path,
            std::uint8_t& channels,
            std::int32_t& sampleRate,
            std::uint8_t& bitsPerSample,
            std::vector<char>& soundData);
    };
}

