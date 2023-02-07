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
#include "DebugUtils.h"

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

#include <iterator>


namespace Hogra {

	void Scene::initShadowMap()
	{
		if (shadowCaster != nullptr) {
			throw std::exception("Shadowcaster already initialised! Only one allowed.");
		}
		shadowCaster = Allocator::New<ShadowCaster>();
		shadowCaster->Init(glm::vec3(-20, 20, -20), glm::normalize(glm::vec3(1, -1, 1)));
	}

	FBO* Scene::findNextFBO(int currentLayer)
	{
		FBO* outFBO = nullptr;
		for (auto i = currentLayer + 1; i < renderLayers.size(); i++) {
			auto fbo = renderLayers[i]->GetInFBO();
			if (nullptr != fbo) {
				outFBO = fbo;
				break;
			}
		}
		return outFBO;
	}
	

	const SceneChange& Scene::GetSceneChange()
	{
		return sceneChange;
	}

	Collider* Scene::IntersectRay(const Ray& ray, glm::vec3& w_intersectionPoint, glm::vec3& w_intersectionNormal)
	{
		return collisionManager.IntersectRay(ray, w_intersectionPoint, w_intersectionNormal);
	}

	void Scene::Init(unsigned int _contextWidth, unsigned int _contextHeight)
	{
		audioManager.Init();
		initShadowMap();
		camera.Init((float)_contextWidth / (float)_contextHeight, glm::vec3(-10.0f, 10.0f, -10.0f), glm::vec3(0.0f, 0.0f, 0.0f));
		lightManager.initDefferedSystem(_contextWidth, _contextHeight);
		lightManager.initDebug();
		collisionManager.InitDebug();
		timeSpent.Init("time", 0.0f);
		OnContextResize(_contextWidth, _contextHeight);
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

		const Texture2D& depth = lightManager.GetDepthTexture();
		FBO defaultFBO = FBO::GetDefault();

		// Get first FBO in pipeline to use for state clearing and initialisation:
		// |layer0| -> |layer1| -> |layer2|
		// | null |    | fbo0 |    | fbo2 |
		//				>this<

		auto* outFBO = findNextFBO(-1);
		if (nullptr == outFBO) {
			outFBO = &defaultFBO;
		}
		outFBO->LinkTexture(GL_DEPTH_ATTACHMENT, depth, 0);
		outFBO->Bind();

		glClearColor(backgroundColor.x, backgroundColor.y, backgroundColor.z, backgroundColor.w);
		glEnable(GL_DEPTH_TEST);
		glDepthMask(GL_TRUE);
		glDepthFunc(GL_LESS);
		glClearDepth(1);
		glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
		glStencilMask(GL_FALSE);


		for (int i = 0; i < renderLayers.size(); i++) {
			outFBO = findNextFBO(i);
			if (nullptr == outFBO) {
				outFBO = &defaultFBO;
			}
			renderLayers[i]->Render(*outFBO, depth, camera, lightManager);
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

	void Scene::AddSceneObject(SceneObject* object, const std::string& instanceGroupName, const std::string& renderLayerName)
	{
		// If already contains:
		if (auto objectIter = std::find(sceneObjects.begin(), sceneObjects.end(), object); objectIter != sceneObjects.end()) {		
			return;
		}

		sceneObjects.push_back(object);		// Add to main collection in Scene


		// Add to instance group:
		static int defaultName = 0;
		if (object->GetMesh() != nullptr) {
			auto* layer = GetRenderLayer(renderLayerName);
			if (nullptr == layer) {
				std::cerr << "Scene Error: Trying to add scene object to unknown render layer!" << std::endl;
				return;
			}
			if (instanceGroupName.length() > 0) {
				const auto& iter = instanceGroups.find(instanceGroupName);
				if (iter != instanceGroups.end()) {
					iter->second->addObject(object);
				}
				else {
					auto* group = Allocator::New<InstanceGroup>();
					group->addObject(object);
					instanceGroups.emplace(instanceGroupName, group);
					if (layer->IsInstanced()) {	// Add to layer
						layer->AddInstanceGroup(group);
					}
				}
			}
			else {
				auto* group = Allocator::New<InstanceGroup>();
				group->addObject(object);
				instanceGroups.emplace(std::to_string(defaultName++), group);
				if (layer->IsInstanced()) {	// Add to layer
					layer->AddInstanceGroup(group);
				}
			}
			// Add to layer if the layer is not using instanced rendering:
			if (!layer->IsInstanced()) {
				layer->AddObject(object);
			}
		}
	}

	void Scene::RemoveSceneObject(SceneObject* object)
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

	void Scene::AddPostProcessStage(PostProcessStage* stage, const std::string& renderLayerName) {
		if (renderLayerName.empty()) {
			stage->AddUniformVariable(&timeSpent);
			(*renderLayers.cend())->AddPostProcessStage(stage);
		}
		auto layer = renderLayersMap.find(renderLayerName);
		if (layer == renderLayersMap.end()) {
			std::cerr << "Trying to add post process effect to unknown render layer!" << std::endl;
			return;
		}
		stage->AddUniformVariable(&timeSpent);
		layer->second->AddPostProcessStage(stage);
		postProcessStages.push_back(stage);		// Main PPS container in Scene
	}

	void Scene::AddLight(Light* light)
	{
		lightManager.AddLight(light);
		lights.push_back(light);
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

	void Scene::OnContextResize(int _contextWidth, int _contextHeight)
	{
		DebugUtils::PrintMsg("Scene", "Context resize");

		camera.SetAspectRatio((float)_contextWidth / (float)_contextHeight);
		for (auto& stage : postProcessStages) {
			stage->OnContextResize(_contextWidth, _contextHeight);
		}
		camera.SetChanged(true);

		lightManager.OnContextResize(_contextWidth, _contextHeight);
		for (auto& cap : captions) {
			cap->UpdateText(L"Context resize. #%!?'!%Ű_:?'§");
		}
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