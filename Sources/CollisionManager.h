#pragma once
#include "ColliderGroup.h"
#include "InstanceGroup.h"

namespace Hogra {
	class CollisionManager
	{
	public:
		CollisionManager() {
			root.Init(nullptr);
		}

		~CollisionManager();

		void AddCollider(Collider* collider, const std::string& colliderGroupName);

		void RemoveCollider(Collider* collider);

		void Collide();

		void Update();

		void SetUseSpatialTree(bool b);

		void InitDebug();

		void DrawDebug();

		Collider* IntersectRay(const Ray& ray, glm::vec3& wIntersectionPoint, glm::vec3& wIntersectionNormal);

	private:
		ColliderGroup root = ColliderGroup();
		std::vector<Collider*> colliders;
		bool useSpatialTree = true;
		InstanceGroup instanceGroup;
		ShaderProgram glyphProgram;
		SceneObject debugObj;
		Mesh debugMesh;
		Geometry* debugGeometry;
		Material debugMaterial;
	};
}

