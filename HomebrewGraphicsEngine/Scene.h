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
#include "SceneChange.h"
#include "AudioManager.h"
#include "SceneAudioSource.h"
#include "UniformVariable.h"
#include "volumetric/VolumeObject.h"
#include "RenderLayer.h"
#include "UniformVariableImpl.h"


namespace Hogra {

	/*
	* Singleton object
	*/
	class Scene
	{
	public:
		void Init(int contextWidth, int contextHeight);

		/*
		* Calls SceneObject::BeforePhysicsLoopUpdate()
		* and executes control actions
		*/
		void BeforePhysicsLoopUpdate();

		/*
		* Performs collisions
		* Calls SceneObject::EarlyPhysicsUpdate(float dt)
		* Performs scene events
		* Calls SceneObject::LatePhysicsUpdate(float dt)
		* Calls SceneObject::Update()
		*/
		void PhysicsUpdate(float dt);

		/*
		* Calls SceneObject::AfterPhysicsLoopUpdate()
		*/
		void AfterPhysicsLoopUpdate();

		void Draw();

		void AddSceneObject(SceneObject* object, const std::string& instanceGroupName = "", const std::string& renderLayerName = "layer0");

		/*
		* Not complete!
		*/
		void RemoveSceneObejct(SceneObject* object);

		void AddCollider(Collider* collider, const std::string& colliderGroupName = "");

		void AddPostProcessStage(PostProcessStage* stage, const std::string& renderLayerName = "");

		void AddLight(Light* light);

		void AddCaption(Caption* caption);

		void AddSceneAudioSource(SceneAudioSource* source);

		void SetUserControl(UserControl* uc);

		const glm::vec3& getPreferedUp() const;

		void TogglePause();

		Camera& GetCamera();

		void Resize(int contextWidth, int contextHeight);

		void Serialize();

		void SetBackgroundColor(const glm::vec3 color) {
			backgroundColor = glm::vec4(color, 1.0);
		}

		~Scene() {
			Destroy();
		}

		UserControl* GetUserControl() {
			return userControl;
		}

		bool getDrawDebug() const {
			return debugMode;
		}

		void setDrawDebug(bool b) {
			debugMode = b;
		}

		ShadowCaster* getShadowCaster() const {
			return shadowCaster;
		}

		const SceneChange& GetSceneChange();

		void SetSceneChange(const SceneChange& change) {
			this->sceneChange = change;
		}

		Collider* IntersectRay(const Ray& ray, glm::vec3& intersectionPoint, glm::vec3& intersectionNormal);

		void UpdateGUI();

		void AddRenderLayer(RenderLayer* renderLayer, int place = -1) {
			// To map:
			auto name = renderLayer->GetName();
			if (name.empty()) {	// Prevent unnamed layers
				name = "layer" + std::to_string(renderLayers.size());
				renderLayer->SetName(name);
			}
			renderLayersMap.emplace(name, renderLayer);

			// To vector:
			if (-1 < place) {
				auto iter = renderLayers.begin();
				for (int i = 0; i < place; i++) {
					std::next(iter);
				}
				renderLayers.emplace(iter, renderLayer);
			}
			else {
				renderLayers.push_back(renderLayer);
			}
		}

		RenderLayer* GetRenderLayer(const std::string& name) const {
			auto val = renderLayersMap.find(name);
			if (renderLayersMap.end() == val) {
				return nullptr;
			}
			return val->second;
		}

	private:

		glm::vec4 backgroundColor = glm::vec4(0.07f, 0.13f, 0.17f, 1.0f);

		Camera camera;
		LightManager lightManager;
		AudioManager audioManager;
		
		//To deallocate:
		std::vector<Light*> lights;
		std::vector<ShaderProgram*> shaders;
		std::vector<SceneObject*> sceneObjects;
		std::map<std::string, InstanceGroup*> instanceGroups;
		std::vector<Caption*> captions;
		UserControl* userControl = nullptr;
		ShadowCaster* shadowCaster = nullptr;
		std::map<std::string, RenderLayer*> renderLayersMap;
		std::vector<RenderLayer*> renderLayers;
		std::vector<PostProcessStage*> postProcessStages;

		CollisionManager collisionManager;
		UniformVariable<float> timeSpent;

		bool pause = true;
		unsigned int contextWidth;
		unsigned int contextHeight;
		bool debugMode = false;
		SceneChange sceneChange;

		glm::vec3 preferedUp = glm::vec3(0.0f, 1.0f, 0.0f);

		void Destroy();

		void initShadowMap();

		class SceneNotInstanciatedException : public std::exception {

		};
	};
}

