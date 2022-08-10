﻿#pragma once
#include "Scene.h"
#include "FirstPersonControl.h"
#include<glm/glm.hpp>

namespace Hogra {
	class SceneFactory
	{
	public:

		static SceneFactory* getInstance();

		static void DestroyInstance();

		Scene* CreateDemoScene(int contextWidth, int contextHeight);

		ForceField* InitGravitation(Scene* scene);

		CompositeCollider* InitCompositeCollider();

		void InitCube(Scene* scene, glm::vec3 pos, Collider* collider, ForceField* field);

		SceneObject* InitSphere(Scene* scene, const glm::vec3& pos, ForceField* field, const char* materialName);

		void InitCaptions(Scene* scene);

		void InitGroud(Scene* scene);

		void InitSkyBox(Scene* scene);

		void InitLoadedGeometry(Scene* scene, const glm::vec3& pos, ForceField* field);

		void InitAvatar(Scene* scene, ForceField* gravitation);

		void InitLaserBeam(Scene* scene, FirstPersonControl* control);

	private:
		SceneFactory() = default;
		static SceneFactory* instance;
	};
}
