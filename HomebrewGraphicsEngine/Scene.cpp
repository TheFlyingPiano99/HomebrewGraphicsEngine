#include "Scene.h"
#include "GlobalInclude.h"
#include "ControlActionManager.h"
#include "AssetFolderPathManager.h"
#include "TextureCube.h"
#include "GeometryFactory.h"
#include "AudioSource.h"
#include "AudioListener.h"
#include "Physics.h"
#include "SphericalCollider.h"
#include "AABBCollider.h"
#include "SceneEventManager.h"
#include "ForceField.h"
#include "CompositeCollider.h"
#include "FirstPersonControl.h"
#include "GeometryLoader.h"
#include "MaterialFactory.h"
#include "ShaderProgramFactory.h"

namespace Hogra {

	void Scene::initShadowMap()
	{
		if (shadowCaster != nullptr) {
			throw std::exception("Shadowcaster already initialised! Only one allowed.");
		}
		shadowCaster = new ShadowCaster(glm::vec3(-20, 20, -20), glm::normalize(glm::vec3(1, -1, 1)));
	}

	void Scene::initPostProcessStages()
	{
		timeSpent.Init("time", 0.0f);
		auto* stage0 = new PostProcessStage(AssetFolderPathManager::getInstance()->getShaderFolderPath().append("depthEffects.frag"),
			contextWidth, contextHeight);
		postProcessStages.push_back(stage0);
		auto* bloom = new Bloom();
		bloom->Init(contextWidth, contextHeight);
		postProcessStages.push_back(bloom);
		auto* stage1 = new PostProcessStage(AssetFolderPathManager::getInstance()->getShaderFolderPath().append("hdr.frag"),
			contextWidth, contextHeight);
		postProcessStages.push_back(stage1);
		auto* stage2 = new PostProcessStage(AssetFolderPathManager::getInstance()->getShaderFolderPath().append("tapeEffect.frag"),
			contextWidth, contextHeight);
		stage2->AddUniformVariable(&timeSpent);
		postProcessStages.push_back(stage2);

		//auto* stage1 = new PostProcessStage(AssetFolderPathManager::getInstance()->getShaderFolderPath().append("edgeDetect.frag"),
		//	contextWidth, contextHeight);
		//postProcessStages.push_back(stage1);
	}

	const SceneChange& Scene::GetSceneChange()
	{
		return sceneChange;
	}

	Collider* Scene::IntersectRay(const Ray& ray, glm::vec3& intersectionPoint, glm::vec3& intersectionNormal)
	{
		return collisionManager.IntersectRay(ray, intersectionPoint, intersectionNormal);
	}

	void Scene::Init(int contextWidth, int contextHeight)
	{
		this->contextWidth = contextWidth;
		this->contextHeight = contextHeight;

		audioManager.Init();
		initShadowMap();
		camera.Init((float)contextWidth / (float)contextHeight, glm::vec3(-10.0f, 10.0f, -10.0f), glm::vec3(-9.0f, 10.0f, -9.0f));
		lightManager.initDefferedSystem(contextWidth, contextHeight);
		lightManager.initDebug();
		initPostProcessStages();
		collisionManager.InitDebug();
	}

	void Scene::BeforePhysicsLoopUpdate() {
		for (auto& obj : sceneObjects) {
			obj->BeforePhysicsLoopUpdate();
		}
		ControlActionManager::getInstance()->ExecuteQueue(*this);
	}

	void Scene::Destroy()
	{
		/*
 		std::vector<Light*> lights;				//1
		std::vector<ShaderProgram*> shaders;	//2
		std::vector<Geometry*> geometries;		//3
		std::vector<Material*> materials;		//4
		std::vector<Mesh*> meshes;				//5
		std::vector<const Texture*> textures;	//6
		std::vector<SceneObject*> sceneObjects;	//7
		std::map<std::string, InstanceGroup*> instanceGroups;	//8
		std::vector<Component*> components;		//9
		std::vector<Font*> fonts;				//10
		std::vector<Caption*> captions;			//11
		UserControl* userControl = nullptr;		//12
		ShadowCaster* shadowCaster = nullptr;	//13
		std::vector<PostProcessStage*> postProcessStages;	//14
		*/

		lightManager.Clear();

		for (auto& instanceGroup : instanceGroups) {	//8
			delete instanceGroup.second;
		}
		instanceGroups.clear();

		if (nullptr != shadowCaster) {			//13
			delete shadowCaster;
			shadowCaster = nullptr;
		}

		for (auto& postProcStage : postProcessStages) {	//14
			delete postProcStage;
		}
		postProcessStages.clear();
	}

	//-----------------------------------------------------------------------------

	void Scene::PhysicsUpdate(float dt)
	{
		collisionManager.Collide();
		timeSpent.Set(timeSpent.Get() + dt);
		for (auto& obj : sceneObjects) {
			obj->EarlyPhysicsUpdate(dt);
		}

		SceneEventManager::getInstance()->ExecuteQueue(*this);

		for (auto& obj : sceneObjects) {
			obj->LatePhysicsUpdate(dt);
		}

		camera.LatePhysicsUpdate(dt);
		for (auto& obj : sceneObjects) {
			obj->Update();
		}
		collisionManager.Update();
	}

	void Scene::AfterPhysicsLoopUpdate()
	{
		camera.Update();
		for (auto& obj : sceneObjects) {
			obj->AfterPhysicsLoopUpdate();
		}
		if (shadowCaster != nullptr) {
			shadowCaster->Update();
		}
		for (auto& group : instanceGroups) {
			group.second->Optimalize(camera);
		}
		lightManager.Update();
		audioManager.Update();
	}

	void Scene::Draw()
	{
		// Init and export data:
		camera.ExportData();
		lightManager.ExportData();

		// Shadow pass:
		for (auto& group : instanceGroups) {
			group.second->GatherInstanceDataForShadow();
		}
		if (nullptr != shadowCaster) {
			shadowCaster->Bind();
			for (auto& group : instanceGroups) {
				group.second->DrawShadow();
			}
		}
		for (auto& group : instanceGroups) {
			group.second->GatherInstanceData();
		}

		// Geometry pass:
		lightManager.BindGBuffer();
		for (auto& group : instanceGroups) {
			group.second->Draw();
		}
		postProcessStages[0]->Bind();
		// Deferred lighting pass:
		glClearColor(backgroundColor.x, backgroundColor.y, backgroundColor.z, backgroundColor.w);
		glClearDepth(1);
		glEnable(GL_DEPTH_TEST);
		glDepthFunc(GL_LESS);
		glFrontFace(GL_CCW);
		glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
		glStencilMask(0x00);

		lightManager.renderDeferredLighting();
		// Post-process pass:
		for (int i = 0; i < postProcessStages.size(); i++) {
			if (i < postProcessStages.size() - 1) {
				postProcessStages[i]->Draw(postProcessStages[i + 1]->GetFBO(), lightManager.GetDepthTexture());
			}
			else {
				postProcessStages[i]->Draw(FBO::getDefault(), lightManager.GetDepthTexture());
			}
		}

		// Text pass:
		for (auto* caption : captions) {
			caption->Draw();
		}

		if (debugMode) {
			collisionManager.DrawDebug();
			lightManager.drawDebug();
		}
	}

	void Scene::AddSceneObject(SceneObject* object, const std::string& instanceGroupName)
	{
		if (auto objectIter = std::find(sceneObjects.begin(), sceneObjects.end(), object); objectIter != sceneObjects.end()) {		// If already contains
			return;
		}

		sceneObjects.push_back(object);

		static int defaultName = 0;
		if (object->GetMesh() != nullptr) {
			if (instanceGroupName.length() > 0) {
				const auto& iter = instanceGroups.find(instanceGroupName);
				if (iter != instanceGroups.end()) {
					iter->second->addObject(object);
				}
				else {
					auto* group = new InstanceGroup();
					group->addObject(object);
					instanceGroups.emplace(instanceGroupName, group);
				}
			}
			else {
				auto* group = new InstanceGroup();
				group->addObject(object);
				instanceGroups.emplace(std::to_string(defaultName++), group);
			}
		}
	}

	void Scene::RemoveSceneObejct(SceneObject* object)
	{
		auto iter = std::ranges::find(sceneObjects.begin(), sceneObjects.end(), object);
		if (sceneObjects.end() != iter) {
			sceneObjects.erase(iter);
		}
		//TODO
		//Remove from instance group
		//Remove Collider
	}

	void Scene::AddCollider(Collider* collider, const std::string& colliderGroupName)
	{
		collisionManager.AddCollider(collider, colliderGroupName);
	}

	void Scene::AddPostProcessStage(PostProcessStage* stage) {
		postProcessStages.push_back(stage);
	}

	void Scene::AddLight(Light* light)
	{
		lightManager.AddLight(light);
	}

	void Scene::AddCaption(Caption* caption)
	{
		if (!captions.empty() && std::find(captions.begin(), captions.end(), caption) != captions.end()) {
			return;
		}
		auto* shader = caption->GetShaderProgram();
		auto shaderIter = std::find(shaders.begin(), shaders.end(), shader);
		if (shader != nullptr && shaderIter == shaders.end()) {
			shaders.push_back(shader);
		}
		captions.push_back(caption);
	}

	void Scene::AddSceneAudioSource(SceneAudioSource* source)
	{
		audioManager.AddSource(source);
	}

	void Scene::SetUserControl(UserControl* uc) {
		userControl = uc;
	}

	const glm::vec3& Scene::getPreferedUp() const {
		return preferedUp;
	}

	void Scene::TogglePause()
	{
		pause = !pause;
	}

	Camera& Scene::GetCamera() {
		return camera;
	}

	void Scene::Resize(int _contextWidth, int _contextHeight)
	{
		this->contextWidth = _contextWidth;
		this->contextHeight = _contextHeight;
		camera.setAspectRatio((float)_contextWidth / (float)_contextHeight);
		for (auto& stage : postProcessStages) {
			stage->OnResize(_contextWidth, _contextHeight);
		}
		camera.setMoved(true);
		lightManager.OnResize(_contextWidth, _contextHeight);
	}

	void Scene::Serialize()
	{
		//TODO
	}
}