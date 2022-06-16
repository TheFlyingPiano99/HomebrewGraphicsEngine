#pragma once

#include <vector>
#include<glm/glm.hpp>
#include "SceneObject.h"
#include "Light.h"
#include "Camera.h"
#include "PostProcessStage.h"
#include "Physics.h"
#include "Collider.h"
#include "ForceField.h"
#include "CompositeCollider.h"

namespace hograengine {

	/*
	* Singleton object
	*/
	class Scene
	{
	public:
		static Scene* init(int contextWidth, int contextHeight);
		static Scene* getInstance();
		static void destroyInstance();

		void control(float dt);
		void update(float dt);
		void draw();

		void addSceneObject(SceneObject* object);

		const glm::vec3& getPreferedUp() const;

		void togglePause();

		Camera* getCamera();

		void onContextResize(int contextWidth, int contextHeight);

		void serialize();

		~Scene() {
			serialize();
			destroy();
		}

		void pokeObject(const glm::vec2& ndcCoords);

	private:
		static Scene* instance;

		glm::vec4 backgroundColor = glm::vec4(0.07f, 0.13f, 0.17f, 1.0f);

		Camera* camera = nullptr;
		std::vector<Light*> lights;

		std::vector<ShaderProgram*> shaders;
		std::vector<Geometry*> geometries;
		std::vector<Material*> materials;
		std::vector<Mesh*> meshes;
		std::vector<SceneObject*> sceneObjects;
		std::vector<Component*> components;
		std::vector<Collider*> colliders;

		bool pause = true;
		unsigned int contextWidth;
		unsigned int contextHeight;

		glm::vec3 preferedUp = glm::vec3(0.0f, 1.0f, 0.0f);

		std::vector<Texture*> textures;
		std::vector<PostProcessStage*> postProcessStages;

		Scene(unsigned int contextWidth, unsigned int contextHeight) : contextWidth(contextWidth), contextHeight(contextHeight) {
		}

		void destroy();

		void initCameraAndLights();
		void initSceneObjects();
		void initSkyBox(Texture** cubeMap);
		void initCube(Texture** cubeMap, glm::vec3 pos, Collider* collider, ForceField* = nullptr);
		void initGroud();
		CompositeCollider* initCompositeCollider();
		ForceField* initGravitation();
		void initPostProcessStages();
		class SceneNotInstanciatedException : public std::exception {

		};
	};
}

