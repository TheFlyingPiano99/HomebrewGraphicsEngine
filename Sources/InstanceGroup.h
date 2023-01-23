#pragma once
#include "SceneObject.h"

namespace Hogra{
	class InstanceGroup
	{
	public:
		InstanceGroup() = default;

		~InstanceGroup() {
		}

		void GatherInstanceData();

		void GatherInstanceDataForShadow();

		void InjectInstanceData(const std::vector<Geometry::InstanceData>& data);

		void Draw();

		void DrawShadow();

		void Optimalize(const Camera& camera);

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
		int visibleCount = 0;
	};
}

