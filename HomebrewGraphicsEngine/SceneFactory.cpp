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
#include "ObserveObjectControl.h"
#include "MemoryManager.h"
#include "PositionConnector.h"
#include "SceneObjectFactory.h"
#include "RenderLayer.h"
#include "ControlActionManager.h"
#include "GUI.h"

namespace Hogra {
	SceneFactory* SceneFactory::instance = nullptr;

	SceneFactory* SceneFactory::getInstance() {
		if (nullptr == instance) {
			instance = Allocator::New<SceneFactory>();
		}
		return instance;
	}
	
	void SceneFactory::DestroyInstance() {
		Allocator::Delete(instance);
	}

	Scene* SceneFactory::CreateDemoScene(int contextWidth, int contextHeight) {
		Scene* scene = Allocator::New<Scene>();
		scene->Init(contextWidth, contextHeight);
		auto* defLayer = Allocator::New<RenderLayer>();
		defLayer->SetRenderMode(RenderLayer::RenderMode::deferredRenderMode);
		defLayer->SetName("DeferredLayer");
		scene->AddRenderLayer(defLayer);
		auto* light = Allocator::New<Light>();
		light->Init(glm::normalize(glm::vec4(-1.0f, 1.0f, -1.0f, 0.0f)), glm::vec3(1.0f, 1.0f, 1.0f));
		scene->AddLight(light);	// Directional light
		
		light = Allocator::New<Light>();
		light->Init(glm::vec4(-80.0f, 2.0f, 0.0f, 1.0f), glm::vec3(250.0f, 50.0f, 50.0f));
		scene->AddLight(light);
		light = Allocator::New<Light>();
		light->Init(glm::vec4(0.0f, 2.0f, 80.0f, 1.0f), glm::vec3(50.0f, 250.0f, 50.0f));
		scene->AddLight(light);
		light = Allocator::New<Light>();
		light->Init(glm::vec4(80.0f, 2.0f, 0.0f, 1.0f), glm::vec3(50.0f, 50.0f, 250.0f));
		scene->AddLight(light);
		std::srand(0);
		for (int i = 0; i < 10; i++) {
			auto* lightInst = Allocator::New<Light>();
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
			auto* light = Allocator::New<Light>();
			light->Init(glm::vec4(0.0f, 0.0f, 0.0f, 1.0f), glm::vec3(0.1f, 0.0f, 10.0f));
			obj->addComponent(light);
			light->SetPositionProvider(obj);
			scene->AddLight(light);
		}

		auto* mario = SceneObjectFactory::GetInstance()->Create2DSpriteObject(
			AssetFolderPathManager::getInstance()->getTextureFolderPath().append("sprites/mario-bros-hd.png"),
			&scene->GetCamera()
		);
		auto* marioColl = Allocator::New<AABBCollider>();
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

		auto* stage0 = Allocator::New<PostProcessStage>();
		stage0->Init(AssetFolderPathManager::getInstance()->getShaderFolderPath().append("depthEffects.frag"),
			contextWidth, contextHeight);
		scene->AddPostProcessStage(stage0);

		auto* bloom = Allocator::New<Bloom>();
		bloom->Init(contextWidth, contextHeight);
		scene->AddPostProcessStage(bloom);

		auto* stage1 = Allocator::New<PostProcessStage>();
		stage1->Init(AssetFolderPathManager::getInstance()->getShaderFolderPath().append("hdr.frag"),
			contextWidth, contextHeight);
		scene->AddPostProcessStage(stage1);
		return scene;
	}

	Scene* SceneFactory::CreateVoxelDemoScene(int contextWidth, int contextHeight)
	{

		//Test mem allocator:
		Texture2D* textureInstance = Allocator::New<Hogra::Texture2D>();

		Scene* scene = Allocator::New<Scene>();
		scene->Init(contextWidth, contextHeight);

		// Render layer:

		auto* forwardLayer = Allocator::New<RenderLayer>();
		forwardLayer->SetName("ForwardLayer");
		forwardLayer->SetRenderMode(RenderLayer::RenderMode::forwardRenderMode);
		scene->AddRenderLayer(forwardLayer);
		
		auto* deferedLayer = Allocator::New<RenderLayer>();
		deferedLayer->SetName("DeferredLayer");
		deferedLayer->SetRenderMode(RenderLayer::RenderMode::deferredRenderMode);
		scene->AddRenderLayer(deferedLayer);

		auto* volumeLayer = Allocator::New<RenderLayer>();
		volumeLayer->SetName("VolumeLayer");
		volumeLayer->SetRenderMode(RenderLayer::RenderMode::forwardRenderMode);
		scene->AddRenderLayer(volumeLayer);

		//InitSkyBox(scene);

		auto* light = Allocator::New<Light>();
		light->Init(glm::normalize(glm::vec4(-1.0f, 1.0f, -1.0f, 0.0f)), glm::vec3(1.0f, 1.0f, 1.0f));
		scene->AddLight(light);	// Directional light


		// Volume:
		const char* dataSetName = "Shoulder";
		//const char* dataSetName = "cthead-8bit";
		auto* voxelTexture = Allocator::New<Texture3D>();
		
		voxelTexture->Init(
	AssetFolderPathManager::getInstance()->getTextureFolderPath().append(dataSetName),
		3,
			GL_RED
		);

		//TODO
		/*
		voxelTexture->Init(
			glm::ivec3(50, 50, 50),
			std::function([](float x, float y, float z) {
				float l = sqrtf((x - 0.5) * (x - 0.5) + (y - 0.5) * (y - 0.5) + (z - 0.5) * (z - 0.5));
				return (x > 0.5f)? 1.0f : 0.0f;
				}),
			(GLuint)3,
			(GLenum)GL_RED
		);
		*/

		auto* volumeLight = Allocator::New<Light>();
		volumeLight->Init(
			glm::vec4(10, 10, 10, 1.0),
			glm::vec3(10000.0f, 10000.0f, 10000.0f)
		);
		volumeLight->SetCastShadow(false);

		auto* bulbSprite = SceneObjectFactory::GetInstance()->Create2DSpriteObject(AssetFolderPathManager::getInstance()->getTextureFolderPath().append("sprites/lightbulb.png"), &scene->GetCamera());
		auto* posConnector = Allocator::New<PositionConnector>();
		posConnector->Init(volumeLight);
		bulbSprite->SetPositionConnector(posConnector);
		scene->AddSceneObject(bulbSprite, "bulbSprite", "ForwardLayer");

		auto* volumeObject = Allocator::New<Volumetric::VolumeObject>();
		volumeObject->Init(
			voxelTexture,
			glm::vec3(0, 0, 0),
			glm::vec3(0.01, 0.01, 0.01),
			glm::angleAxis(1.57079633f, glm::vec3(1, 0, 0)),
			volumeLight,
			glm::ivec2(GlobalVariables::renderResolutionWidth, GlobalVariables::renderResolutionHeight));
		scene->AddLight(volumeLight);

		auto* volumeSceneObj = Allocator::New<SceneObject>();
		volumeSceneObj->Init();
		volumeSceneObj->addComponent(volumeObject);
		scene->AddSceneObject(volumeSceneObj, "volumeObj", "VolumeLayer");

		auto* ball = InitSphere(scene, glm::vec3(0,5,10), nullptr, "gold");
		auto* control = InitObjectObserverControl(scene, volumeObject);
		InitVoxelCaption(scene, dataSetName);
		auto* bloom = Allocator::New<Bloom>();
		bloom->Init(contextWidth, contextHeight);
		scene->AddPostProcessStage(bloom, "VolumeLayer");

		auto* hdr = Allocator::New<PostProcessStage>();
		hdr->Init(
			AssetFolderPathManager::getInstance()->getShaderFolderPath().append("hdr.frag"),
			contextWidth, contextHeight);
		scene->AddPostProcessStage(hdr, "VolumeLayer");

		// Init controls:
		{
			auto* rotateCam = Allocator::New<AxisMoveAction>();
			rotateCam->SetAction(
				[scene](const glm::vec2& pixDelta, const glm::vec2& pixPos) {
					scene->GetUserControl()->Rotate(-pixDelta);
				}
			);
			ControlActionManager::getInstance()->RegisterMouseMoveAction(rotateCam);

			auto* grabCam = Allocator::New<ButtonKeyAction>();
			grabCam->Init(GLFW_MOUSE_BUTTON_RIGHT, ButtonKeyAction::TriggerType::triggerOnPress);
			grabCam->SetAction(
				[scene]() {
					scene->GetUserControl()->grab();
				}
			);
			ControlActionManager::getInstance()->RegisterMouseButtonAction(grabCam);

			auto* releaseCam = Allocator::New<ButtonKeyAction>();
			releaseCam->Init(GLFW_MOUSE_BUTTON_RIGHT, ButtonKeyAction::TriggerType::triggerOnRelease);
			releaseCam->SetAction(
				[scene]() {
					scene->GetUserControl()->release();
				}
			);
			ControlActionManager::getInstance()->RegisterMouseButtonAction(releaseCam);

			auto* zoomCam = Allocator::New<AxisMoveAction>();
			zoomCam->SetAction(
				[scene](const glm::vec2& pixDelta, const glm::vec2& pixPos) {
					scene->GetUserControl()->Zoom(pixPos.y);
				}
			);
			ControlActionManager::getInstance()->RegisterMouseScrollAction(zoomCam);

			auto* leftClick = Allocator::New<ButtonKeyAction>();
			leftClick->Init(GLFW_MOUSE_BUTTON_LEFT, ButtonKeyAction::TriggerType::triggerOnPress);
			leftClick->SetAction(
				[control, volumeObject]() {
					double x;
					double y;
					glfwGetCursorPos(GlobalVariables::window, &x, &y);
					float ndc_x = x / (double)GlobalVariables::windowWidth * 2.0 - 1.0;
					float ndc_y = y / (double)GlobalVariables::windowHeight * 2.0 - 1.0;

					bool isSuccess = volumeObject->SelectTransferFunctionRegion(ndc_x, ndc_y);
					if (!isSuccess) {
						control->grabPlane(ndc_x, ndc_y);
					}
				}
			);
			ControlActionManager::getInstance()->RegisterMouseButtonAction(leftClick);

			auto* leftRelease = Allocator::New<ButtonKeyAction>();
			leftRelease->Init(GLFW_MOUSE_BUTTON_LEFT, ButtonKeyAction::TriggerType::triggerOnRelease);
			leftRelease->SetAction(
				[control]() {
					double x;
					double y;
					glfwGetCursorPos(GlobalVariables::window, &x, &y);
					float ndc_x = x / (double)GlobalVariables::windowWidth * 2.0 - 1.0;
					float ndc_y = y / (double)GlobalVariables::windowHeight * 2.0 - 1.0;
					control->releasePlane(ndc_x, ndc_y);
				}
			);
			ControlActionManager::getInstance()->RegisterMouseButtonAction(leftRelease);

			auto* toggleTransferVis = Allocator::New<ButtonKeyAction>();
			toggleTransferVis->Init(GLFW_KEY_H, ButtonKeyAction::TriggerType::triggerOnPress);
			toggleTransferVis->SetAction(
				[volumeObject]() {
					volumeObject->GetTransferFunction().ToggleVisibility();
				}
			);
			ControlActionManager::getInstance()->RegisterKeyAction(toggleTransferVis);

			auto* toggleMenu = Allocator::New<ButtonKeyAction>();
			toggleMenu->Init(GLFW_KEY_O, ButtonKeyAction::TriggerType::triggerOnPress);
			toggleMenu->SetAction(
				[]() {
					GUI::getInstance()->setVisible(!(GUI::getInstance()->IsVisible()));
				}
			);
			ControlActionManager::getInstance()->RegisterKeyAction(toggleMenu);

			auto* nextFeature = Allocator::New<ButtonKeyAction>();
			nextFeature->Init(GLFW_KEY_SPACE, ButtonKeyAction::TriggerType::triggerOnPress);
			nextFeature->SetAction(
			[volumeObject]() {
					volumeObject->CycleSelectedFeature();
				}
			);
			ControlActionManager::getInstance()->RegisterKeyAction(nextFeature);
		}

		return scene;
	}
	
	ForceField* SceneFactory::InitGravitation(Scene* scene)
	{
		SceneObject* obj = Allocator::New<SceneObject>();
		obj->Init();
		auto* field = Allocator::New<HomogeneForceField>();
		field->SetStrength(9.8f);

		obj->addComponent(field);
		scene->AddSceneObject(obj);
		return field;
	}
	
	CompositeCollider* SceneFactory::InitCompositeCollider()
	{
		// 1, 1 quarter
		CompositeCollider* col = Allocator::New<CompositeCollider>();
		col->Init();
		auto* subCol = Allocator::New<AABBCollider>();
		subCol->Init();
		subCol->setMinRelToPosition(glm::vec3(-2.5f, -0.5f, -2.5f));
		subCol->setMaxRelToPosition(glm::vec3(2.5f, 0.5f, 2.5f));
		col->addSubCollider(subCol, glm::vec3(2.5f, 0.0f, 2.5f));
		subCol = Allocator::New<AABBCollider>();
		subCol->Init();
		subCol->setMinRelToPosition(glm::vec3(-2.5f, -0.5f, -2.5f));
		subCol->setMaxRelToPosition(glm::vec3(2.5f, 0.5f, 2.5f));
		col->addSubCollider(subCol, glm::vec3(7.5f, 0.0f, 2.5f));
		subCol = Allocator::New<AABBCollider>();
		subCol->Init();
		subCol->setMinRelToPosition(glm::vec3(-2.5f, -0.5f, -2.5f));
		subCol->setMaxRelToPosition(glm::vec3(2.5f, 0.5f, 2.5f));
		col->addSubCollider(subCol, glm::vec3(7.5f, 0.0f, 7.5f));
		subCol = Allocator::New<AABBCollider>();
		subCol->Init();
		subCol->setMinRelToPosition(glm::vec3(-2.5f, -0.5f, -2.5f));
		subCol->setMaxRelToPosition(glm::vec3(2.5f, 0.5f, 2.5f));
		col->addSubCollider(subCol, glm::vec3(2.5f, 0.0f, 7.5f));

		// -1, 1 quarter
		subCol = Allocator::New<AABBCollider>();
		subCol->Init();
		subCol->setMinRelToPosition(glm::vec3(-2.5f, -0.5f, -2.5f));
		subCol->setMaxRelToPosition(glm::vec3(2.5f, 0.5f, 2.5f));
		col->addSubCollider(subCol, glm::vec3(-2.5f, 0.0f, 2.5f));
		subCol = Allocator::New<AABBCollider>();
		subCol->Init();
		subCol->setMinRelToPosition(glm::vec3(-2.5f, -0.5f, -2.5f));
		subCol->setMaxRelToPosition(glm::vec3(2.5f, 0.5f, 2.5f));
		col->addSubCollider(subCol, glm::vec3(-7.5f, 0.0f, 2.5f));
		subCol = Allocator::New<AABBCollider>();
		subCol->Init();
		subCol->setMinRelToPosition(glm::vec3(-2.5f, -0.5f, -2.5f));
		subCol->setMaxRelToPosition(glm::vec3(2.5f, 0.5f, 2.5f));
		col->addSubCollider(subCol, glm::vec3(-2.5f, 0.0f, 7.5f));
		subCol = Allocator::New<AABBCollider>();
		subCol->Init();
		subCol->setMinRelToPosition(glm::vec3(-2.5f, -0.5f, -2.5f));
		subCol->setMaxRelToPosition(glm::vec3(2.5f, 0.5f, 2.5f));
		col->addSubCollider(subCol, glm::vec3(-7.5f, 0.0f, 7.5f));

		// 1, -1 quarter
		subCol = Allocator::New<AABBCollider>();
		subCol->Init();
		subCol->setMinRelToPosition(glm::vec3(-2.5f, -0.5f, -2.5f));
		subCol->setMaxRelToPosition(glm::vec3(2.5f, 0.5f, 2.5f));
		col->addSubCollider(subCol, glm::vec3(2.5f, 0.0f, -2.5f));
		subCol = Allocator::New<AABBCollider>();
		subCol->Init();
		subCol->setMinRelToPosition(glm::vec3(-2.5f, -0.5f, -2.5f));
		subCol->setMaxRelToPosition(glm::vec3(2.5f, 0.5f, 2.5f));
		col->addSubCollider(subCol, glm::vec3(7.5f, 0.0f, -2.5f));
		subCol = Allocator::New<AABBCollider>();
		subCol->Init();
		subCol->setMinRelToPosition(glm::vec3(-2.5f, -0.5f, -2.5f));
		subCol->setMaxRelToPosition(glm::vec3(2.5f, 0.5f, 2.5f));
		col->addSubCollider(subCol, glm::vec3(2.5f, 0.0f, -7.5f));
		subCol = Allocator::New<AABBCollider>();
		subCol->Init();
		subCol->setMinRelToPosition(glm::vec3(-2.5f, -0.5f, -2.5f));
		subCol->setMaxRelToPosition(glm::vec3(2.5f, 0.5f, 2.5f));
		col->addSubCollider(subCol, glm::vec3(7.5f, 0.0f, -7.5f));

		// -1, -1 quarter
		subCol = Allocator::New<AABBCollider>();
		subCol->Init();
		subCol->setMinRelToPosition(glm::vec3(-2.5f, -0.5f, -2.5f));
		subCol->setMaxRelToPosition(glm::vec3(2.5f, 0.5f, 2.5f));
		col->addSubCollider(subCol, glm::vec3(-2.5f, 0.0f, -2.5f));
		subCol = Allocator::New<AABBCollider>();
		subCol->Init();
		subCol->setMinRelToPosition(glm::vec3(-2.5f, -0.5f, -2.5f));
		subCol->setMaxRelToPosition(glm::vec3(2.5f, 0.5f, 2.5f));
		col->addSubCollider(subCol, glm::vec3(-7.5f, 0.0f, -2.5f));
		subCol = Allocator::New<AABBCollider>();
		subCol->Init();
		subCol->setMinRelToPosition(glm::vec3(-2.5f, -0.5f, -2.5f));
		subCol->setMaxRelToPosition(glm::vec3(2.5f, 0.5f, 2.5f));
		col->addSubCollider(subCol, glm::vec3(-2.5f, 0.0f, -7.5f));
		subCol = Allocator::New<AABBCollider>();
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
		auto* cubeMesh = Allocator::New<Mesh>();
		cubeMesh->Init(material, cubeGeometry);
		auto* obj = Allocator::New<SceneObject>();
		obj->Init(cubeMesh);
		obj->SetPosition(pos);
		obj->SetScale(glm::vec3(10.0f, 0.5f, 10.0f));

		auto* cubePhysics = Allocator::New<Physics>();
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
		scene->AddCollider(collider);
		scene->AddSceneObject(obj, "cube");
	}
	
	SceneObject* SceneFactory::InitSphere(Scene* scene, const glm::vec3& pos, ForceField* field, const char* materialName)
	{
		SphericalCollider* collider = Allocator::New<SphericalCollider>();
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
		auto* mesh = Allocator::New<Mesh>();
		mesh->Init(material, geometry);
		auto* obj = Allocator::New<SceneObject>();
		obj->Init(mesh);
		obj->SetPosition(pos);
		obj->SetScale(glm::vec3(0.5f, 0.5f, 0.5f));

		auto* physics = Allocator::New<Physics>();
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
		scene->AddSceneObject(obj, std::string("sphere").append(materialName), "DeferredLayer");
		return obj;
	}
	
	void SceneFactory::InitCaptions(Scene* scene)
	{
		auto* font = Allocator::New<Font>();
		font->Init("arial.ttf");

		Caption* caption1 = Allocator::New<Caption>();
		caption1->Init("Homebrew Graphics Engine Demo", font,
			glm::vec2(GlobalVariables::renderResolutionWidth / 2, GlobalVariables::renderResolutionHeight * 0.95), 1.5f, glm::vec4(1, 1, 1, 1));
		scene->AddCaption(caption1);
		Allocator::Delete(font);
	}

	void SceneFactory::InitVoxelCaption(Scene* scene, const char* dataSetName) {
		auto* font = Allocator::New<Font>();
		font->Init("arial.ttf");
		auto* caption1 = Allocator::New<Caption>();
		caption1->Init("Volume rendering", font,
			glm::vec2(GlobalVariables::renderResolutionWidth / 2, GlobalVariables::renderResolutionHeight * 0.96), 1.0f, glm::vec4(1, 1, 1, 1));
		scene->AddCaption(caption1);

		auto* caption2 = Allocator::New<Caption>();
		caption2->Init(std::string("Dataset: ").append(dataSetName), font,
			glm::vec2(GlobalVariables::renderResolutionWidth / 2, GlobalVariables::renderResolutionHeight * 0.93), 1.0f, glm::vec4(1, 1, 1, 1));
		scene->AddCaption(caption2);

		caption2 = Allocator::New<Caption>();
		caption2->Init(std::string("Toggle transfer function [H]"), font,
			glm::vec2(GlobalVariables::renderResolutionWidth * 0.9f, GlobalVariables::renderResolutionHeight * 0.03), 1.0f, glm::vec4(1, 1, 1, 1));
		scene->AddCaption(caption2);

		caption2 = Allocator::New<Caption>();
		caption2->Init(std::string("Toggle options [O]"), font,
			glm::vec2(GlobalVariables::renderResolutionWidth * 0.1f, GlobalVariables::renderResolutionHeight * 0.03), 1.0f, glm::vec4(1, 1, 1, 1));
		scene->AddCaption(caption2);

		Allocator::Delete(font);
	}
	
	void SceneFactory::InitGroud(Scene* scene)
	{
		for (int i = 0; i < 10; i++) {
			for (int j = 0; j < 10; j++) {
				auto* material = MaterialFactory::GetInstance()->getPBRMaterial("vinyl");
				Geometry* cubeGeometry = GeometryFactory::GetInstance()->getCube();
				auto* cubeMesh = Allocator::New<Mesh>();
				cubeMesh->Init(material, cubeGeometry);
				auto* obj = Allocator::New<SceneObject>();
				obj->Init(cubeMesh);
				obj->SetPosition(glm::vec3(i * 100.0f - 500.0f, 0.0f, j * 100.0f - 500.0f));
				obj->SetScale(glm::vec3(50.0f, 1.0f, 50.0f));

				auto* cubePhysics = Allocator::New<Physics>();
				cubePhysics->Init(obj);
				cubePhysics->setPositionForcingLevel(0.0f);
				auto* collider = Allocator::New<AABBCollider>();
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
		ShaderProgram* skyboxShader = Allocator::New<ShaderProgram>();
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
		auto* cubeMap = Allocator::New<TextureCube>();
		cubeMap->Init(imagePaths, SKYBOX_UNIT);
		auto* skyBoxMaterial = Allocator::New<Material>();
		skyBoxMaterial->Init(skyboxShader);
		skyBoxMaterial->addTexture(cubeMap);
		Geometry* fullscreenQuad = GeometryFactory::GetInstance()->getFullScreenQuad();
		auto* skyBoxMesh = Allocator::New<Mesh>();
		skyBoxMesh->Init(skyBoxMaterial, fullscreenQuad);
		skyBoxMesh->setDepthTest(false);
		skyBoxMesh->setStencilTest(false);
		auto* obj = Allocator::New<SceneObject>();
		obj->Init(skyBoxMesh);
		scene->AddSceneObject(obj, "skybox", "DeferredLayer");
	}
	
	void SceneFactory::InitLoadedGeometry(Scene* scene, const glm::vec3& pos, ForceField* field)
	{
		SphericalCollider* collider = Allocator::New<SphericalCollider>();
		collider->Init();
		collider->SetRadius(0.5f);
		scene->AddCollider(collider);
		ShaderProgram* shader = ShaderProgramFactory::GetInstance()->GetDefaultPBRProgramWithMapping();
		auto* material = MaterialFactory::GetInstance()->getPBRMaterial("planks");
		Geometry* geometry = GeometryLoader().Load(AssetFolderPathManager::getInstance()->getGeometryFolderPath().append("mango.obj"));
		geometry->SetFaceCulling(false);
		auto* mesh = Allocator::New<Mesh>();
		mesh->Init(material, geometry);
		auto* obj = Allocator::New<SceneObject>();
		obj->Init(mesh);
		obj->SetPosition(pos);
		obj->SetScale(glm::vec3(0.5f, 0.5f, 0.5f));
		collider->SetPositionProvider(obj);
		collider->SetOrientationProvider(obj);
		auto* physics = Allocator::New<Physics>();
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
		auto* avatar = Allocator::New<SceneObject>();
		avatar->Init();
		avatar->SetPosition(glm::vec3(-60.0f, 2.0f, -60.0f));
		auto* posConnector = Allocator::New<PositionConnector>();
		posConnector->Init(avatar, glm::vec3(0.0f, 0.8f, 0.0f));
		scene->GetCamera().SetPositionConnetor(posConnector);
		auto* collider = Allocator::New<AABBCollider>();
		collider->Init();
		collider->setMinRelToPosition(glm::vec3(-0.2f, -1.0f, -0.2f));
		collider->setMaxRelToPosition(glm::vec3(0.2f, 1.0f, 0.2f));
		auto* physics = Allocator::New<Physics>();
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
		control = Allocator::New<FirstPersonControl>();
		control->SetScene(scene);
		control->setPhysics(physics);
		control->setCamera(&scene->GetCamera());
		control->setInitialDirection(glm::normalize(glm::vec3(1, 0, 1)));
		control->setInitialUp(scene->getPreferedUp());
		control->setJumpImpulse(600.0f);
		control->setPropellingForce(2000.0f);
		auto* jumpCollider = Allocator::New<AABBCollider>();
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

	ObserveObjectControl* SceneFactory::InitObjectObserverControl(Scene* scene, Volumetric::VolumeObject* volumeObject)
	{
		auto control = Allocator::New<ObserveObjectControl>();
		control->SetCamera(scene->GetCamera());
		if (nullptr != volumeObject) {
			control->SetVolumeObject(*volumeObject);
		}
		control->SetScene(scene);
		auto collider = Allocator::New<AABBCollider>();
		collider->Init();
		collider->SetMin(glm::vec3(-2.0f, -2.0f, -2.0f));
		collider->SetMax(glm::vec3(2.0f, 2.0f, 2.0f));
		collider->SetPosition(glm::vec3(0,0,0));
		control->AddCollider(collider);
		scene->AddCollider(collider, "volumePlane");
		scene->SetUserControl(control);
		return control;
	}
	
	void SceneFactory::InitLaserBeam(Hogra::Scene* scene, Hogra::FirstPersonControl* control)
	{
		auto obj = Allocator::New<SceneObject>();
		auto geom = GeometryFactory::GetInstance()->getCilinder();
		auto material = MaterialFactory::GetInstance()->getEmissiveMaterial("laser", glm::vec3(1.0f, 0.0f, 0.0f), 100.0f);
		auto mesh = Allocator::New<Mesh>();
		mesh->Init(material, geom);
		obj->Init(mesh);
		obj->SetIsVisible(false);
		obj->SetIsCastingShadow(false);
		scene->AddSceneObject(obj);
		control->SetLaserObject(obj);
		Light* laserInpactLight = Allocator::New<Light>();
		laserInpactLight->Init(glm::vec4(0.0f, 0.0f, 0.0f, 1.0f), glm::vec3(25.0f, 20.0f, 10.0f));
		scene->AddLight(laserInpactLight);
		control->SetLaserInpactLight(laserInpactLight);
	}

	void SceneFactory::InitAudio(Scene* scene, FirstPersonControl* control)
	{


		auto buffer = Allocator::New<AudioBuffer>();
		buffer->Init(AssetFolderPathManager::getInstance()->getSoundsFolderPath().append("human-impact.wav"));

		auto source = Allocator::New<AudioSource>();
		source->Init(buffer);
		source->SetGain(0.3f);

		auto sceneSource = Allocator::New<SceneAudioSource>();
		sceneSource->Init(source);
		scene->AddSceneAudioSource(sceneSource);
		control->SetJumpAudioSource(sceneSource);
		
		// Steps:
		auto footstepsBuffer = Allocator::New<AudioBuffer>();
		footstepsBuffer->Init(AssetFolderPathManager::getInstance()->getSoundsFolderPath().append("footsteps.wav"));
		auto footstepsSource = Allocator::New<AudioSource>();
		footstepsSource->Init(footstepsBuffer);
		auto footstepsSceneAudioSource = Allocator::New<SceneAudioSource>();
		footstepsSceneAudioSource->Init(footstepsSource);
		scene->AddSceneAudioSource(footstepsSceneAudioSource);
		control->SetFootstepAudioSource(footstepsSceneAudioSource);

		// Laser:
		auto laserBuffer = Allocator::New<AudioBuffer>();
		laserBuffer->Init(AssetFolderPathManager::getInstance()->getSoundsFolderPath().append("bruteLaser.wav"));
		auto laserSource = Allocator::New<AudioSource>();
		laserSource->Init(laserBuffer);
		auto laserSceneAudioSource = Allocator::New<SceneAudioSource>();
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
		auto ambientBuffer = Allocator::New<AudioBuffer>();
		ambientBuffer->Init(AssetFolderPathManager::getInstance()->getSoundsFolderPath().append("wind.wav"));
		auto ambientSource = Allocator::New<AudioSource>();
		ambientSource->Init(ambientBuffer);
		auto ambientSceneSource = Allocator::New<SceneAudioSource>();
		ambientSceneSource->Init(ambientSource);
		scene->AddSceneAudioSource(ambientSceneSource);
		ambientSceneSource->SetLoop(true);
		ambientSceneSource->Play();
	}
	
	SceneAudioSource* SceneFactory::buildAudioSource(const std::string& fileName) {
		auto buffer = Allocator::New<AudioBuffer>();
		buffer->Init(AssetFolderPathManager::getInstance()->getSoundsFolderPath().append(fileName));
		auto source = Allocator::New<AudioSource>();
		source->Init(buffer);
		auto sceneSource = Allocator::New<SceneAudioSource>();
		sceneSource->Init(source);
		return sceneSource;
	}

	Scene* SceneFactory::LoadSceneFromFile(const std::string& fileName)
	{
		// Not working source files missing!
		//YAML::Node sceneNode = YAML::LoadFile(AssetFolderPathManager::getInstance()->getScenesFolderPath().append(fileName));
		
		// TODO

		auto* scene = Allocator::New<Scene>();
		return scene;
	}
}
