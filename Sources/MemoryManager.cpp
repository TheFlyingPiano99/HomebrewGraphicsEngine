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
}

