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

namespace hograengine {

	Scene* Scene::instance = nullptr;

	void Scene::initCameraAndLights()
	{
		camera = new Camera((float)contextWidth / (float)contextHeight, glm::vec3(-10.0f, 10.0f, -10.0f), glm::vec3(-9.0f, 10.0f, -9.0f));
		lights.push_back(new Light(glm::normalize(glm::vec4(-1.0f, 1.0f, -1.0f, 0.0f)), glm::vec3(1.0f, 1.0f, 1.0f)));	// Directional light
		lights.push_back(new Light(glm::vec4(-80.0f, 2.0f, 0.0f, 1.0f), glm::vec3(1000.0f, 100.0f, 100.0f)));
		lights.push_back(new Light(glm::vec4(0.0f, 2.0f, 80.0f, 1.0f), glm::vec3(100.0f, 1000.0f, 100.0f)));
		lights.push_back(new Light(glm::vec4(80.0f, 2.0f, 0.0f, 1.0f), glm::vec3(100.0f, 100.0f, 1000.0f)));
		std::srand(0);
		for (int i = 0; i < 100; i++) {
			lights.push_back(new Light(glm::vec4(std::rand() % 100 - 50, 2.0f, std::rand() % 100 - 50, 1.0f), glm::vec3(5.0f, 5.0f, 5.0f)));
		}
		for (auto& light : lights) {
			lightManager.addLight(light);
		}
		lightManager.initDefferedSystem(contextWidth, contextHeight);
	}

	void Scene::initShadowMap()
	{
		if (shadowCaster != nullptr) {
			throw new std::exception("Shadowcaster already initialised! Only one allowed.");
		}
		shadowCaster = new ShadowCaster(glm::vec3(-20, 20, -20), glm::normalize(glm::vec3(1, -1, 1)));
		textures.push_back(shadowCaster->getShadowMap());
	}

	void Scene::initSceneObjects()
	{
		Texture* cubeMap = nullptr;
		initSkyBox(&cubeMap);
		ForceField* field = initGravitation();
		initGroud(cubeMap);

		auto* col = initCompositeCollider();
		initCube(&cubeMap, glm::vec3(0.0f, 0.0f, 0.0f), col, field);
		col = initCompositeCollider();
		initCube(&cubeMap, glm::vec3(0.0f, 10.0f, 30.0f), col, field);
		col = initCompositeCollider();
		initCube(&cubeMap, glm::vec3(30.0f, 10.0f, 0.0f), col, field);
		col = initCompositeCollider();
		initCube(&cubeMap, glm::vec3(-30.0f, 10.0f, 30.0f), col, field);
		col = initCompositeCollider();
		initCube(&cubeMap, glm::vec3(0.0f, 10.0f, -30.0f), col, field);
		col = initCompositeCollider();
		initCube(&cubeMap, glm::vec3(0.0f, 3.0f, 0.0f), col, field);
		initSphere(&cubeMap, glm::vec3(-20.0f, 3.0f, -20.0f), field);
		initSphere(&cubeMap, glm::vec3(-20.0f, 3.0f, -10.0f), field);
		initSphere(&cubeMap, glm::vec3(-30.0f, 3.0f, -10.0f), field);
		initSphere(&cubeMap, glm::vec3(-10.0f, 3.0f, -20.0f), field);
		for (int i = 0; i < 100; i++) {
			initSphere(&cubeMap, glm::vec3(-10.0f, 3.0f + i * 5.0f, -20.0f), field);
		}
		initLoadedGeometry(&cubeMap, glm::vec3(-10.0f, 3.0f, -30.0f), field);
		initAvatar(field);
		collisionManager.initDebug();
	}

	void Scene::initSkyBox(Texture** cubeMap)
	{
		ShaderProgram* skyboxShader = new ShaderProgram(
			AssetFolderPathManager::getInstance()->getShaderFolderPath().append("fullscreenQuad.vert"),
			"",
			AssetFolderPathManager::getInstance()->getShaderFolderPath().append("skybox.frag")
		);
		std::vector<std::string> imagePaths;
		imagePaths.push_back(AssetFolderPathManager::getInstance()->getTextureFolderPath().append("right.jpg").c_str());
		imagePaths.push_back(AssetFolderPathManager::getInstance()->getTextureFolderPath().append("left.jpg").c_str());
		imagePaths.push_back(AssetFolderPathManager::getInstance()->getTextureFolderPath().append("top.jpg").c_str());
		imagePaths.push_back(AssetFolderPathManager::getInstance()->getTextureFolderPath().append("bottom.jpg").c_str());
		imagePaths.push_back(AssetFolderPathManager::getInstance()->getTextureFolderPath().append("front.jpg").c_str());
		imagePaths.push_back(AssetFolderPathManager::getInstance()->getTextureFolderPath().append("back.jpg").c_str());
		*cubeMap = new TextureCube(imagePaths, SKYBOX_UNIT);
		auto* skyBoxMaterial = new Material(skyboxShader);
		skyBoxMaterial->addTexture(*cubeMap);
		Geometry* fullscreenQuad = GeometryFactory::getInstance()->getFullScreenQuad();
		auto* skyBoxMesh = new Mesh(skyBoxMaterial, fullscreenQuad);
		skyBoxMesh->setDepthTest(false);
		skyBoxMesh->setStencilTest(false);
		addSceneObject(new SceneObject(skyBoxMesh));
	}

	void Scene::initCube(Texture** cubeMap, glm::vec3 pos, Collider* collider, ForceField* field)
	{
		ShaderProgram* cubeShader = ShaderProgramFactory::getInstance()->getDefaultPBRProgramWithMapping();
		auto* material = MaterialFactory::getInstance()->getPBRMaterial("vinyl",*cubeMap);
		Geometry* cubeGeometry = GeometryFactory::getInstance()->getCube();
		auto* cubeMesh = new Mesh(material, cubeGeometry);
		auto* obj = new SceneObject(cubeMesh);
		obj->setPosition(pos);
		obj->setScale(glm::vec3(10.0f, 0.5f, 10.0f));

		auto* cubePhysics = new Physics(obj);
		//cubePhysics->addAppliedForce(glm::vec3(100.0f, 0.0f, 0.0f));
		cubePhysics->setWorldSpaceDrag(glm::vec3(0.5f, 100.5f, 0.5f));
		cubePhysics->setModelSpaceDrag(glm::vec3(0.1f, 0.3f, 0.1f));
		cubePhysics->setMass(3140000.0f);
		//cubePhysics->addAppliedTorque(glm::vec3(0.5f, 0.5f, 0.5f));
		cubePhysics->setMomentOfInertia(Physics::getMomentOfInertiaOfCuboid(cubePhysics->getMass(), obj->getScale()));
		cubePhysics->setRotationalDrag(glm::vec3(2000.0f, 1000.0f, 2000.0f));
		cubePhysics->setPositionForcingLevel(0.5f);
		cubePhysics->setElasticity(0.1f);
		if (field != nullptr) {
			field->addListener(cubePhysics);
		}
		obj->addComponent(cubePhysics);
		collider->setPhysics(cubePhysics);
		collider->setPositionProvider(obj);
		collider->setOrientationProvider(obj);
		obj->addComponent(collider);
		obj->update(0.0f);
		addCollider(collider);
		addSceneObject(obj, "cube");
	}

	void Scene::initSphere(Texture** cubeMap, const glm::vec3& pos, ForceField* field)
	{
		SphericalCollider* collider = new SphericalCollider();
		collider->setRadius(0.5f);
		addCollider(collider);
		ShaderProgram* shader = ShaderProgramFactory::getInstance()->getDefaultPBRProgramWithMapping();
		auto* material = MaterialFactory::getInstance()->getPBRMaterial("planks", *cubeMap);
		Geometry* geometry = GeometryFactory::getInstance()->getSphere();
		geometry->setFaceCulling(false);
		auto* mesh = new Mesh(material, geometry);
		auto* obj = new SceneObject(mesh);
		obj->setPosition(pos);
		obj->setScale(glm::vec3(0.5f, 0.5f, 0.5f));

		auto* physics = new Physics(obj);
		//cubePhysics->addAppliedForce(glm::vec3(100.0f, 0.0f, 0.0f));
		physics->setWorldSpaceDrag(glm::vec3(10.0f, 10.0f, 10.0f));
		physics->setMass(50.0f);
		//cubePhysics->addAppliedTorque(glm::vec3(0.5f, 0.5f, 0.5f));
		physics->setMomentOfInertia(Physics::getMomentOfInertiaOfSolidSphere(physics->getMass(), 0.5f));
		physics->setRotationalDrag(glm::vec3(5.0f, 5.0f, 5.0f));
		physics->setPositionForcingLevel(1.0f);
		physics->setElasticity(0.95f);
		physics->setFriction(1.0f);
		if (field != nullptr) {
			field->addListener(physics);
		}
		obj->addComponent(physics);
		collider->setPhysics(physics);
		collider->setPositionProvider(obj);
		collider->setOrientationProvider(obj);
		obj->addComponent(collider);
		addSceneObject(obj, "sphere");
	}

	void Scene::initFonts()
	{
		auto* shader = ShaderProgramFactory::getInstance()->getGlyphProgram();
		auto* font = new Font(shader);
		font->Load(AssetFolderPathManager::getInstance()->getFontsFolderPath().append("arial.ttf"));
		addFont(font);
	}

	void Scene::initGroud(const Texture* skyBox)
	{
		for (int i = 0; i < 10; i++) {
			for (int j = 0; j < 10; j++) {
				auto* material = MaterialFactory::getInstance()->getPBRMaterial("vinyl", skyBox);
				Geometry* cubeGeometry = GeometryFactory::getInstance()->getCube();
				auto* cubeMesh = new Mesh(material, cubeGeometry);
				auto* obj = new SceneObject(cubeMesh);
				obj->setPosition(glm::vec3(i * 100.0f - 500.0f, 0.0f, j * 100.0f -500.0f));
				obj->setScale(glm::vec3(50.0f, 1.0f, 50.0f));

				auto* cubePhysics = new Physics(obj);
				cubePhysics->setPositionForcingLevel(0.0f);
				AABBCollider* collider = new AABBCollider();
				cubePhysics->setElasticity(0.2f);
				cubePhysics->setFriction(0.9f);
				obj->addComponent(cubePhysics);
				collider->setPhysics(cubePhysics);
				collider->setMinRelToPosition(glm::vec3(-49.99f, -1.0f, -49.99f));
				collider->setMaxRelToPosition(glm::vec3(49.99f, 1.0f, 49.99f));
				collider->setPositionProvider(obj);
				obj->addComponent(collider);
				addCollider(collider, "ground");
				addSceneObject(obj, "ground");
			}
		}
	}

	void Scene::initLoadedGeometry(Texture** cubeMap, const glm::vec3& pos, ForceField* field)
	{
		SphericalCollider* collider = new SphericalCollider();
		collider->setRadius(0.5f);
		addCollider(collider);
		ShaderProgram* shader = ShaderProgramFactory::getInstance()->getDefaultPBRProgramWithMapping();
		auto* material = MaterialFactory::getInstance()->getPBRMaterial("planks", *cubeMap);
		Geometry* geometry = GeometryLoader().load(AssetFolderPathManager::getInstance()->getGeometryFolderPath().append("mango.obj"));
		geometry->setFaceCulling(false);
		auto* mesh = new Mesh(material, geometry);
		auto* obj = new SceneObject(mesh);
		obj->setPosition(pos);
		obj->setScale(glm::vec3(0.5f, 0.5f, 0.5f));
		collider->setPositionProvider(obj);
		collider->setOrientationProvider(obj);
		auto* physics = new Physics(obj);
		//cubePhysics->addAppliedForce(glm::vec3(100.0f, 0.0f, 0.0f));
		physics->setWorldSpaceDrag(glm::vec3(10.0f, 10.0f, 10.0f));
		physics->setMass(50.0f);
		//cubePhysics->addAppliedTorque(glm::vec3(0.5f, 0.5f, 0.5f));
		physics->setMomentOfInertia(Physics::getMomentOfInertiaOfSolidSphere(physics->getMass(), 0.5f));
		physics->setRotationalDrag(glm::vec3(5.0f, 5.0f, 5.0f));
		physics->setPositionForcingLevel(1.0f);
		physics->setElasticity(0.95f);
		physics->setFriction(1.0f);
		if (field != nullptr) {
			field->addListener(physics);
		}
		obj->addComponent(physics);
		collider->setPhysics(physics);
		obj->addComponent(collider);
		addSceneObject(obj);
	}


	void Scene::initAvatar(ForceField* gravitation)
	{
		auto* avatar = new SceneObject();
		avatar->setPosition(glm::vec3(-60.0f, 2.0f, -60.0f));
		camera->setPositionProvider(avatar);
		camera->setPositionInProvidersSpace(glm::vec3(0.0f, 0.8f, 0.0f));
		auto* collider = new AABBCollider();
		collider->setMinRelToPosition(glm::vec3(-0.2f, -1.0f, -0.2f));
		collider->setMaxRelToPosition(glm::vec3(0.2f, 1.0f, 0.2f));
		auto* physics = new Physics(avatar);
		physics->setMass(80.0f);
		physics->setWorldSpaceDrag(glm::vec3(200.0f, 0.01f, 200.0f));
		physics->setPositionForcingLevel(1.0f);
		physics->setFriction(0.001f);
		gravitation->addListener(physics);
		collider->setPhysics(physics);
		collider->setPositionProvider(avatar);
		addCollider(collider, "avatar");
		FirstPersonControl* control = new FirstPersonControl();
		control->setPhysics(physics);
		control->setCamera(camera);
		control->setInitialDirection(glm::normalize(glm::vec3(1, 0, 1)));
		control->setInitialUp(getPreferedUp());
		control->setJumpImpulse(600.0f);
		control->setPropellingForce(glm::vec3(2000.0f, 0.0f, 2000.0f));
		auto* jumpCollider = new AABBCollider();
		jumpCollider->setMinRelToPosition(glm::vec3(-0.2f, -1.1f, -0.2f));
		jumpCollider->setMaxRelToPosition(glm::vec3(0.2f, -0.9f, 0.2f));
		jumpCollider->setPositionProvider(avatar);
		control->setJumpCollider(jumpCollider);
		control->setPositionProvider(avatar);
		control->setOrientationProvider(avatar);
		avatar->addComponent(jumpCollider);
		addCollider(jumpCollider, "avatar");
		userControl = control;
		avatar->addComponent(control);
		avatar->addComponent(physics);
		avatar->addComponent(collider);
		shadowCaster->setPositionProvider(avatar);
		shadowCaster->setPositionOffsetToProvider(glm::vec3(-20, 20, -20));
		addSceneObject(avatar);
	}

	CompositeCollider* Scene::initCompositeCollider()
	{
		// 1, 1 quarter
		CompositeCollider* col = new CompositeCollider();
		auto* subCol = new AABBCollider();
		subCol->setMinRelToPosition(glm::vec3(-2.5f, -0.5f, -2.5f));
		subCol->setMaxRelToPosition(glm::vec3(2.5f, 0.5f, 2.5f));
		col->addSubCollider(subCol, glm::vec3(2.5f, 0.0f, 2.5f));
		subCol = new AABBCollider();
		subCol->setMinRelToPosition(glm::vec3(-2.5f, -0.5f, -2.5f));
		subCol->setMaxRelToPosition(glm::vec3(2.5f, 0.5f, 2.5f));
		col->addSubCollider(subCol, glm::vec3(7.5f, 0.0f, 2.5f));
		subCol = new AABBCollider();
		subCol->setMinRelToPosition(glm::vec3(-2.5f, -0.5f, -2.5f));
		subCol->setMaxRelToPosition(glm::vec3(2.5f, 0.5f, 2.5f));
		col->addSubCollider(subCol, glm::vec3(7.5f, 0.0f, 7.5f));
		subCol = new AABBCollider();
		subCol->setMinRelToPosition(glm::vec3(-2.5f, -0.5f, -2.5f));
		subCol->setMaxRelToPosition(glm::vec3(2.5f, 0.5f, 2.5f));
		col->addSubCollider(subCol, glm::vec3(2.5f, 0.0f, 7.5f));

		// -1, 1 quarter
		subCol = new AABBCollider();
		subCol->setMinRelToPosition(glm::vec3(-2.5f, -0.5f, -2.5f));
		subCol->setMaxRelToPosition(glm::vec3(2.5f, 0.5f, 2.5f));
		col->addSubCollider(subCol, glm::vec3(-2.5f, 0.0f, 2.5f));
		subCol = new AABBCollider();
		subCol->setMinRelToPosition(glm::vec3(-2.5f, -0.5f, -2.5f));
		subCol->setMaxRelToPosition(glm::vec3(2.5f, 0.5f, 2.5f));
		col->addSubCollider(subCol, glm::vec3(-7.5f, 0.0f, 2.5f));
		subCol = new AABBCollider();
		subCol->setMinRelToPosition(glm::vec3(-2.5f, -0.5f, -2.5f));
		subCol->setMaxRelToPosition(glm::vec3(2.5f, 0.5f, 2.5f));
		col->addSubCollider(subCol, glm::vec3(-2.5f, 0.0f, 7.5f));
		subCol = new AABBCollider();
		subCol->setMinRelToPosition(glm::vec3(-2.5f, -0.5f, -2.5f));
		subCol->setMaxRelToPosition(glm::vec3(2.5f, 0.5f, 2.5f));
		col->addSubCollider(subCol, glm::vec3(-7.5f, 0.0f, 7.5f));

		// 1, -1 quarter
		subCol = new AABBCollider();
		subCol->setMinRelToPosition(glm::vec3(-2.5f, -0.5f, -2.5f));
		subCol->setMaxRelToPosition(glm::vec3(2.5f, 0.5f, 2.5f));
		col->addSubCollider(subCol, glm::vec3(2.5f, 0.0f, -2.5f));
		subCol = new AABBCollider();
		subCol->setMinRelToPosition(glm::vec3(-2.5f, -0.5f, -2.5f));
		subCol->setMaxRelToPosition(glm::vec3(2.5f, 0.5f, 2.5f));
		col->addSubCollider(subCol, glm::vec3(7.5f, 0.0f, -2.5f));
		subCol = new AABBCollider();
		subCol->setMinRelToPosition(glm::vec3(-2.5f, -0.5f, -2.5f));
		subCol->setMaxRelToPosition(glm::vec3(2.5f, 0.5f, 2.5f));
		col->addSubCollider(subCol, glm::vec3(2.5f, 0.0f, -7.5f));
		subCol = new AABBCollider();
		subCol->setMinRelToPosition(glm::vec3(-2.5f, -0.5f, -2.5f));
		subCol->setMaxRelToPosition(glm::vec3(2.5f, 0.5f, 2.5f));
		col->addSubCollider(subCol, glm::vec3(7.5f, 0.0f, -7.5f));

		// -1, -1 quarter
		subCol = new AABBCollider();
		subCol->setMinRelToPosition(glm::vec3(-2.5f, -0.5f, -2.5f));
		subCol->setMaxRelToPosition(glm::vec3(2.5f, 0.5f, 2.5f));
		col->addSubCollider(subCol, glm::vec3(-2.5f, 0.0f, -2.5f));
		subCol = new AABBCollider();
		subCol->setMinRelToPosition(glm::vec3(-2.5f, -0.5f, -2.5f));
		subCol->setMaxRelToPosition(glm::vec3(2.5f, 0.5f, 2.5f));
		col->addSubCollider(subCol, glm::vec3(-7.5f, 0.0f, -2.5f));
		subCol = new AABBCollider();
		subCol->setMinRelToPosition(glm::vec3(-2.5f, -0.5f, -2.5f));
		subCol->setMaxRelToPosition(glm::vec3(2.5f, 0.5f, 2.5f));
		col->addSubCollider(subCol, glm::vec3(-2.5f, 0.0f, -7.5f));
		subCol = new AABBCollider();
		subCol->setMinRelToPosition(glm::vec3(-2.5f, -0.5f, -2.5f));
		subCol->setMaxRelToPosition(glm::vec3(2.5f, 0.5f, 2.5f));
		col->addSubCollider(subCol, glm::vec3(-7.5f, 0.0f, -7.5f));

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
		auto* stage0 = new PostProcessStage(AssetFolderPathManager::getInstance()->getShaderFolderPath().append("hdr.frag"),
			contextWidth, contextHeight);
		postProcessStages.push_back(stage0);
		//auto* stage1 = new PostProcessStage(AssetFolderPathManager::getInstance()->getShaderFolderPath().append("edgeDetect.frag"),
		//	contextWidth, contextHeight);
		//postProcessStages.push_back(stage1);
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

		//TODO: Collide

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
			instance->initShadowMap();
			instance->initCameraAndLights();
			instance->initSceneObjects();
			instance->initPostProcessStages();
			instance->initFonts();
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

		if (shadowCaster != nullptr) {
			delete shadowCaster;
		}

		instanceGroups.clear();

		for (auto& font : fonts) {
			delete font;
		}
		fonts.clear();
	}

	//-----------------------------------------------------------------------------

	void Scene::control(float dt)
	{
		collisionManager.collide();

		ControlActionManager::getInstance()->executeQueue(this, dt);
		SceneEventManager::getInstance()->executeQueue(dt);
		for (auto& obj : sceneObjects) {
			obj->control(dt);
		}
	}

	void Scene::update(float dt)
	{
		for (auto& obj : sceneObjects) {
			obj->update(dt);
		}

		camera->update(dt);
		if (shadowCaster != nullptr) {
			shadowCaster->update();
		}
		collisionManager.update();
	}

	void Scene::draw()
	{
		// Init and export data:
		for (auto& group : instanceGroups) {
			group.second->gatherInstanceData();
		}
		camera->exportData();
		lightManager.exportData();

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
			group.second->draw();
		}

		// Deferred lighting pass:
		if (postProcessStages.size() > 0) {
			postProcessStages[0]->Bind();
		}
		else {
			FBO::BindDefault();
		}
		glClearColor(backgroundColor.x, backgroundColor.y, backgroundColor.z, backgroundColor.w);
		glClearDepth(1);
		glDepthFunc(GL_LESS);
		glFrontFace(GL_CCW);
		glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
		glStencilMask(0x00);

		lightManager.renderDeferredLighting();

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
			collisionManager.drawDebug();
			fonts[0]->RenderText("Debug mode", contextWidth / 2, contextHeight * 0.9f, 1.0f, glm::vec3(0, 1, 0));
			fonts[0]->RenderText("A nevem Simon Zoltán és ez UTF-8 kódolású. Ügyes!", contextWidth / 2, contextHeight * 0.8f, 1.0f, glm::vec3(0, 1, 0));
		}
	}

	void Scene::addSceneObject(SceneObject* object, const std::string& instanceGroupName)
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

	void Scene::addCollider(Collider* collider, const std::string& colliderGroupName)
	{
		collisionManager.addCollider(collider, colliderGroupName);
	}

	void Scene::addPostProcessStage(PostProcessStage* stage) {
		postProcessStages.push_back(stage);
	}

	void Scene::setCamera(Camera* _camera) {
		camera = _camera;
	}

	void Scene::addLight(Light* light)
	{
		lightManager.addLight(light);
	}


	void Scene::addFont(Font* font)
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

	void Scene::setUserControl(UserControl* uc) {
		userControl = uc;
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
			camera->setAspectRatio((float)_contextWidth / (float)_contextHeight);
		}
		for (auto& stage : postProcessStages) {
			stage->resize(_contextWidth, _contextHeight);
		}
		camera->setMoved(true);
		lightManager.onResize(_contextWidth, _contextHeight);
	}

	void Scene::serialize()
	{
		//TODO
	}
}