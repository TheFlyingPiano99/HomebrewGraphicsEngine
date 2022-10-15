#include "MemoryManager.h"

#include "ControlActionManager.h"
#include "Scene.h"
#include "AssetFolderPathManager.h"
#include "GUI.h"
#include "SceneEventManager.h"
#include "MaterialFactory.h"
#include "ShaderProgramFactory.h"
#include "SceneManager.h"
#include "SceneFactory.h"
#include "AudioBuffer.h"
#include "AudioSource.h"
#include "AudioListener.h"


namespace Hogra {
	std::set<Allocator::AllocationData> Allocator::allocations = std::set<Allocator::AllocationData>();

	bool operator<(const Allocator::AllocationData& ad, const Allocator::AllocationData& ad2) {
		return reinterpret_cast<uintptr_t>(ad.pointer) < reinterpret_cast<uintptr_t>(ad2.pointer);
	}

	bool operator>(const Allocator::AllocationData& ad, const Allocator::AllocationData& ad2) {
		return reinterpret_cast<uintptr_t>(ad.pointer) > reinterpret_cast<uintptr_t>(ad2.pointer);
	}
}

