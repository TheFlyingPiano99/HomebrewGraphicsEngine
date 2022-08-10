#include "SceneFactory.h"
#include "MaterialFactory.h"
#include "SphericalCollider.h"
#include "CompositeCollider.h"
#include "ShaderProgramFactory.h"
#include <glm/ext/vector_float3.hpp>
#include <glm/ext/vector_float4.hpp>
#include "GeometryLoader.h"
#include "ShaderProgramFactory.h"

namespace Hogra {
	SceneFactory* SceneFactory::instance = nullptr;

	SceneFactory* SceneFactory::getInstance() {
		if (nullptr == instance) {
			instance = new SceneFactory();
		}
		return instance;
	}
	
	void SceneFactory::DestroyInstance() {
		if (nullptr != instance) {
			delete instance;
			instance = nullptr;
		}
	}
	Scene* SceneFactory::CreateDemoScene(int contextWidth, int contextHeight) {
		Scene* scene = new Scene();
		scene->Init(contextWidth, contextHeight);
		auto* light = Light::Instantiate();
		light->Init(glm::normalize(glm::vec4(-1.0f, 1.0f, -1.0f, 0.0f)), glm::vec3(1.0f, 1.0f, 1.0f));
		scene->AddLight(light);	// Directional light
		light = Light::Instantiate();
		light->Init(glm::vec4(-80.0f, 2.0f, 0.0f, 1.0f), glm::vec3(250.0f, 50.0f, 50.0f));
		scene->AddLight(light);
		light = Light::Instantiate();
		light->Init(glm::vec4(0.0f, 2.0f, 80.0f, 1.0f), glm::vec3(50.0f, 250.0f, 50.0f));
		scene->AddLight(light);
		light = Light::Instantiate();
		light->Init(glm::vec4(80.0f, 2.0f, 0.0f, 1.0f), glm::vec3(50.0f, 50.0f, 250.0f));
		scene->AddLight(light);
		std::srand(0);
		for (int i = 0; i < 10; i++) {
			auto* instance = Light::Instantiate();
			instance->Init(glm::vec4(std::rand() % 1000 - 500, 2.0f, std::rand() % 1000 - 500, 1.0f), glm::vec3(5.0f, 5.0f, 5.0f));
			scene->AddLight(instance);
		}

		ForceField* field = InitGravitation(scene);
		InitGroud(scene);
		auto* col = InitCompositeCollider();
		InitCube(scene, glm::vec3(0.0f, 0.0f, 0.0f), col, field);
		col = InitCompositeCollider();
		InitCube(scene, glm::vec3(0.0f, 10.0f, 30.0f), col, field);
		col = InitCompositeCollider();
		InitCube(scene, glm::vec3(30.0f, 10.0f, 0.0f), col, field);
		col = InitCompositeCollider();
		InitCube(scene, glm::vec3(-30.0f, 10.0f, 30.0f), col, field);
		col = InitCompositeCollider();
		InitCube(scene, glm::vec3(0.0f, 10.0f, -30.0f), col, field);
		col = InitCompositeCollider();
		InitCube(scene, glm::vec3(0.0f, 3.0f, 0.0f), col, field);
		InitSphere(scene, glm::vec3(-20.0f, 3.0f, -20.0f), field, "planks");
		InitSphere(scene, glm::vec3(-20.0f, 3.0f, -10.0f), field, "planks");
		InitSphere(scene, glm::vec3(-30.0f, 3.0f, -10.0f), field, "planks");
		InitSphere(scene, glm::vec3(-10.0f, 3.0f, -20.0f), field, "planks");
		for (int i = 0; i < 5; i++) {
			InitSphere(scene, glm::vec3(-10.0f + 0.02f * (float)(i % 2), 3.0f + (float)i * 5.0f, -20.0f), field, "planks");
		}
		for (int i = 5; i < 10; i++) {
			InitSphere(scene, glm::vec3(-11.0f + 0.02f * (float)(i % 2), 3.0f + (float)i * 5.0f, -20.0f), field, "gold");
		}
		for (int i = 10; i < 15; i++) {
			auto* obj = InitSphere(scene, glm::vec3(-11.0f + 0.02f * (float)(i % 2), 3.0f + (float)i * 5.0f, -20.0f), field, "glowing");
			auto* light = Light::Instantiate();
			light->Init(glm::vec4(0.0f, 0.0f, 0.0f, 1.0f), glm::vec3(0.1f, 0.0f, 10.0f));
			obj->addComponent(light);
			light->SetPositionProvider(obj);
			scene->AddLight(light);
		}
		InitSkyBox(scene);
		InitLoadedGeometry(scene, glm::vec3(-10.0f, 3.0f, -30.0f), field);
		InitAvatar(scene, field);
		InitCaptions(scene);

		return scene;
	}
	
	ForceField* SceneFactory::InitGravitation(Scene* scene)
	{
		SceneObject* obj = SceneObject::Instantiate();
		obj->Init();
		auto* field = HomogeneForceField::Instantiate();
		field->SetStrength(9.8f);

		obj->addComponent(field);
		scene->AddSceneObject(obj);
		return field;
	}
	
	CompositeCollider* SceneFactory::InitCompositeCollider()
	{
		// 1, 1 quarter
		CompositeCollider* col = CompositeCollider::Instantiate();
		col->Init();
		auto* subCol = AABBCollider::Instantiate();
		subCol->Init();
		subCol->setMinRelToPosition(glm::vec3(-2.5f, -0.5f, -2.5f));
		subCol->setMaxRelToPosition(glm::vec3(2.5f, 0.5f, 2.5f));
		col->addSubCollider(subCol, glm::vec3(2.5f, 0.0f, 2.5f));
		subCol = AABBCollider::Instantiate();
		subCol->Init();
		subCol->setMinRelToPosition(glm::vec3(-2.5f, -0.5f, -2.5f));
		subCol->setMaxRelToPosition(glm::vec3(2.5f, 0.5f, 2.5f));
		col->addSubCollider(subCol, glm::vec3(7.5f, 0.0f, 2.5f));
		subCol = AABBCollider::Instantiate();
		subCol->Init();
		subCol->setMinRelToPosition(glm::vec3(-2.5f, -0.5f, -2.5f));
		subCol->setMaxRelToPosition(glm::vec3(2.5f, 0.5f, 2.5f));
		col->addSubCollider(subCol, glm::vec3(7.5f, 0.0f, 7.5f));
		subCol = AABBCollider::Instantiate();
		subCol->Init();
		subCol->setMinRelToPosition(glm::vec3(-2.5f, -0.5f, -2.5f));
		subCol->setMaxRelToPosition(glm::vec3(2.5f, 0.5f, 2.5f));
		col->addSubCollider(subCol, glm::vec3(2.5f, 0.0f, 7.5f));

		// -1, 1 quarter
		subCol = AABBCollider::Instantiate();
		subCol->Init();
		subCol->setMinRelToPosition(glm::vec3(-2.5f, -0.5f, -2.5f));
		subCol->setMaxRelToPosition(glm::vec3(2.5f, 0.5f, 2.5f));
		col->addSubCollider(subCol, glm::vec3(-2.5f, 0.0f, 2.5f));
		subCol = AABBCollider::Instantiate();
		subCol->Init();
		subCol->setMinRelToPosition(glm::vec3(-2.5f, -0.5f, -2.5f));
		subCol->setMaxRelToPosition(glm::vec3(2.5f, 0.5f, 2.5f));
		col->addSubCollider(subCol, glm::vec3(-7.5f, 0.0f, 2.5f));
		subCol = AABBCollider::Instantiate();
		subCol->Init();
		subCol->setMinRelToPosition(glm::vec3(-2.5f, -0.5f, -2.5f));
		subCol->setMaxRelToPosition(glm::vec3(2.5f, 0.5f, 2.5f));
		col->addSubCollider(subCol, glm::vec3(-2.5f, 0.0f, 7.5f));
		subCol = AABBCollider::Instantiate();
		subCol->Init();
		subCol->setMinRelToPosition(glm::vec3(-2.5f, -0.5f, -2.5f));
		subCol->setMaxRelToPosition(glm::vec3(2.5f, 0.5f, 2.5f));
		col->addSubCollider(subCol, glm::vec3(-7.5f, 0.0f, 7.5f));

		// 1, -1 quarter
		subCol = AABBCollider::Instantiate();
		subCol->Init();
		subCol->setMinRelToPosition(glm::vec3(-2.5f, -0.5f, -2.5f));
		subCol->setMaxRelToPosition(glm::vec3(2.5f, 0.5f, 2.5f));
		col->addSubCollider(subCol, glm::vec3(2.5f, 0.0f, -2.5f));
		subCol = AABBCollider::Instantiate();
		subCol->Init();
		subCol->setMinRelToPosition(glm::vec3(-2.5f, -0.5f, -2.5f));
		subCol->setMaxRelToPosition(glm::vec3(2.5f, 0.5f, 2.5f));
		col->addSubCollider(subCol, glm::vec3(7.5f, 0.0f, -2.5f));
		subCol = AABBCollider::Instantiate();
		subCol->Init();
		subCol->setMinRelToPosition(glm::vec3(-2.5f, -0.5f, -2.5f));
		subCol->setMaxRelToPosition(glm::vec3(2.5f, 0.5f, 2.5f));
		col->addSubCollider(subCol, glm::vec3(2.5f, 0.0f, -7.5f));
		subCol = AABBCollider::Instantiate();
		subCol->Init();
		subCol->setMinRelToPosition(glm::vec3(-2.5f, -0.5f, -2.5f));
		subCol->setMaxRelToPosition(glm::vec3(2.5f, 0.5f, 2.5f));
		col->addSubCollider(subCol, glm::vec3(7.5f, 0.0f, -7.5f));

		// -1, -1 quarter
		subCol = AABBCollider::Instantiate();
		subCol->Init();
		subCol->setMinRelToPosition(glm::vec3(-2.5f, -0.5f, -2.5f));
		subCol->setMaxRelToPosition(glm::vec3(2.5f, 0.5f, 2.5f));
		col->addSubCollider(subCol, glm::vec3(-2.5f, 0.0f, -2.5f));
		subCol = AABBCollider::Instantiate();
		subCol->Init();
		subCol->setMinRelToPosition(glm::vec3(-2.5f, -0.5f, -2.5f));
		subCol->setMaxRelToPosition(glm::vec3(2.5f, 0.5f, 2.5f));
		col->addSubCollider(subCol, glm::vec3(-7.5f, 0.0f, -2.5f));
		subCol = AABBCollider::Instantiate();
		subCol->Init();
		subCol->setMinRelToPosition(glm::vec3(-2.5f, -0.5f, -2.5f));
		subCol->setMaxRelToPosition(glm::vec3(2.5f, 0.5f, 2.5f));
		col->addSubCollider(subCol, glm::vec3(-2.5f, 0.0f, -7.5f));
		subCol = AABBCollider::Instantiate();
		subCol->Init();
		subCol->setMinRelToPosition(glm::vec3(-2.5f, -0.5f, -2.5f));
		subCol->setMaxRelToPosition(glm::vec3(2.5f, 0.5f, 2.5f));
		col->addSubCollider(subCol, glm::vec3(-7.5f, 0.0f, -7.5f));

		return col;
	}
	
	void SceneFactory::InitCube(Scene* scene, glm::vec3 pos, Collider* collider, ForceField* field)
	{
		ShaderProgram* cubeShader = ShaderProgramFactory::GetInstance()->GetDefaultPBRProgramWithMapping();
		auto* material = MaterialFactory::GetInstance()->getPBRMaterial("vinyl");
		Geometry* cubeGeometry = GeometryFactory::GetInstance()->getCube();
		auto* cubeMesh = Mesh::Instantiate();
		cubeMesh->Init(material, cubeGeometry);
		auto* obj = SceneObject::Instantiate();
		obj->Init(cubeMesh);
		obj->SetPosition(pos);
		obj->SetScale(glm::vec3(10.0f, 0.5f, 10.0f));

		auto* cubePhysics = Physics::Instantiate();
		cubePhysics->Init(obj);
		//cubePhysics->addAppliedForce(glm::vec3(100.0f, 0.0f, 0.0f));
		cubePhysics->setWorldSpaceDrag(glm::vec3(0.5f, 100.5f, 0.5f));
		cubePhysics->setModelSpaceDrag(glm::vec3(0.1f, 0.3f, 0.1f));
		cubePhysics->setMass(3140000.0f);
		//cubePhysics->addAppliedTorque(glm::vec3(0.5f, 0.5f, 0.5f));
		cubePhysics->setMomentOfInertia(Physics::getMomentOfInertiaOfCuboid(cubePhysics->getMass(), obj->GetScale()));
		cubePhysics->setRotationalDrag(glm::vec3(2000.0f, 1000.0f, 2000.0f));
		cubePhysics->setPositionForcingLevel(0.5f);
		cubePhysics->setElasticity(0.1f);
		if (field != nullptr) {
			field->AddListener(cubePhysics);
		}
		obj->addComponent(cubePhysics);
		collider->SetPhysics(cubePhysics);
		collider->SetPositionProvider(obj);
		collider->SetOrientationProvider(obj);
		obj->addComponent(collider);
		obj->Update(0.0f, scene->GetCamera());
		scene->AddCollider(collider);
		scene->AddSceneObject(obj, "cube");
	}
	
	SceneObject* SceneFactory::InitSphere(Scene* scene, const glm::vec3& pos, ForceField* field, const char* materialName)
	{
		SphericalCollider* collider = SphericalCollider::Instantiate();
		collider->Init();
		collider->SetRadius(0.5f);
		scene->AddCollider(collider);
		Material* material;
		if (std::string(materialName) == std::string("glowing")) {
			material = MaterialFactory::GetInstance()->getEmissiveMaterial("glowing", glm::vec3(0, 0, 1), 100.0f);
		}
		else {
			material = MaterialFactory::GetInstance()->getPBRMaterial(materialName);
		}
		Geometry* geometry = GeometryFactory::GetInstance()->getSphere();
		auto* mesh = Mesh::Instantiate();
		mesh->Init(material, geometry);
		auto* obj = SceneObject::Instantiate();
		obj->Init(mesh);
		obj->SetPosition(pos);
		obj->SetScale(glm::vec3(0.5f, 0.5f, 0.5f));

		auto* physics = Physics::Instantiate();
		physics->Init(obj);
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
			field->AddListener(physics);
		}
		obj->addComponent(physics);
		collider->SetPhysics(physics);
		collider->SetPositionProvider(obj);
		collider->SetOrientationProvider(obj);
		obj->addComponent(collider);
		scene->AddSceneObject(obj, std::string("sphere").append(materialName));
		return obj;
	}
	
	void SceneFactory::InitCaptions(Scene* scene)
	{
		auto* shader = ShaderProgramFactory::GetInstance()->GetGlyphProgram();
		auto* font = Font::Instantiate();
		font->Init(shader);
		font->Load(AssetFolderPathManager::getInstance()->getFontsFolderPath().append("arial.ttf"));

		Caption* caption1 = Caption::Instantiate();
		caption1->Init("Homebrew Graphics Engine Demo", font,
			glm::vec2(GlobalVariables::renderResolutionWidth / 2, GlobalVariables::renderResolutionHeight * 0.95), 1.5f, glm::vec4(1, 1, 1, 1));
		scene->AddCaption(caption1);
	}
	
	void SceneFactory::InitGroud(Scene* scene)
	{
		for (int i = 0; i < 10; i++) {
			for (int j = 0; j < 10; j++) {
				auto* material = MaterialFactory::GetInstance()->getPBRMaterial("vinyl");
				Geometry* cubeGeometry = GeometryFactory::GetInstance()->getCube();
				auto* cubeMesh = Mesh::Instantiate();
				cubeMesh->Init(material, cubeGeometry);
				auto* obj = SceneObject::Instantiate();
				obj->Init(cubeMesh);
				obj->SetPosition(glm::vec3(i * 100.0f - 500.0f, 0.0f, j * 100.0f - 500.0f));
				obj->SetScale(glm::vec3(50.0f, 1.0f, 50.0f));

				auto* cubePhysics = Physics::Instantiate();
				cubePhysics->Init(obj);
				cubePhysics->setPositionForcingLevel(0.0f);
				AABBCollider* collider = AABBCollider::Instantiate();
				collider->Init();
				cubePhysics->setElasticity(0.2f);
				cubePhysics->setFriction(0.9f);
				obj->addComponent(cubePhysics);
				collider->SetPhysics(cubePhysics);
				collider->setMinRelToPosition(glm::vec3(-49.99f, -1.0f, -49.99f));
				collider->setMaxRelToPosition(glm::vec3(49.99f, 1.0f, 49.99f));
				collider->SetPositionProvider(obj);
				obj->addComponent(collider);
				scene->AddCollider(collider, "ground");
				scene->AddSceneObject(obj, "ground");
			}
		}
	}
	
	void SceneFactory::InitSkyBox(Scene* scene)
	{
		ShaderProgram* skyboxShader = ShaderProgram::Instantiate();
		skyboxShader->Init(
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
		auto* cubeMap = TextureCube::Instantiate();
		cubeMap->Init(imagePaths, SKYBOX_UNIT);
		auto* skyBoxMaterial = Material::Instantiate();
		skyBoxMaterial->Init(skyboxShader);
		skyBoxMaterial->addTexture(cubeMap);
		Geometry* fullscreenQuad = GeometryFactory::GetInstance()->getFullScreenQuad();
		auto* skyBoxMesh = Mesh::Instantiate();
		skyBoxMesh->Init(skyBoxMaterial, fullscreenQuad);
		skyBoxMesh->setDepthTest(false);
		skyBoxMesh->setStencilTest(false);
		auto* obj = SceneObject::Instantiate();
		obj->Init(skyBoxMesh);
		scene->AddSceneObject(obj);
	}
	
	void SceneFactory::InitLoadedGeometry(Scene* scene, const glm::vec3& pos, ForceField* field)
	{
		SphericalCollider* collider = SphericalCollider::Instantiate();
		collider->Init();
		collider->SetRadius(0.5f);
		scene->AddCollider(collider);
		ShaderProgram* shader = ShaderProgramFactory::GetInstance()->GetDefaultPBRProgramWithMapping();
		auto* material = MaterialFactory::GetInstance()->getPBRMaterial("planks");
		Geometry* geometry = GeometryLoader().Load(AssetFolderPathManager::getInstance()->getGeometryFolderPath().append("mango.obj"));
		geometry->SetFaceCulling(false);
		auto* mesh = Mesh::Instantiate();
		mesh->Init(material, geometry);
		auto* obj = SceneObject::Instantiate();
		obj->Init(mesh);
		obj->SetPosition(pos);
		obj->SetScale(glm::vec3(0.5f, 0.5f, 0.5f));
		collider->SetPositionProvider(obj);
		collider->SetOrientationProvider(obj);
		auto* physics = Physics::Instantiate();
		physics->Init(obj);
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
			field->AddListener(physics);
		}
		obj->addComponent(physics);
		collider->SetPhysics(physics);
		obj->addComponent(collider);
		scene->AddSceneObject(obj);
	}
	
	void SceneFactory::InitAvatar(Scene* scene, ForceField* gravitation)
	{
		auto* avatar = SceneObject::Instantiate();
		avatar->Init();
		avatar->SetPosition(glm::vec3(-60.0f, 2.0f, -60.0f));
		scene->GetCamera().SetPositionProvider(avatar);
		scene->GetCamera().setPositionInProvidersSpace(glm::vec3(0.0f, 0.8f, 0.0f));
		auto* collider = AABBCollider::Instantiate();
		collider->Init();
		collider->setMinRelToPosition(glm::vec3(-0.2f, -1.0f, -0.2f));
		collider->setMaxRelToPosition(glm::vec3(0.2f, 1.0f, 0.2f));
		auto* physics = Physics::Instantiate();
		physics->Init(avatar);
		physics->setMass(80.0f);
		physics->setWorldSpaceDrag(glm::vec3(200.0f, 0.01f, 200.0f));
		physics->setPositionForcingLevel(1.0f);
		physics->setFriction(0.001f);
		gravitation->AddListener(physics);
		collider->SetPhysics(physics);
		collider->SetPositionProvider(avatar);
		scene->AddCollider(collider, "avatar");
		FirstPersonControl* control = FirstPersonControl::Instantiate();
		control->SetScene(scene);
		control->setPhysics(physics);
		control->setCamera(&scene->GetCamera());
		control->setInitialDirection(glm::normalize(glm::vec3(1, 0, 1)));
		control->setInitialUp(scene->getPreferedUp());
		control->setJumpImpulse(600.0f);
		control->setPropellingForce(2000.0f);
		auto* jumpCollider = AABBCollider::Instantiate();
		jumpCollider->Init();
		jumpCollider->setMinRelToPosition(glm::vec3(-0.2f, -1.1f, -0.2f));
		jumpCollider->setMaxRelToPosition(glm::vec3(0.2f, -0.9f, 0.2f));
		jumpCollider->SetPositionProvider(avatar);
		control->setJumpCollider(jumpCollider);
		control->SetPositionProvider(avatar);
		control->SetOrientationProvider(avatar);
		avatar->addComponent(jumpCollider);
		scene->AddCollider(jumpCollider, "avatar");
		scene->SetUserControl(control);
		avatar->addComponent(control);
		avatar->addComponent(physics);
		avatar->addComponent(collider);
		scene->getShadowCaster()->SetPositionProvider(avatar);
		scene->getShadowCaster()->setPositionOffsetToProvider(glm::vec3(-20, 20, -20));
		scene->AddSceneObject(avatar);
		InitLaserBeam(scene, control);
	}
	
	void SceneFactory::InitLaserBeam(Hogra::Scene* scene, Hogra::FirstPersonControl* control)
	{
		auto obj = SceneObject::Instantiate();
		auto geom = GeometryFactory::GetInstance()->getCilinder();
		auto material = MaterialFactory::GetInstance()->getEmissiveMaterial("laser", glm::vec3(1.0f, 0.0f, 0.0f), 100.0f);
		auto mesh = Mesh::Instantiate();
		mesh->Init(material, geom);
		obj->Init(mesh);
		obj->SetIsVisible(false);
		obj->SetIsCastingShadow(false);
		scene->AddSceneObject(obj);
		control->SetLaserObject(obj);
		Light* laserInpactLight = Light::Instantiate();
		laserInpactLight->Init(glm::vec4(0.0f, 0.0f, 0.0f, 1.0f), glm::vec3(25.0f, 20.0f, 10.0f));
		scene->AddLight(laserInpactLight);
		control->SetLaserInpactLight(laserInpactLight);
	}
}
