#include "Scene.h"
#include "GlobalInclude.h"
#include "ControlActionManager.h"
#include "AssetFolderPathManager.h"
#include "TextureCube.h"
#include "GeometryFactory.h"
#include "AudioSource.h"
#include "Listener.h"
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
			throw new std::exception("Shadowcaster already initialised! Only one allowed.");
		}
		shadowCaster = new ShadowCaster(glm::vec3(-20, 20, -20), glm::normalize(glm::vec3(1, -1, 1)));
		textures.push_back(shadowCaster->getShadowMap());
	}

	void Scene::initPostProcessStages()
	{
		auto* stage0 = new PostProcessStage(AssetFolderPathManager::getInstance()->getShaderFolderPath().append("hdr.frag"),
			contextWidth, contextHeight);
		postProcessStages.push_back(stage0);
		//auto* stage1 = new PostProcessStage(AssetFolderPathManager::getInstance()->getShaderFolderPath().append("edgeDetect.frag"),
		//	contextWidth, contextHeight);
		//postProcessStages.push_back(stage1);
	}

	void Scene::PokeObject(const glm::vec2& ndcCoords)
	{
		glm::vec4 wDir = camera.getRayDirMatrix() * glm::vec4(ndcCoords, 0.0, 1.0f);
		wDir /= wDir.w;
		glm::vec3 dir = glm::normalize(glm::vec3(wDir));
		glm::vec3 intersectionPoint;
		glm::vec3 intersectionNormal;
		Ray ray;
		ray.SetPosition(camera.getEyePos());
		ray.setDirection(dir);
		float minT = -1.0f;
		Collider* selected = nullptr;

		//TODO: Collide

		if (nullptr != selected) {
			selected->GetPhysics()->applyImpulse(dir * 100.0f, intersectionPoint - selected->GetPhysics()->getOwnerPosition());
		}
	}

	void Scene::Init(int contextWidth, int contextHeight)
	{
		this->contextWidth = contextWidth;
		this->contextHeight = contextHeight;
		initShadowMap();
		camera.Init((float)contextWidth / (float)contextHeight, glm::vec3(-10.0f, 10.0f, -10.0f), glm::vec3(-9.0f, 10.0f, -9.0f));
		lightManager.initDefferedSystem(contextWidth, contextHeight);
		lightManager.initDebug();
		bloom.Init(contextWidth, contextHeight);
		initPostProcessStages();
		collisionManager.InitDebug();
	}

	void Scene::Destroy()
	{
		for (auto* light : lights) {
			delete light;
		}
		lights.clear();
		lightManager.Clear();

		for (auto sh : shaders) {
			sh->Delete();
			delete sh;
		}
		shaders.clear();
		for (auto& material : materials) {
			delete material;
		}
		materials.clear();

		for (auto& geometry : geometries) {
			delete geometry;
		}
		geometries.clear();

		for (auto& mesh : meshes) {
			delete mesh;
		}
		meshes.clear();

		for (auto& obj : sceneObjects) {
			delete obj;
		}
		sceneObjects.clear();

		for (auto texture : textures) {
			delete texture;
		}
		textures.clear();

		for (auto& postProcStage : postProcessStages) {
			delete postProcStage;
		}
		postProcessStages.clear();

		for (auto& component : components) {
			delete component;
		}
		components.clear();

		if (shadowCaster != nullptr) {
			delete shadowCaster;
		}

		instanceGroups.clear();

		for (auto& font : fonts) {
			delete font;
		}
		fonts.clear();
		for (auto& caption : captions) {
			delete caption;
		}
		captions.clear();
	}

	//-----------------------------------------------------------------------------

	void Scene::Control(float dt)
	{
		collisionManager.Collide();

		ControlActionManager::getInstance()->executeQueue(this, dt);
		SceneEventManager::getInstance()->executeQueue(dt);
		for (auto& obj : sceneObjects) {
			obj->Control(dt);
		}
	}

	void Scene::Update(float dt)
	{
		for (auto& obj : sceneObjects) {
			obj->Update(dt, camera);
		}

		camera.Update(dt);
		if (shadowCaster != nullptr) {
			shadowCaster->Update();
		}
		collisionManager.Update();
		for (auto& group : instanceGroups) {
			group.second->optimalize(camera);
		}
	}

	void Scene::Draw()
	{
		// Init and export data:
		for (auto& group : instanceGroups) {
			group.second->gatherInstanceData();
		}
		camera.ExportData();
		lightManager.ExportData();

		// Shadow pass:
		if (nullptr != shadowCaster) {
			shadowCaster->Bind();
			for (auto& group : instanceGroups) {
				group.second->drawShadow();
			}
		}

		// Geometry pass:
		lightManager.BindGBuffer();
		for (auto& group : instanceGroups) {
			group.second->Draw();
		}
		bloom.Bind();
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

		bloom.Draw(postProcessStages[0]->getFBO());

		// Post-process pass:
		for (int i = 0; i < postProcessStages.size(); i++) {
			if (i < postProcessStages.size() - 1) {
				postProcessStages[i]->Draw(postProcessStages[i + 1]->getFBO());
			}
			else {
				postProcessStages[i]->Draw(FBO::getDefault());
			}
		}
		if (drawDebug) {
			collisionManager.DrawDebug();
			lightManager.drawDebug();
			for (auto* caption : captions) {
				caption->Draw();
			}
		}
	}

	void Scene::AddSceneObject(SceneObject* object, const std::string& instanceGroupName)
	{
		if (auto objectIter = std::find(sceneObjects.begin(), sceneObjects.end(), object); objectIter != sceneObjects.end()) {		// If already contains
			return;
		}

		Mesh* mesh = object->getMesh();
		Material* material = nullptr;
		Geometry* geometry = nullptr;
		if (mesh != nullptr) {
			material = mesh->getMaterial();
			geometry = mesh->getGeometry();
			ShaderProgram* program = material->getShaderProgram();
			auto _textures = material->getTextures();

			auto meshIter = std::find(meshes.begin(), meshes.end(), mesh);
			if (meshIter == meshes.end()) {	// If does not contain
				meshes.push_back(mesh);
			}

			if (auto materialIter = std::find(materials.begin(), materials.end(), material); materialIter == materials.end()) { // If does not contain
				materials.push_back(material);
			}

			if (auto geometryIter = std::find(geometries.begin(), geometries.end(), geometry); geometryIter == geometries.end()) { // If does not contain
				geometries.push_back(geometry);
			}
			if (auto programIter = std::find(shaders.begin(), shaders.end(), program); programIter == shaders.end()) { // If does not contain
				shaders.push_back(program);
			}

			for (auto* texture : _textures) {
				auto textureIter = std::find(this->textures.begin(), this->textures.end(), texture);
				if (textureIter == this->textures.end()) { // If does not contain
					this->textures.push_back(texture);
				}
			}
		}
		auto objComponents = object->getComponents();

		for (auto* comp : objComponents) {
			auto compIter = std::find(this->components.begin(), this->components.end(), comp);
			if (compIter == this->components.end()) { // If does not contain
				this->components.push_back(comp);
			}
		}

		sceneObjects.push_back(object);

		static int defaultName = 0;
		if (mesh != nullptr) {
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


	void Scene::AddFont(Font* font)
	{
		if (!fonts.empty() && std::find(fonts.begin(), fonts.end(), font) != fonts.end()) {
			return;
		}
		auto* shader = font->getShaderProgram();
		auto shaderIter = std::find(shaders.begin(), shaders.end(), shader);
		if (shader != nullptr && shaderIter == shaders.end()) {
			shaders.push_back(shader);
		}
		fonts.push_back(font);
	}

	void Scene::AddCaption(Caption* caption)
	{
		if (!captions.empty() && std::find(captions.begin(), captions.end(), caption) != captions.end()) {
			return;
		}
		auto* shader = caption->getShaderProgram();
		auto shaderIter = std::find(shaders.begin(), shaders.end(), shader);
		if (shader != nullptr && shaderIter == shaders.end()) {
			shaders.push_back(shader);
		}
		captions.push_back(caption);
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
			stage->onResize(_contextWidth, _contextHeight);
		}
		camera.setMoved(true);
		lightManager.onResize(_contextWidth, _contextHeight);
		bloom.onResize(_contextWidth, _contextHeight);
	}

	void Scene::Serialize()
	{
		//TODO
	}
}