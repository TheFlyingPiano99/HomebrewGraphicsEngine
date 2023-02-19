#pragma once
#include<glad/glad.h>

namespace Hogra {
	class ShadowCaster
	{
	public:

		virtual void Update() = 0;

		void SetIdx(GLint i) {
			idx = i;
		}

		GLint& GetIdx() {
			return idx;
		}

	protected:
		GLint idx;
	};
}
