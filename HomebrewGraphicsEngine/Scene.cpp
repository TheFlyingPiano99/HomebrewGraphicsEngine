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
#include "GUI.h"
#include "HograTime.h"

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

namespace Hogra {

	void Scene::initShadowMap()
	{
		if (shadowCaster != nullptr) {
			throw std::exception("Shadowcaster already initialised! Only one allowed.");
		}
		shadowCaster = Allocator::New<ShadowCaster>();
		shadowCaster->Init(glm::vec3(-20, 20, -20), glm::normalize(glm::vec3(1, -1, 1)));
	}
	

	const SceneChange& Scene::GetSceneChange()
	{
		return sceneChange;
	}

	Collider* Scene::IntersectRay(const Ray& ray, glm::vec3& w_intersectionPoint, glm::vec3& w_intersectionNormal)
	{
		return collisionManager.IntersectRay(ray, w_intersectionPoint, w_intersectionNormal);
	}

	void Scene::Init(int contextWidth, int contextHeight)
	{
		this->contextWidth = contextWidth;
		this->contextHeight = contextHeight;

		audioManager.Init();
		initShadowMap();
		camera.Init((float)contextWidth / (float)contextHeight, glm::vec3(-10.0f, 10.0f, -10.0f), glm::vec3(0.0f, 0.0f, 0.0f));
		lightManager.initDefferedSystem(contextWidth, contextHeight);
		lightManager.initDebug();
		collisionManager.InitDebug();
		timeSpent.Init("time", 0.0f);
	}

	void Scene::BeforePhysicsLoopUpdate() {
		for (auto& obj : sceneObjects) {
			obj->BeforePhysicsLoopUpdate();
		}
		ControlActionManager::getInstance()->ExecuteQueue(*this);
	}

	void Scene::Destroy()
	{

		lightManager.Clear();

		for (auto& instanceGroup : instanceGroups) {	//8
			Allocator::Delete(instanceGroup.second);
		}
		instanceGroups.clear();

		Allocator::Delete(shadowCaster);

		for (auto& postProcStage : postProcessStages) {	//14
			Allocator::Delete(postProcStage);
		}
		postProcessStages.clear();

		for (auto& sceneObject : sceneObjects) {
			Allocator::Delete(sceneObject);
		}

		for (auto renderLayer : renderLayers) {
			Allocator::Delete(renderLayer);
		}

		if (nullptr != userControl) {
			Allocator::Delete(userControl);
		}
	}

	void Scene::PhysicsUpdate(float dt)
	{
		collisionManager.Collide();
		timeSpent.Set(Time::totalTime);
		for (auto& obj : sceneObjects) {
			obj->EarlyPhysicsUpdate(dt);
		}

		SceneEventManager::getInstance()->ExecuteQueue(*this);

		for (auto& obj : sceneObjects) {
			obj->LatePhysicsUpdate(dt);
		}

		camera.LatePhysicsUpdate(dt);
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
		UpdateGUI();

		// Init and export data:
		camera.ExportData();
		lightManager.ExportData();

		/*
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

		// Geometry pass:
		for (auto& group : instanceGroups) {
			group.second->GatherInstanceData();
		}

		lightManager.BindGBuffer();
		for (auto& group : instanceGroups) {
			group.second->Draw();
		}
		*/

		const Texture2D& depth = lightManager.GetDepthTexture();
		FBO defaultFBO = FBO::GetDefault();
		// Deferred lighting pass:
		if (renderLayers.empty()) {
			defaultFBO.Bind();
		}
		else {
			renderLayers.front()->GetInFBO().Bind();
			renderLayers.front()->GetInFBO().LinkTexture(GL_DEPTH_ATTACHMENT, depth, 0);
		}
		glClearColor(backgroundColor.x, backgroundColor.y, backgroundColor.z, backgroundColor.w);
		glClearDepth(1);
		glDisable(GL_DEPTH_TEST);
		glDepthFunc(GL_LESS);
		glDisable(GL_BLEND);
		glEnable(GL_CULL_FACE);
		glFrontFace(GL_CCW);
		glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
		glStencilMask(0x00);
		lightManager.RenderDeferredLighting();

		for (int i = 0; i < renderLayers.size(); i++) {
			renderLayers[i]->Render((i < renderLayers.size() - 1)? renderLayers[i + 1]->GetInFBO() : defaultFBO, depth, camera);
		}

		// Text pass:
		for (auto* caption : captions) {
			caption->Draw();
		}

		if (debugMode) {
			collisionManager.DrawDebug();
			lightManager.drawDebug();
		}

		// Reset camera state:
		camera.SetIsMoved(false);
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
					auto* group = Allocator::New<InstanceGroup>();
					group->addObject(object);
					instanceGroups.emplace(instanceGroupName, group);
				}
			}
			else {
				auto* group = Allocator::New<InstanceGroup>();
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
		stage->AddUniformVariable(&timeSpent);
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
		for (auto& obj : sceneObjects) {
			obj->Serialize();
		}
	}

	void Scene::UpdateGUI() {

		for (auto* obj : sceneObjects) {
			obj->UpdateGui();
		}
	}

}