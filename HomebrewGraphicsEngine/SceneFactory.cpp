#include "SceneFactory.h"
#include "MaterialFactory.h"
#include "SphericalCollider.h"
#include "CompositeCollider.h"
#include "ShaderProgramFactory.h"
#include <glm/ext/vector_float3.hpp>
#include <glm/ext/vector_float4.hpp>
#include "GeometryLoader.h"
#include "ShaderProgramFactory.h"
#include "AudioDevice.h"
#include "Texture3D.h"
#include "Texture2D.h"
#include "Scene.h"
#include "AudioBuffer.h"
#include "AudioSource.h"
#include "AudioListener.h"

#include "SceneAudioSource.h"
#include "ObservObjectControl.h"
#include "MemoryManager.h"
#include "PositionConnector.h"
#include "SceneObjectFactory.h"


namespace Hogra {
	SceneFactory* SceneFactory::instance = nullptr;

	SceneFactory* SceneFactory::getInstance() {
		if (nullptr == instance) {
			instance = new SceneFactory();
		}
		return instance;
	}
	
	void SceneFactory::DestroyInstance() {
		Allocator<SceneFactory>::Delete(instance);
	}

	Scene* SceneFactory::CreateDemoScene(int contextWidth, int contextHeight) {
		Scene* scene = new Scene();
		scene->Init(contextWidth, contextHeight);
		auto* light = Allocator<Light>::New();
		light->Init(glm::normalize(glm::vec4(-1.0f, 1.0f, -1.0f, 0.0f)), glm::vec3(1.0f, 1.0f, 1.0f));
		scene->AddLight(light);	// Directional light
		
		light = Allocator<Light>::New();
		light->Init(glm::vec4(-80.0f, 2.0f, 0.0f, 1.0f), glm::vec3(250.0f, 50.0f, 50.0f));
		scene->AddLight(light);
		light = Allocator<Light>::New();
		light->Init(glm::vec4(0.0f, 2.0f, 80.0f, 1.0f), glm::vec3(50.0f, 250.0f, 50.0f));
		scene->AddLight(light);
		light = Allocator<Light>::New();
		light->Init(glm::vec4(80.0f, 2.0f, 0.0f, 1.0f), glm::vec3(50.0f, 50.0f, 250.0f));
		scene->AddLight(light);
		std::srand(0);
		for (int i = 0; i < 10; i++) {
			auto* lightInst = Allocator<Light>::New();
			lightInst->Init(glm::vec4(std::rand() % 1000 - 500, 2.0f, std::rand() % 1000 - 500, 1.0f), glm::vec3(5.0f, 5.0f, 5.0f));
			scene->AddLight(lightInst);
		}
		
		InitGroud(scene);

		
		ForceField* field = InitGravitation(scene);
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
			auto* light = Allocator<Light>::New();
			light->Init(glm::vec4(0.0f, 0.0f, 0.0f, 1.0f), glm::vec3(0.1f, 0.0f, 10.0f));
			obj->addComponent(light);
			light->SetPositionProvider(obj);
			scene->AddLight(light);
		}

		auto* mario = SceneObjectFactory::GetInstance()->Create2DSpriteObject(
			AssetFolderPathManager::getInstance()->getTextureFolderPath().append("sprites/mario-bros-hd.png"),
			&scene->GetCamera()
		);
		auto* marioColl = Allocator<AABBCollider>::New();
		marioColl->Init();
		marioColl->SetPositionProvider(mario);
		marioColl->setMinRelToPosition(glm::vec3(-0.5f, -0.5f, -0.5f));
		marioColl->setMaxRelToPosition(glm::vec3(0.5f, 0.5f, 0.5f));
		scene->AddCollider(marioColl);
		mario->addComponent(marioColl);
		mario->SetPosition(glm::vec3(-11.0f, 4.0f, -20.0f));
		scene->AddSceneObject(mario, "mario_sprite");

		InitSkyBox(scene);
		InitLoadedGeometry(scene, glm::vec3(-10.0f, 3.0f, -30.0f), field);
		
		
		/*
		auto* volumeLight = Light::Instantiate();
		volumeLight->Init(
			glm::vec4(15, 5, 15, 1.0),
			glm::vec3(1000.0f, 1000.0f, 1000.0f)
		);
		auto volumeObject = new VolumeObject();
		volumeObject->Init(
			voxelTexture,
			glm::vec3(0, 0, 0),
			glm::vec3(0.01, 0.01, 0.01),
			glm::angleAxis(90.0f, glm::vec3(1, 0, 0)),
			volumeLight,
			glm::ivec2(GlobalVariables::renderResolutionWidth, GlobalVariables::renderResolutionHeight));
		scene->AddVolumeObject(volumeObject);
		*/

		FirstPersonControl* control = nullptr;
		InitAvatar(scene, field, control);
		// InitObjectObserverControl(scene, volumeObject);
		InitCaptions(scene);

		InitAudio(scene, control);

		auto* stage0 = new PostProcessStage(AssetFolderPathManager::getInstance()->getShaderFolderPath().append("depthEffects.frag"),
			contextWidth, contextHeight);
		scene->AddPostProcessStage(stage0);

		auto* bloom = new Bloom();
		bloom->Init(contextWidth, contextHeight);
		scene->AddPostProcessStage(bloom);

		auto* stage1 = new PostProcessStage(AssetFolderPathManager::getInstance()->getShaderFolderPath().append("hdr.frag"),
			contextWidth, contextHeight);
		scene->AddPostProcessStage(stage1);
		/*
		auto* stage2 = new PostProcessStage(AssetFolderPathManager::getInstance()->getShaderFolderPath().append("tapeEffect.frag"),
			contextWidth, contextHeight);
		stage2->AddUniformVariable(&timeSpent);
		scene->AddPostProcessStage(stage2);
		*/

		//auto* stage1 = new PostProcessStage(AssetFolderPathManager::getInstance()->getShaderFolderPath().append("edgeDetect.frag"),
		//	contextWidth, contextHeight);
		//  scene->AddPostProcessStage(stage1);

		return scene;
	}

	Scene* SceneFactory::CreateVoxelDemoScene(int contextWidth, int contextHeight)
	{

		//Test mem allocator:
		Texture2D* textureInstance = Allocator<Hogra::Texture2D>::New();

		Scene* scene = Allocator<Scene>::New();
		scene->Init(contextWidth, contextHeight);

		// Volume:
		const char* dataSetName = "Shoulder";
		auto* voxelTexture = Allocator<Texture3D>::New();
		voxelTexture->Init(
//			AssetFolderPathManager::getInstance()->getTextureFolderPath().append("cthead-8bit"),
	AssetFolderPathManager::getInstance()->getTextureFolderPath().append(dataSetName),
		3,
			GL_RED
		);

		auto* volumeLight = Allocator<Light>::New();
		volumeLight->Init(
			glm::vec4(10, 10, 10, 1.0),
			glm::vec3(10000.0f, 10000.0f, 10000.0f)
		);
		auto* volumeObject = Allocator<VolumeObject>::New();
		volumeObject->Init(
			voxelTexture,
			glm::vec3(0, 0, 0),
			glm::vec3(0.01, 0.01, 0.01),
			glm::angleAxis(1.57079633f, glm::vec3(1, 0, 0)),
			volumeLight,
			glm::ivec2(GlobalVariables::renderResolutionWidth, GlobalVariables::renderResolutionHeight));
		scene->AddVolumeObject(volumeObject);

		InitObjectObserverControl(scene, volumeObject);
		InitVoxelCaption(scene, dataSetName);
		
		
		auto* bloom = Allocator<Bloom>::New();
		bloom->Init(contextWidth, contextHeight);
		scene->AddPostProcessStage(bloom);

		auto* hdr = new PostProcessStage(
			AssetFolderPathManager::getInstance()->getShaderFolderPath().append("hdr.frag"),
			contextWidth, contextHeight);
		scene->AddPostProcessStage(hdr);

		
		return scene;
	}
	
	ForceField* SceneFactory::InitGravitation(Scene* scene)
	{
		SceneObject* obj = Allocator<SceneObject>::New();
		obj->Init();
		auto* field = Allocator<HomogeneForceField>::New();
		field->SetStrength(9.8f);

		obj->addComponent(field);
		scene->AddSceneObject(obj);
		return field;
	}
	
	CompositeCollider* SceneFactory::InitCompositeCollider()
	{
		// 1, 1 quarter
		CompositeCollider* col = Allocator<CompositeCollider>::New();
		col->Init();
		auto* subCol = Allocator<AABBCollider>::New();
		subCol->Init();
		subCol->setMinRelToPosition(glm::vec3(-2.5f, -0.5f, -2.5f));
		subCol->setMaxRelToPosition(glm::vec3(2.5f, 0.5f, 2.5f));
		col->addSubCollider(subCol, glm::vec3(2.5f, 0.0f, 2.5f));
		subCol = Allocator<AABBCollider>::New();
		subCol->Init();
		subCol->setMinRelToPosition(glm::vec3(-2.5f, -0.5f, -2.5f));
		subCol->setMaxRelToPosition(glm::vec3(2.5f, 0.5f, 2.5f));
		col->addSubCollider(subCol, glm::vec3(7.5f, 0.0f, 2.5f));
		subCol = Allocator<AABBCollider>::New();
		subCol->Init();
		subCol->setMinRelToPosition(glm::vec3(-2.5f, -0.5f, -2.5f));
		subCol->setMaxRelToPosition(glm::vec3(2.5f, 0.5f, 2.5f));
		col->addSubCollider(subCol, glm::vec3(7.5f, 0.0f, 7.5f));
		subCol = Allocator<AABBCollider>::New();
		subCol->Init();
		subCol->setMinRelToPosition(glm::vec3(-2.5f, -0.5f, -2.5f));
		subCol->setMaxRelToPosition(glm::vec3(2.5f, 0.5f, 2.5f));
		col->addSubCollider(subCol, glm::vec3(2.5f, 0.0f, 7.5f));

		// -1, 1 quarter
		subCol = Allocator<AABBCollider>::New();
		subCol->Init();
		subCol->setMinRelToPosition(glm::vec3(-2.5f, -0.5f, -2.5f));
		subCol->setMaxRelToPosition(glm::vec3(2.5f, 0.5f, 2.5f));
		col->addSubCollider(subCol, glm::vec3(-2.5f, 0.0f, 2.5f));
		subCol = Allocator<AABBCollider>::New();
		subCol->Init();
		subCol->setMinRelToPosition(glm::vec3(-2.5f, -0.5f, -2.5f));
		subCol->setMaxRelToPosition(glm::vec3(2.5f, 0.5f, 2.5f));
		col->addSubCollider(subCol, glm::vec3(-7.5f, 0.0f, 2.5f));
		subCol = Allocator<AABBCollider>::New();
		subCol->Init();
		subCol->setMinRelToPosition(glm::vec3(-2.5f, -0.5f, -2.5f));
		subCol->setMaxRelToPosition(glm::vec3(2.5f, 0.5f, 2.5f));
		col->addSubCollider(subCol, glm::vec3(-2.5f, 0.0f, 7.5f));
		subCol = Allocator<AABBCollider>::New();
		subCol->Init();
		subCol->setMinRelToPosition(glm::vec3(-2.5f, -0.5f, -2.5f));
		subCol->setMaxRelToPosition(glm::vec3(2.5f, 0.5f, 2.5f));
		col->addSubCollider(subCol, glm::vec3(-7.5f, 0.0f, 7.5f));

		// 1, -1 quarter
		subCol = Allocator<AABBCollider>::New();
		subCol->Init();
		subCol->setMinRelToPosition(glm::vec3(-2.5f, -0.5f, -2.5f));
		subCol->setMaxRelToPosition(glm::vec3(2.5f, 0.5f, 2.5f));
		col->addSubCollider(subCol, glm::vec3(2.5f, 0.0f, -2.5f));
		subCol = Allocator<AABBCollider>::New();
		subCol->Init();
		subCol->setMinRelToPosition(glm::vec3(-2.5f, -0.5f, -2.5f));
		subCol->setMaxRelToPosition(glm::vec3(2.5f, 0.5f, 2.5f));
		col->addSubCollider(subCol, glm::vec3(7.5f, 0.0f, -2.5f));
		subCol = Allocator<AABBCollider>::New();
		subCol->Init();
		subCol->setMinRelToPosition(glm::vec3(-2.5f, -0.5f, -2.5f));
		subCol->setMaxRelToPosition(glm::vec3(2.5f, 0.5f, 2.5f));
		col->addSubCollider(subCol, glm::vec3(2.5f, 0.0f, -7.5f));
		subCol = Allocator<AABBCollider>::New();
		subCol->Init();
		subCol->setMinRelToPosition(glm::vec3(-2.5f, -0.5f, -2.5f));
		subCol->setMaxRelToPosition(glm::vec3(2.5f, 0.5f, 2.5f));
		col->addSubCollider(subCol, glm::vec3(7.5f, 0.0f, -7.5f));

		// -1, -1 quarter
		subCol = Allocator<AABBCollider>::New();
		subCol->Init();
		subCol->setMinRelToPosition(glm::vec3(-2.5f, -0.5f, -2.5f));
		subCol->setMaxRelToPosition(glm::vec3(2.5f, 0.5f, 2.5f));
		col->addSubCollider(subCol, glm::vec3(-2.5f, 0.0f, -2.5f));
		subCol = Allocator<AABBCollider>::New();
		subCol->Init();
		subCol->setMinRelToPosition(glm::vec3(-2.5f, -0.5f, -2.5f));
		subCol->setMaxRelToPosition(glm::vec3(2.5f, 0.5f, 2.5f));
		col->addSubCollider(subCol, glm::vec3(-7.5f, 0.0f, -2.5f));
		subCol = Allocator<AABBCollider>::New();
		subCol->Init();
		subCol->setMinRelToPosition(glm::vec3(-2.5f, -0.5f, -2.5f));
		subCol->setMaxRelToPosition(glm::vec3(2.5f, 0.5f, 2.5f));
		col->addSubCollider(subCol, glm::vec3(-2.5f, 0.0f, -7.5f));
		subCol = Allocator<AABBCollider>::New();
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
		auto* cubeMesh = Allocator<Mesh>::New();
		cubeMesh->Init(material, cubeGeometry);
		auto* obj = Allocator<SceneObject>::New();
		obj->Init(cubeMesh);
		obj->SetPosition(pos);
		obj->SetScale(glm::vec3(10.0f, 0.5f, 10.0f));

		auto* cubePhysics = Allocator<Physics>::New();
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
		obj->Update();
		scene->AddCollider(collider);
		scene->AddSceneObject(obj, "cube");
	}
	
	SceneObject* SceneFactory::InitSphere(Scene* scene, const glm::vec3& pos, ForceField* field, const char* materialName)
	{
		SphericalCollider* collider = Allocator<SphericalCollider>::New();
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
		auto* mesh = Allocator<Mesh>::New();
		mesh->Init(material, geometry);
		auto* obj = Allocator<SceneObject>::New();
		obj->Init(mesh);
		obj->SetPosition(pos);
		obj->SetScale(glm::vec3(0.5f, 0.5f, 0.5f));

		auto* physics = Allocator<Physics>::New();
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
		auto* font = Allocator<Font>::New();
		font->Init("arial.ttf");

		Caption* caption1 = Allocator<Caption>::New();
		caption1->Init("Homebrew Graphics Engine Demo", font,
			glm::vec2(GlobalVariables::renderResolutionWidth / 2, GlobalVariables::renderResolutionHeight * 0.95), 1.5f, glm::vec4(1, 1, 1, 1));
		scene->AddCaption(caption1);
		Allocator<Font>::Delete(font);
	}

	void SceneFactory::InitVoxelCaption(Scene* scene, const char* dataSetName) {
		auto* font = Allocator<Font>::New();
		font->Init("arial.ttf");
		auto* caption1 = Allocator<Caption>::New();
		caption1->Init("Volume rendering", font,
			glm::vec2(GlobalVariables::renderResolutionWidth / 2, GlobalVariables::renderResolutionHeight * 0.95), 1.0f, glm::vec4(1, 1, 1, 1));
		scene->AddCaption(caption1);
		auto* caption2 = Allocator<Caption>::New();
		caption2->Init(std::string("Data set: ").append(dataSetName), font,
			glm::vec2(GlobalVariables::renderResolutionWidth / 2, GlobalVariables::renderResolutionHeight * 0.91), 1.0f, glm::vec4(1, 1, 1, 1));
		scene->AddCaption(caption2);
		Allocator<Font>::Delete(font);
	}
	
	void SceneFactory::InitGroud(Scene* scene)
	{
		for (int i = 0; i < 10; i++) {
			for (int j = 0; j < 10; j++) {
				auto* material = MaterialFactory::GetInstance()->getPBRMaterial("vinyl");
				Geometry* cubeGeometry = GeometryFactory::GetInstance()->getCube();
				auto* cubeMesh = Allocator<Mesh>::New();
				cubeMesh->Init(material, cubeGeometry);
				auto* obj = Allocator<SceneObject>::New();
				obj->Init(cubeMesh);
				obj->SetPosition(glm::vec3(i * 100.0f - 500.0f, 0.0f, j * 100.0f - 500.0f));
				obj->SetScale(glm::vec3(50.0f, 1.0f, 50.0f));

				auto* cubePhysics = Allocator<Physics>::New();
				cubePhysics->Init(obj);
				cubePhysics->setPositionForcingLevel(0.0f);
				auto* collider = Allocator<AABBCollider>::New();
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
		ShaderProgram* skyboxShader = Allocator<ShaderProgram>::New();
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
		auto* cubeMap = Allocator<TextureCube>::New();
		cubeMap->Init(imagePaths, SKYBOX_UNIT);
		auto* skyBoxMaterial = Allocator<Material>::New();
		skyBoxMaterial->Init(skyboxShader);
		skyBoxMaterial->addTexture(cubeMap);
		Geometry* fullscreenQuad = GeometryFactory::GetInstance()->getFullScreenQuad();
		auto* skyBoxMesh = Allocator<Mesh>::New();
		skyBoxMesh->Init(skyBoxMaterial, fullscreenQuad);
		skyBoxMesh->setDepthTest(false);
		skyBoxMesh->setStencilTest(false);
		auto* obj = Allocator<SceneObject>::New();
		obj->Init(skyBoxMesh);
		scene->AddSceneObject(obj);
	}
	
	void SceneFactory::InitLoadedGeometry(Scene* scene, const glm::vec3& pos, ForceField* field)
	{
		SphericalCollider* collider = Allocator<SphericalCollider>::New();
		collider->Init();
		collider->SetRadius(0.5f);
		scene->AddCollider(collider);
		ShaderProgram* shader = ShaderProgramFactory::GetInstance()->GetDefaultPBRProgramWithMapping();
		auto* material = MaterialFactory::GetInstance()->getPBRMaterial("planks");
		Geometry* geometry = GeometryLoader().Load(AssetFolderPathManager::getInstance()->getGeometryFolderPath().append("mango.obj"));
		geometry->SetFaceCulling(false);
		auto* mesh = Allocator<Mesh>::New();
		mesh->Init(material, geometry);
		auto* obj = Allocator<SceneObject>::New();
		obj->Init(mesh);
		obj->SetPosition(pos);
		obj->SetScale(glm::vec3(0.5f, 0.5f, 0.5f));
		collider->SetPositionProvider(obj);
		collider->SetOrientationProvider(obj);
		auto* physics = Allocator<Physics>::New();
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
	
	void SceneFactory::InitAvatar(Scene* scene, ForceField* gravitation, FirstPersonControl*& control)
	{
		auto* avatar = Allocator<SceneObject>::New();
		avatar->Init();
		avatar->SetPosition(glm::vec3(-60.0f, 2.0f, -60.0f));
		auto* posConnector = Allocator<PositionConnector>::New();
		posConnector->Init(avatar, glm::vec3(0.0f, 0.8f, 0.0f));
		scene->GetCamera().SetPositionConnetor(posConnector);
		auto* collider = Allocator<AABBCollider>::New();
		collider->Init();
		collider->setMinRelToPosition(glm::vec3(-0.2f, -1.0f, -0.2f));
		collider->setMaxRelToPosition(glm::vec3(0.2f, 1.0f, 0.2f));
		auto* physics = Allocator<Physics>::New();
		physics->Init(avatar);
		physics->setMass(80.0f);
		physics->setWorldSpaceDrag(glm::vec3(200.0f, 0.01f, 200.0f));
		physics->setPositionForcingLevel(1.0f);
		physics->setFriction(0.001f);
		if (nullptr != gravitation) {
			gravitation->AddListener(physics);
		}
		collider->SetPhysics(physics);
		collider->SetPositionProvider(avatar);
		scene->AddCollider(collider, "avatar");
		control = Allocator<FirstPersonControl>::New();
		control->SetScene(scene);
		control->setPhysics(physics);
		control->setCamera(&scene->GetCamera());
		control->setInitialDirection(glm::normalize(glm::vec3(1, 0, 1)));
		control->setInitialUp(scene->getPreferedUp());
		control->setJumpImpulse(600.0f);
		control->setPropellingForce(2000.0f);
		auto* jumpCollider = Allocator<AABBCollider>::New();
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

	void SceneFactory::InitObjectObserverControl(Scene* scene, VolumeObject* volumeObject)
	{
		auto control = new ObservObjectControl();
		control->SetCamera(scene->GetCamera());
		if (nullptr != volumeObject) {
			control->SetVolumeObject(*volumeObject);
		}
		scene->SetUserControl(control);
	}
	
	void SceneFactory::InitLaserBeam(Hogra::Scene* scene, Hogra::FirstPersonControl* control)
	{
		auto obj = Allocator<SceneObject>::New();
		auto geom = GeometryFactory::GetInstance()->getCilinder();
		auto material = MaterialFactory::GetInstance()->getEmissiveMaterial("laser", glm::vec3(1.0f, 0.0f, 0.0f), 100.0f);
		auto mesh = Allocator<Mesh>::New();
		mesh->Init(material, geom);
		obj->Init(mesh);
		obj->SetIsVisible(false);
		obj->SetIsCastingShadow(false);
		scene->AddSceneObject(obj);
		control->SetLaserObject(obj);
		Light* laserInpactLight = Allocator<Light>::New();
		laserInpactLight->Init(glm::vec4(0.0f, 0.0f, 0.0f, 1.0f), glm::vec3(25.0f, 20.0f, 10.0f));
		scene->AddLight(laserInpactLight);
		control->SetLaserInpactLight(laserInpactLight);
	}

	void SceneFactory::InitAudio(Scene* scene, FirstPersonControl* control)
	{


		auto buffer = Allocator<AudioBuffer>::New();
		buffer->Init(AssetFolderPathManager::getInstance()->getSoundsFolderPath().append("human-impact.wav"));

		auto source = Allocator<AudioSource>::New();
		source->Init(buffer);
		source->SetGain(0.3f);

		auto sceneSource = Allocator<SceneAudioSource>::New();
		sceneSource->Init(source);
		scene->AddSceneAudioSource(sceneSource);
		control->SetJumpAudioSource(sceneSource);
		
		// Steps:
		auto footstepsBuffer = Allocator<AudioBuffer>::New();
		footstepsBuffer->Init(AssetFolderPathManager::getInstance()->getSoundsFolderPath().append("footsteps.wav"));
		auto footstepsSource = Allocator<AudioSource>::New();
		footstepsSource->Init(footstepsBuffer);
		auto footstepsSceneAudioSource = Allocator<SceneAudioSource>::New();
		footstepsSceneAudioSource->Init(footstepsSource);
		scene->AddSceneAudioSource(footstepsSceneAudioSource);
		control->SetFootstepAudioSource(footstepsSceneAudioSource);

		// Laser:
		auto laserBuffer = Allocator<AudioBuffer>::New();
		laserBuffer->Init(AssetFolderPathManager::getInstance()->getSoundsFolderPath().append("bruteLaser.wav"));
		auto laserSource = Allocator<AudioSource>::New();
		laserSource->Init(laserBuffer);
		auto laserSceneAudioSource = Allocator<SceneAudioSource>::New();
		laserSceneAudioSource->Init(laserSource);
		scene->AddSceneAudioSource(laserSceneAudioSource);
		control->SetLaserAudioSource(laserSceneAudioSource);

		auto laserCooldown = buildAudioSource("laserCooldown.wav");
		scene->AddSceneAudioSource(laserCooldown);
		control->SetLaserCooldownAudioSource(laserCooldown);

		auto laserChargeup = buildAudioSource("laserChargeup.wav");
		laserChargeup->SetGain(5.0f);
		scene->AddSceneAudioSource(laserChargeup);
		control->SetLaserChargeupAudioSource(laserChargeup);

		// Ambient:
		auto ambientBuffer = Allocator<AudioBuffer>::New();
		ambientBuffer->Init(AssetFolderPathManager::getInstance()->getSoundsFolderPath().append("wind.wav"));
		auto ambientSource = Allocator<AudioSource>::New();
		ambientSource->Init(ambientBuffer);
		auto ambientSceneSource = Allocator<SceneAudioSource>::New();
		ambientSceneSource->Init(ambientSource);
		scene->AddSceneAudioSource(ambientSceneSource);
		ambientSceneSource->SetLoop(true);
		ambientSceneSource->Play();
	}
	
	SceneAudioSource* SceneFactory::buildAudioSource(const std::string& fileName) {
		auto buffer = Allocator<AudioBuffer>::New();
		buffer->Init(AssetFolderPathManager::getInstance()->getSoundsFolderPath().append(fileName));
		auto source = Allocator<AudioSource>::New();
		source->Init(buffer);
		auto sceneSource = Allocator<SceneAudioSource>::New();
		sceneSource->Init(source);
		return sceneSource;
	}

	Scene* SceneFactory::LoadSceneFromFile(const std::string& fileName)
	{
		// Not working source files missing!
		//YAML::Node sceneNode = YAML::LoadFile(AssetFolderPathManager::getInstance()->getScenesFolderPath().append(fileName));
		
		// TODO

		Scene* scene = new Scene();
		return scene;
	}
}
