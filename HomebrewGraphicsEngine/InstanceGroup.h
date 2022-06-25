#pragma once
#include "SceneObject.h"

namespace hograengine{
	class InstanceGroup
	{
	public:
		InstanceGroup() {
		};

		void gatherInstanceData();

		void draw();

		void drawShadow();

		void addObject(SceneObject* object) {
			objects.push_back(object);
		}

		void removeObject(SceneObject* object) {
			const auto& iter = std::find(objects.begin(), objects.end(), object);
			objects.erase(iter);
		}

		void setVertexAttribArray();

	private:
		std::vector<SceneObject*> objects;
		std::vector<Geometry::InstanceData> instanceData;
	};
}

