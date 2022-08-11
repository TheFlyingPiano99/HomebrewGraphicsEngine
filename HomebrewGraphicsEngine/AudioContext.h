#pragma once

#include "openAL/al.h"
#include "openAL/alc.h"
#include "AudioDevice.h"

namespace Hogra {
	class AudioContext
	{
	public:
		void Init();

		void Destroy();

		~AudioContext();

		void MakeCurrent();

		static void MakeDefaultCurrent();

	private:
		ALCcontext* context = nullptr;
		AudioDevice device;
	};
}

