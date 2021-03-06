#pragma once
#include "SceneObject.h"

namespace Hogra{
	class InstanceGroup
	{
	public:
		InstanceGroup() {
		};

		~InstanceGroup() {
		}

		void gatherInstanceData();

		void injectInstanceData(const std::vector<Geometry::InstanceData>& data);

		void Draw();

		void drawShadow();

		void optimalize(const Camera& camera);

		void addObject(SceneObject* object) {
			objects.push_back(object);
		}

		void removeObject(SceneObject* object) {
			const auto& iter = std::find(objects.begin(), objects.end(), object);
			objects.erase(iter);
		}

	private:
		std::vector<SceneObject*> objects;
		std::vector<Geometry::InstanceData> instanceData;
		int optimalizationCounter = 0;
	};
}

