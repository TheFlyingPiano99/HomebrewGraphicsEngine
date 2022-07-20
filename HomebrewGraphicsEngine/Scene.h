#pragma once

#include <vector>
#include <map>
#include<glm/glm.hpp>
#include "SceneObject.h"
#include "Light.h"
#include "Camera.h"
#include "PostProcessStage.h"
#include "Physics.h"
#include "Collider.h"
#include "ForceField.h"
#include "CompositeCollider.h"
#include "ShadowCaster.h"
#include "UserControl.h"
#include "TextureCube.h"
#include "LightManager.h"
#include "InstanceGroup.h"
#include "CollisionManager.h"
#include "Light.h"
#include "Font.h"
#include "Caption.h"
#include "Bloom.h"

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

		void addSceneObject(SceneObject* object, const std::string& instanceGroupName = "");

		void addCollider(Collider* collider, const std::string& colliderGroupName = "");

		void addPostProcessStage(PostProcessStage* stage);

		void setCamera(Camera* _camera);

		void addLight(Light* light);

		void addLights(const std::vector<Light*>& lights);

		void addFont(Font* font);

		void addCaption(Caption* caption);

		void setUserControl(UserControl* uc);

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

		UserControl* getAvatarControl() {
			return userControl;
		}

		bool getDrawDebug() const {
			return drawDebug;
		}

		void setDrawDebug(bool b) {
			drawDebug = b;
		}

		void setBloomTreshold(float treshold) {
			bloom.setTreshold(treshold);
		}

		void setBloomIntensity(float intensity) {
			bloom.setIntensity(intensity);
		}

	private:
		static Scene* instance;

		glm::vec4 backgroundColor = glm::vec4(0.07f, 0.13f, 0.17f, 1.0f);

		Camera* camera = nullptr;
		std::vector<Light*> lights;
		LightManager lightManager;
		Bloom bloom;

		std::vector<ShaderProgram*> shaders;
		std::vector<Geometry*> geometries;
		std::vector<Material*> materials;
		std::vector<Mesh*> meshes;
		std::vector<SceneObject*> sceneObjects;
		std::map<std::string, InstanceGroup*> instanceGroups;
		std::vector<Component*> components;
		std::vector<Font*> fonts;
		std::vector<Caption*> captions;
		UserControl* userControl = nullptr;

		CollisionManager collisionManager;

		bool pause = true;
		unsigned int contextWidth;
		unsigned int contextHeight;
		bool drawDebug = false;

		glm::vec3 preferedUp = glm::vec3(0.0f, 1.0f, 0.0f);

		std::vector<const Texture*> textures;
		std::vector<PostProcessStage*> postProcessStages;
		ShadowCaster* shadowCaster = nullptr;

		Scene(unsigned int contextWidth, unsigned int contextHeight) : contextWidth(contextWidth), contextHeight(contextHeight) {
		}

		void destroy();

		void initCameraAndLights();
		void initShadowMap();
		void initSceneObjects();
		void initSkyBox(Texture** cubeMap);
		void initCube(Texture** cubeMap, glm::vec3 pos, Collider* collider, ForceField* = nullptr);
		void initSphere(Texture** cubeMap, const glm::vec3& pos, ForceField* field = nullptr);
		void initLoadedGeometry(Texture** cubeMap, const glm::vec3& pos, ForceField* field = nullptr);
		void initGroud(const Texture* skyBox);
		void initAvatar(ForceField* gravitation);
		void initFonts();
		CompositeCollider* initCompositeCollider();
		ForceField* initGravitation();
		void initPostProcessStages();
		class SceneNotInstanciatedException : public std::exception {

		};
	};
}

