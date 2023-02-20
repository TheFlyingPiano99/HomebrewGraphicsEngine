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
		camera.Init((float)_contextWidth / (float)_contextHeight, glm::vec3(-10.0f, 10.0f, -10.0f), glm::vec3(0.0f, 0.0f, 0.0f));
		lightManager.InitDefferedSystem(_contextWidth, _contextHeight);
		lightManager.InitDebug();
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

		for (auto& light : dirLights) {
			Allocator::Delete(light);
		}
		dirLights.clear();

		for (auto& light : pointLights) {
			Allocator::Delete(light);
		}
		pointLights.clear();

		for (auto& instanceGroup : instanceGroups) {
			Allocator::Delete(instanceGroup.second);
		}
		instanceGroups.clear();

		for (auto& dirCaster : dirShadowCasters) {
			Allocator::Delete(dirCaster);
		}
		dirShadowCasters.clear();

		for (auto& omniCaster : omniDirShadowCasters) {
			Allocator::Delete(omniCaster);
		}
		omniDirShadowCasters.clear();

		for (auto& postProcStage : postProcessStages) {
			Allocator::Delete(postProcStage);
		}
		postProcessStages.clear();

		for (auto& sceneObject : sceneObjects) {
			Allocator::Delete(sceneObject);
		}

		for (auto renderLayer : renderLayers) {
			Allocator::Delete(renderLayer);
		}

		for (auto shader : shaders) {
			Allocator::Delete(shader);
		}

		for (auto volumeMaterial : materials) {
			Allocator::Delete(volumeMaterial);
		}

		for (auto geometry : geometries) {
			Allocator::Delete(geometry);
		}

		for (auto mesh : meshes) {
			Allocator::Delete(mesh);
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

		// Delete objects:
		for (auto obj : toDelete) {
			DeleteSceneObject(obj);
		}
		toDelete.clear();

		for (auto& dirCaster : dirShadowCasters) {
			dirCaster->Update();
		}
		for (auto& omniCaster : omniDirShadowCasters) {
			omniCaster->Update();
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
		lightManager.ExportData(omniDirShadowCasters);

		// Shadow pass:
		for (auto& group : instanceGroups) {
			group.second->GatherInstanceDataForShadow();
		}
		for (auto& dirCaster : dirShadowCasters) {
			dirCaster->Bind();
			for (auto& group : instanceGroups) {
				group.second->DrawShadow();
			}
		}
		for (auto& omniCaster : omniDirShadowCasters) {
			omniCaster->Bind();
			// TODO
			/*
			for (auto& group : instanceGroups) {
				group.second->DrawShadow();
			}
			*/
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
			lightManager.DrawDebug();
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

		//TODO don't add resource to collection if already there

		{	// Add mesh to collection
			auto mesh = object->GetMesh();
			if (nullptr != mesh) {
				meshes.push_back(mesh);
				auto geom = mesh->getGeometry();
				if (nullptr != geom) {
					geometries.push_back(geom);
				}
				auto mat = mesh->getMaterial();
				if (nullptr != mat) {
					materials.push_back(mat);
					auto shader = mat->GetShaderProgram();
					if (nullptr != shader) {
						shaders.push_back(shader);
					}
					auto ts = mat->GetTextures();
					for (auto texture : ts) {
						textures.push_back(texture);
					}
				}
			}
		}

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
					iter->second->AddObject(object);
				}
				else {
					auto* group = Allocator::New<InstanceGroup>();
					group->AddObject(object);
					instanceGroups.emplace(instanceGroupName, group);
					if (layer->IsInstanced()) {	// Add to layer
						layer->AddInstanceGroup(group);
					}
				}
			}
			else {
				auto* group = Allocator::New<InstanceGroup>();
				group->AddObject(object);
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

	void Scene::RegisterObjectToDelete(SceneObject* object) {
		toDelete.push_back(object);
	}

	void Scene::DeleteSceneObject(SceneObject* object)
	{
		auto objIter = std::ranges::find(sceneObjects.begin(), sceneObjects.end(), object);
		if (sceneObjects.end() != objIter) {
			sceneObjects.erase(objIter);
			for (auto layer : renderLayers) {
				layer->RemoveSceneObject(object);
			}
			Allocator::Delete(object);
		}
	}

	void Scene::AddCollider(Collider* collider, const std::string& colliderGroupName)
	{
		collisionManager.AddCollider(collider, colliderGroupName);
	}

	void Scene::AddPostProcessStage(PostProcessStage* stage, const std::string& renderLayerName) {
		if (renderLayerName.empty()) {	// No layer specified
			stage->AddUniformVariable(&timeSpent);
			(*renderLayers.cend())->AddPostProcessStage(stage);
			postProcessStages.push_back(stage);		// Main PPS container in Scene
			return;
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

	void Scene::AddLight(PointLight* light)
	{	
		lightManager.AddLight(light);
		if (light->IsCastingShadow()) {
			if (MAX_SHADOW_MAP_COUNT - 1 <= omniDirShadowCasters.size()) {	// -1 for the first empty map that is not in this vector
				auto* p = light->GetShadowCaster();
				Allocator::Delete(p);
				light->SetShadowCaster(nullptr);
				DebugUtils::PrintError("Scene", "Trying to add too many omniDir shadow casters to scene!");
			}
			else {
				light->GetShadowCaster()->SetIdx(omniDirShadowCasters.size() + 1);	// +1 because first in shader is the empty map
				omniDirShadowCasters.push_back(light->GetShadowCaster());
				light->GetShadowCaster()->SetPositionProvider(light);
			}
		}
		pointLights.push_back(light);
	}

	void Scene::AddLight(DirectionalLight* light)
	{
		lightManager.AddLight(light);
		if (light->IsCastingShadow()) {
			if (MAX_SHADOW_MAP_COUNT - 1 <= dirShadowCasters.size()) {	// -1 for the first empty map that is not in this vector
				auto* p = light->GetShadowCaster();
				Allocator::Delete(p);
				light->SetShadowCaster(nullptr);
				DebugUtils::PrintError("Scene", "Trying to add too many directional shadow casters to scene!");
			}
			else {
				light->GetShadowCaster()->SetIdx(dirShadowCasters.size() + 1);	// +1 because first in shader is the empty map
				dirShadowCasters.push_back(light->GetShadowCaster());
				light->GetShadowCaster()->SetPositionProvider(&camera);
				light->GetShadowCaster()->SetPositionOffsetToProvider(20.0f * light->GetDirection());
				light->GetShadowCaster()->SetDirection(-1.0f * light->GetDirection());
			}
		}
		dirLights.push_back(light);
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