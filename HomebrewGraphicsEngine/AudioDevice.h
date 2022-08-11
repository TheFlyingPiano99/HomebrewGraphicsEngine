#pragma once

#include "openAL/al.h"
#include "openAL/alc.h"

namespace Hogra {

	class AudioDevice
	{
	public:

		void Init();

		~AudioDevice();

		void Destroy();
			
		ALCdevice* Get() const;


	private:
		ALCdevice* device = nullptr;
	};
}

