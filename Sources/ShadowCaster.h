#pragma once
#include<glad/glad.h>
#include "MemoryManager.h"

namespace Hogra {
	class ShadowCaster
	{
		ALLOCATOR_CONSTRUCTIBLE
	public:

		virtual void Update() = 0;

		void SetIdx(int i) {
			idx = i;
		}

		const int& GetIdx() {
			return idx;
		}

	protected:
		int idx;
	};
}
