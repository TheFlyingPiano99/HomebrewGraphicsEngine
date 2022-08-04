#pragma once
#include "ColliderGroup.h"
#include "InstanceGroup.h"

namespace Hogra {
	class CollisionManager
	{
	public:
		CollisionManager() = default;

		~CollisionManager();

		void AddCollider(Collider* collider, const std::string& colliderGroupName);

		void RemoveCollider(Collider* collider);

		void Collide();

		void Update();

		void SetUseSpatialTree(bool b);

		void InitDebug();

		void DrawDebug();

	private:
		ColliderGroup root = ColliderGroup(nullptr);
		std::vector<Collider*> colliders;
		bool useSpatialTree = true;
		InstanceGroup instanceGroup;
		ShaderProgram shaderProgram;
		SceneObject debugObj;
		Mesh debugMesh;
		Geometry* debugGeometry;
		Material debugMaterial;
	};
}

