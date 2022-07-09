#pragma once
#include "Scene.h"

namespace hograengine {
	class SceneFactory
	{
	public:

		static SceneFactory* getInstance();

		static void destroyInstance();

		void initDefaultScene(int contextWidth, int contextHeight);

	private:
		SceneFactory() = default;
		static SceneFactory* instance;
	};
}
