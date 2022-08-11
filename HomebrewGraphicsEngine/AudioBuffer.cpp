#include "AudioBuffer.h"

namespace Hogra {
    
    std::vector<AudioBuffer*> AudioBuffer::heapAllocatedInstances = std::vector<AudioBuffer*>();

    AudioBuffer* AudioBuffer::Instantiate()
    {
        auto* instance = new AudioBuffer();
        heapAllocatedInstances.push_back(instance);
        return instance;
    }
    void AudioBuffer::Deallocate(AudioBuffer* toDelete)
    {
        auto iter = std::ranges::find(heapAllocatedInstances.begin(), heapAllocatedInstances.end(), toDelete);
        if (heapAllocatedInstances.end() != iter) {
            heapAllocatedInstances.erase(iter);
            delete toDelete;
        }
    }
    void AudioBuffer::DeallocateAll()
    {
        for (auto instance : heapAllocatedInstances) {
            delete instance;
        }
        heapAllocatedInstances.clear();
    }
    
    void AudioBuffer::Init(std::string& path) {
        std::uint8_t numChannels;
        std::int32_t sampleRate;
        std::uint8_t bitsPerSample;
        std::vector<char> soundData;
        
        if (!load_wav(path, numChannels, sampleRate, bitsPerSample, soundData))
        {
            std::cerr << "ERRsoundDataOR: Could not load wav" << std::endl;
        }
        ALenum format;
        if (numChannels == 1 && bitsPerSample == 8) {
            format = AL_FORMAT_MONO8;
        }
        else if (numChannels == 1 && bitsPerSample == 16) {
            format = AL_FORMAT_MONO16;
        }
        else if (numChannels == 2 && bitsPerSample == 8)
        {
            format = AL_FORMAT_STEREO8;
        }
        else if (numChannels == 2 && bitsPerSample == 16) {
            format = AL_FORMAT_STEREO16;
        }
        else
        {
            std::cerr
                << "ERROR: unrecognised wave format: "
                << numChannels << " channels, "
                << bitsPerSample << " bps" << std::endl;
            return;
        }

        alGenBuffers(1, &ID);
        alBufferData(ID, format, &soundData[0], (ALsizei)soundData.size(), (ALsizei)sampleRate);
    }

    AudioBuffer::~AudioBuffer()
    {
        alDeleteBuffers(1, &ID);
    }
    ALuint AudioBuffer::Get() const
    {
        return ID;
    }
    
    std::int32_t AudioBuffer::convert_to_int(char const* buffer, std::size_t len)
    {
        std::int32_t a = 0;
        if (std::endian::native == std::endian::little)
            std::memcpy(&a, buffer, len);
        else
            for (std::size_t i = 0; i < len; ++i)
                reinterpret_cast<char*>(&a)[3 - i] = buffer[i];
        return a;
    }

    bool AudioBuffer::load_wav_file_header(std::ifstream& file, std::uint8_t& channels, std::int32_t& sampleRate, std::uint8_t& bitsPerSample, ALsizei& size)
    {
        char buffer[4];
        if (!file.is_open())
            return false;

        // the RIFF
        if (!file.read(buffer, 4))
        {
            std::cerr << "ERROR: could not read RIFF" << std::endl;
            return false;
        }
        if (std::strncmp(buffer, "RIFF", 4) != 0)
        {
            std::cerr << "ERROR: file is not a valid WAVE file (header doesn't begin with RIFF)" << std::endl;
            return false;
        }

        // the size of the file
        if (!file.read(buffer, 4))
        {
            std::cerr << "ERROR: could not read size of file" << std::endl;
            return false;
        }

        // the WAVE
        if (!file.read(buffer, 4))
        {
            std::cerr << "ERROR: could not read WAVE" << std::endl;
            return false;
        }
        if (std::strncmp(buffer, "WAVE", 4) != 0)
        {
            std::cerr << "ERROR: file is not a valid WAVE file (header doesn't contain WAVE)" << std::endl;
            return false;
        }

        // "fmt/0"
        if (!file.read(buffer, 4))
        {
            std::cerr << "ERROR: could not read fmt/0" << std::endl;
            return false;
        }

        // this is always 16, the size of the fmt data chunk
        if (!file.read(buffer, 4))
        {
            std::cerr << "ERROR: could not read the 16" << std::endl;
            return false;
        }

        // PCM should be 1?
        if (!file.read(buffer, 2))
        {
            std::cerr << "ERROR: could not read PCM" << std::endl;
            return false;
        }

        // the number of channels
        if (!file.read(buffer, 2))
        {
            std::cerr << "ERROR: could not read number of channels" << std::endl;
            return false;
        }
        channels = convert_to_int(buffer, 2);

        // sample rate
        if (!file.read(buffer, 4))
        {
            std::cerr << "ERROR: could not read sample rate" << std::endl;
            return false;
        }
        sampleRate = convert_to_int(buffer, 4);

        // (sampleRate * bitsPerSample * channels) / 8
        if (!file.read(buffer, 4))
        {
            std::cerr << "ERROR: could not read (sampleRate * bitsPerSample * channels) / 8" << std::endl;
            return false;
        }

        // ?? dafaq
        if (!file.read(buffer, 2))
        {
            std::cerr << "ERROR: could not read dafaq" << std::endl;
            return false;
        }

        // bitsPerSample
        if (!file.read(buffer, 2))
        {
            std::cerr << "ERROR: could not read bits per sample" << std::endl;
            return false;
        }
        bitsPerSample = convert_to_int(buffer, 2);

        // data chunk header "data"
        if (!file.read(buffer, 4))
        {
            std::cerr << "ERROR: could not read data chunk header" << std::endl;
            return false;
        }
        if (std::strncmp(buffer, "data", 4) != 0)
        {
            std::cerr << "ERROR: file is not a valid WAVE file (doesn't have 'data' tag)" << std::endl;
            return false;
        }

        // size of data
        if (!file.read(buffer, 4))
        {
            std::cerr << "ERROR: could not read data size" << std::endl;
            return false;
        }
        size = convert_to_int(buffer, 4);

        /* cannot be at the end of file */
        if (file.eof())
        {
            std::cerr << "ERROR: reached EOF on the file" << std::endl;
            return false;
        }
        if (file.fail())
        {
            std::cerr << "ERROR: fail state set on the file" << std::endl;
            return false;
        }

        return true;
    }
    
    bool AudioBuffer::load_wav(const std::string& path, std::uint8_t& channels, std::int32_t& sampleRate, std::uint8_t& bitsPerSample, std::vector<char>& soundData)
    {
        int size = 0;
        std::ifstream in(path, std::ios::binary);
        if (!in.is_open())
        {
            std::cerr << "ERROR: Could not open \"" << path << "\"" << std::endl;
            return false;
        }
        if (!load_wav_file_header(in, channels, sampleRate, bitsPerSample, size))
        {
            std::cerr << "ERROR: Could not load wav header of \"" << path << "\"" << std::endl;
            return false;
        }

        soundData.resize(size);

        in.read(&soundData[0], size);

        return true;
    }
}