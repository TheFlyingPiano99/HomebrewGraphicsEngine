#pragma once

#include "openAL/al.h"
#include "openAL/alc.h"
#include <vector>


namespace Hogra {

	class AudioListener
	{
	public:

		static AudioListener* Instantiate();

		static void Deallocate(AudioListener* toDelete);

		static void DeallocateAll();

		void Init();

	private:
		static std::vector<AudioListener*> heapAllocatedInstances;

	};
}
