#pragma once
#include<glad/glad.h>

namespace Hogra {
	class ShadowCaster
	{
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
