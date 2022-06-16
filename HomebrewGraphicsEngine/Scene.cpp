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

namespace hograengine {

	Scene* Scene::instance = nullptr;

	void Scene::initCameraAndLights()
	{
		camera = new Camera(contextWidth, contextHeight, glm::vec3(-10.0f, 10.0f, -10.0f), glm::vec3(0.0f, 0.0f, 0.0f));
		lights.push_back(new Light(glm::normalize(glm::vec4(-1.0f, 1.0f, -1.0f, 0.0f)), glm::vec3(0.8f, 0.8f, 0.8f)));	// Directional light
		lights.push_back(new Light(glm::vec4(10.0f, 5.0f, 20.0f, 1.0f), glm::vec3(100.0f, 100.0f, 100.0f)));
	}

	void Scene::initSceneObjects()
	{
		Texture* cubeMap = nullptr;
		initSkyBox(&cubeMap);
		ForceField* field = initGravitation();
		initGroud();

		auto* col = initCompositeCollider();
		initCube(&cubeMap, glm::vec3(0.0f, 0.0f, 0.0f), col, field);
		AABBCollider* col2 = new AABBCollider();
		col = initCompositeCollider();
		initCube(&cubeMap, glm::vec3(0.0f, 0.0f, 30.0f), col, field);
		col = initCompositeCollider();
		initCube(&cubeMap, glm::vec3(30.0f, 0.0f, 0.0f), col, field);
		col = initCompositeCollider();
		initCube(&cubeMap, glm::vec3(-30.0f, 0.0f, 30.0f), col, field);
		col = initCompositeCollider();
		initCube(&cubeMap, glm::vec3(0.0f, 0.0f, -30.0f), col, field);
		col = initCompositeCollider();
		initCube(&cubeMap, glm::vec3(0.0f, -30.0f, 0.0f), col, field);
	}

	void Scene::initSkyBox(Texture** cubeMap)
	{
		ShaderProgram* skyboxShader = new ShaderProgram(
			AssetFolderPathManager::getInstance()->getShaderFolderPath().append("fullscreenQuad.vert"),
			AssetFolderPathManager::getInstance()->getShaderFolderPath().append("skybox.frag")
		);
		std::vector<std::string> imagePaths;
		imagePaths.push_back(AssetFolderPathManager::getInstance()->getTextureFolderPath().append("right.jpg").c_str());
		imagePaths.push_back(AssetFolderPathManager::getInstance()->getTextureFolderPath().append("left.jpg").c_str());
		imagePaths.push_back(AssetFolderPathManager::getInstance()->getTextureFolderPath().append("top.jpg").c_str());
		imagePaths.push_back(AssetFolderPathManager::getInstance()->getTextureFolderPath().append("bottom.jpg").c_str());
		imagePaths.push_back(AssetFolderPathManager::getInstance()->getTextureFolderPath().append("front.jpg").c_str());
		imagePaths.push_back(AssetFolderPathManager::getInstance()->getTextureFolderPath().append("back.jpg").c_str());
		*cubeMap = new TextureCube(imagePaths, 5);
		auto* skyBoxMaterial = new Material(skyboxShader);
		skyBoxMaterial->addTexture(*cubeMap);
		Geometry* fullscreenQuad = GeometryFactory::FullScreenQuad::getInstance();
		auto* skyBoxMesh = new Mesh(skyBoxMaterial, fullscreenQuad);
		skyBoxMesh->setDepthTest(false);
		skyBoxMesh->setStencilTest(false);
		addSceneObject(new SceneObject(skyBoxMesh));
	}

	void Scene::initCube(Texture** cubeMap, glm::vec3 pos, Collider* collider, ForceField* field)
	{
		ShaderProgram* cubeShader = new ShaderProgram(
			AssetFolderPathManager::getInstance()->getShaderFolderPath().append("default.vert"),
			AssetFolderPathManager::getInstance()->getShaderFolderPath().append("default.frag")
		);
		auto* cubeMaterial = new Material(cubeShader);
		cubeMaterial->addTexture(*cubeMap);
		cubeMaterial->setReflectiveness(0.3f);
		Geometry* cubeGeometry = GeometryFactory::Cube::getInstance();
		auto* cubeMesh = new Mesh(cubeMaterial, cubeGeometry);
		auto* obj = new SceneObject(cubeMesh);
		obj->setPosition(pos);
		obj->setScale(glm::vec3(10.0f, 0.5f, 10.0f));

		auto* cubePhysics = new Physics(obj);
		//cubePhysics->addAppliedForce(glm::vec3(100.0f, 0.0f, 0.0f));
		cubePhysics->setWorldSpaceDrag(glm::vec3(0.5f, 0.5f, 0.5f));
		cubePhysics->setModelSpaceDrag(glm::vec3(0.1f, 0.3f, 0.1f));
		cubePhysics->setMass(100.0f);
		//cubePhysics->addAppliedTorque(glm::vec3(0.5f, 0.5f, 0.5f));
		cubePhysics->setMomentOfInertia(Physics::getMomentOfInertiaOfCuboid(cubePhysics->getMass(), obj->getScale()));
		cubePhysics->setRotationalDrag(glm::vec3(2000.0f, 1000.0f, 2000.0f));
		cubePhysics->setPositionForcingLevel(1.0f);
		if (field != nullptr) {
			field->addListener(cubePhysics);
		}
		obj->addComponent(cubePhysics);
		collider->setPhysics(cubePhysics);
		collider->setElasticity(1.0f);
		obj->addComponent(collider);
		colliders.push_back(collider);
		addSceneObject(obj);
	}

	void Scene::initGroud()
	{
		ShaderProgram* cubeShader = new ShaderProgram(
			AssetFolderPathManager::getInstance()->getShaderFolderPath().append("default.vert"),
			AssetFolderPathManager::getInstance()->getShaderFolderPath().append("default.frag")
		);
		auto* cubeMaterial = new Material(cubeShader);
		cubeMaterial->setReflectiveness(0.0f);
		Geometry* cubeGeometry = GeometryFactory::Cube::getInstance();
		auto* cubeMesh = new Mesh(cubeMaterial, cubeGeometry);
		auto* obj = new SceneObject(cubeMesh);
		obj->setPosition(glm::vec3(0.0f, -50.0f, 0.0f));
		obj->setScale(glm::vec3(100.0f, 1.0f, 100.0f));

		auto* cubePhysics = new Physics(obj);
		cubePhysics->setPositionForcingLevel(0.0f);
		AABBCollider* collider = new AABBCollider();
		collider->setElasticity(0.2f);
		obj->addComponent(cubePhysics);
		collider->setPhysics(cubePhysics);
		collider->setMinInOrigo(glm::vec3(-100.0f, -1.0f, -100.0f));
		collider->setMaxInOrigo(glm::vec3(100.0f, 1.0f, 100.0f));
		obj->addComponent(collider);
		colliders.push_back(collider);
		addSceneObject(obj);
	}

	CompositeCollider* Scene::initCompositeCollider()
	{
		// 1, 1 quarter
		CompositeCollider* col = new CompositeCollider();
		auto* subCol = new AABBCollider();
		subCol->setMinInOrigo(glm::vec3(-2.5f, -0.5f, -2.5f));
		subCol->setMaxInOrigo(glm::vec3(2.5f, 0.5f, 2.5f));
		col->addSubCollider(subCol, glm::vec3(5.0f, 0.0f, 5.0f));
		subCol = new AABBCollider();
		subCol->setMinInOrigo(glm::vec3(-2.5f, -0.5f, -2.5f));
		subCol->setMaxInOrigo(glm::vec3(2.5f, 0.5f, 2.5f));
		col->addSubCollider(subCol, glm::vec3(10.0f, 0.0f, 5.0f));
		subCol = new AABBCollider();
		subCol->setMinInOrigo(glm::vec3(-2.5f, -0.5f, -2.5f));
		subCol->setMaxInOrigo(glm::vec3(2.5f, 0.5f, 2.5f));
		col->addSubCollider(subCol, glm::vec3(10.0f, 0.0f, 10.0f));
		subCol = new AABBCollider();
		subCol->setMinInOrigo(glm::vec3(-2.5f, -0.5f, -2.5f));
		subCol->setMaxInOrigo(glm::vec3(2.5f, 0.5f, 2.5f));
		col->addSubCollider(subCol, glm::vec3(5.0f, 0.0f, 10.0f));

		// -1, 1 quarter
		subCol = new AABBCollider();
		subCol->setMinInOrigo(glm::vec3(-2.5f, -0.5f, -2.5f));
		subCol->setMaxInOrigo(glm::vec3(2.5f, 0.5f, 2.5f));
		col->addSubCollider(subCol, glm::vec3(-5.0f, 0.0f, 5.0f));
		subCol = new AABBCollider();
		subCol->setMinInOrigo(glm::vec3(-2.5f, -0.5f, -2.5f));
		subCol->setMaxInOrigo(glm::vec3(2.5f, 0.5f, 2.5f));
		col->addSubCollider(subCol, glm::vec3(-10.0f, 0.0f, 5.0f));
		subCol = new AABBCollider();
		subCol->setMinInOrigo(glm::vec3(-2.5f, -0.5f, -2.5f));
		subCol->setMaxInOrigo(glm::vec3(2.5f, 0.5f, 2.5f));
		col->addSubCollider(subCol, glm::vec3(-5.0f, 0.0f, 10.0f));
		subCol = new AABBCollider();
		subCol->setMinInOrigo(glm::vec3(-2.5f, -0.5f, -2.5f));
		subCol->setMaxInOrigo(glm::vec3(2.5f, 0.5f, 2.5f));
		col->addSubCollider(subCol, glm::vec3(-10.0f, 0.0f, 10.0f));

		// 1, -1 quarter
		subCol = new AABBCollider();
		subCol->setMinInOrigo(glm::vec3(-2.5f, -0.5f, -2.5f));
		subCol->setMaxInOrigo(glm::vec3(2.5f, 0.5f, 2.5f));
		col->addSubCollider(subCol, glm::vec3(5.0f, 0.0f, -5.0f));
		subCol = new AABBCollider();
		subCol->setMinInOrigo(glm::vec3(-2.5f, -0.5f, -2.5f));
		subCol->setMaxInOrigo(glm::vec3(2.5f, 0.5f, 2.5f));
		col->addSubCollider(subCol, glm::vec3(10.0f, 0.0f, -5.0f));
		subCol = new AABBCollider();
		subCol->setMinInOrigo(glm::vec3(-2.5f, -0.5f, -2.5f));
		subCol->setMaxInOrigo(glm::vec3(2.5f, 0.5f, 2.5f));
		col->addSubCollider(subCol, glm::vec3(5.0f, 0.0f, -10.0f));
		subCol = new AABBCollider();
		subCol->setMinInOrigo(glm::vec3(-2.5f, -0.5f, -2.5f));
		subCol->setMaxInOrigo(glm::vec3(2.5f, 0.5f, 2.5f));
		col->addSubCollider(subCol, glm::vec3(10.0f, 0.0f, -10.0f));

		// -1, -1 quarter
		subCol = new AABBCollider();
		subCol->setMinInOrigo(glm::vec3(-2.5f, -0.5f, -2.5f));
		subCol->setMaxInOrigo(glm::vec3(2.5f, 0.5f, 2.5f));
		col->addSubCollider(subCol, glm::vec3(-5.0f, 0.0f, -5.0f));
		subCol = new AABBCollider();
		subCol->setMinInOrigo(glm::vec3(-2.5f, -0.5f, -2.5f));
		subCol->setMaxInOrigo(glm::vec3(2.5f, 0.5f, 2.5f));
		col->addSubCollider(subCol, glm::vec3(-10.0f, 0.0f, -5.0f));
		subCol = new AABBCollider();
		subCol->setMinInOrigo(glm::vec3(-2.5f, -0.5f, -2.5f));
		subCol->setMaxInOrigo(glm::vec3(2.5f, 0.5f, 2.5f));
		col->addSubCollider(subCol, glm::vec3(-5.0f, 0.0f, -10.0f));
		subCol = new AABBCollider();
		subCol->setMinInOrigo(glm::vec3(-2.5f, -0.5f, -2.5f));
		subCol->setMaxInOrigo(glm::vec3(2.5f, 0.5f, 2.5f));
		col->addSubCollider(subCol, glm::vec3(-10.0f, 0.0f, -10.0f));

		return col;
	}

	ForceField* Scene::initGravitation()
	{
		SceneObject* obj = new SceneObject();
		auto* field = new HomogeneForceField();
		field->setStrength(9.8f);

		obj->addComponent(field);
		addSceneObject(obj);
		return field;
	}

	void Scene::initPostProcessStages()
	{
		auto* stage = new PostProcessStage(AssetFolderPathManager::getInstance()->getShaderFolderPath().append("edgeDetect.frag"),
			contextWidth, contextHeight);
		postProcessStages.push_back(stage);
	}

	void Scene::pokeObject(const glm::vec2& ndcCoords)
	{
		glm::vec4 wDir = camera->getRayDirMatrix() * glm::vec4(ndcCoords, 0.0, 1.0f);
		wDir /= wDir.w;
		glm::vec3 dir = glm::normalize(glm::vec3(wDir));
		glm::vec3 intersectionPoint;
		glm::vec3 intersectionNormal;
		Ray ray;
		ray.setPosition(camera->getEyePos());
		ray.setDirection(dir);
		float minT = -1.0f;
		Collider* selected = nullptr;
		for (auto* collider : colliders) {
			if (collider->testRayIntersection(ray, intersectionPoint, intersectionNormal)) {
				float t = glm::length(intersectionPoint - ray.getPosition());
				if (t < minT || minT < 0.0f) {
					selected = collider;
					minT = t;
				}
			}
		}
		if (nullptr != selected) {
			selected->getPhysics()->applyImpulse(dir * 100.0f, intersectionPoint - selected->getPhysics()->getOwnerPosition());
		}
	}

	Scene* Scene::getInstance()
	{
		if (instance == nullptr) {
			throw new SceneNotInstanciatedException();
		}
		return instance;
	}

	void Scene::destroyInstance()
	{
		if (instance != nullptr) {
			delete instance;
			instance = nullptr;
		}
	}


	Scene* Scene::init(int contextWidth, int contextHeight)
	{
		if (instance == nullptr) {
			instance = new Scene(contextWidth, contextHeight);
			instance->initCameraAndLights();
			instance->initSceneObjects();
			instance->initPostProcessStages();
		}
		return instance;
	}

	void Scene::destroy()
	{
		if (camera != nullptr) {
			delete camera;
			camera = nullptr;
		}

		lights.clear();

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

		colliders.clear(); // The colliders have been deleted as componenets!!!
	}

	//-----------------------------------------------------------------------------

	void Scene::control(float dt)
	{
		ControlActionManager::getInstance()->executeQueue(this, dt);
		SceneEventManager::getInstance()->executeQueue(dt);
		for (auto& obj : sceneObjects) {
			obj->control(dt);
		}

		for (int i = 0; i < colliders.size() - 1; i++) {
			for (int j = i + 1; j < colliders.size(); j++) {
				colliders[i]->collide(*colliders[j]);
			}
		}
	}

	void Scene::update(float dt)
	{
		for (auto& obj : sceneObjects) {
			obj->update(dt);
		}

		camera->update();
	}

	void Scene::draw()
	{
		if (postProcessStages.size() > 0) {
			postProcessStages[0]->Bind();
		}
		glClearColor(backgroundColor.x, backgroundColor.y, backgroundColor.z, backgroundColor.w);
		glClearDepth(1);
		glDepthFunc(GL_LESS);
		glFrontFace(GL_CCW);
		glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
		glStencilMask(0x00);

		for (auto& obj : sceneObjects) {
			obj->draw(*camera, lights);
		}
		for (int i = 0; i < postProcessStages.size(); i++) {
			if (i < postProcessStages.size() - 1) {
				postProcessStages[i]->Draw(postProcessStages[i + 1]->getFBO());
			}
			else {
				postProcessStages[i]->Draw(FBO::getDefault());
			}
		}
	}

	void Scene::addSceneObject(SceneObject* object)
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
			const std::vector<Texture*>& _textures = material->getTextures();

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
	}

	const glm::vec3& Scene::getPreferedUp() const {
		return preferedUp;
	}

	void Scene::togglePause()
	{
		pause = !pause;
	}

	Camera* Scene::getCamera() {
		return camera;
	}

	void Scene::onContextResize(int _contextWidth, int _contextHeight)
	{
		this->contextWidth = _contextWidth;
		this->contextHeight = _contextHeight;
		if (camera != nullptr) {
			camera->width = _contextWidth;
			camera->height = _contextHeight;
		}
		for (auto& stage : postProcessStages) {
			stage->resize(_contextWidth, _contextHeight);
		}
		camera->moved = true;
	}

	void Scene::serialize()
	{
		//TODO
	}
}