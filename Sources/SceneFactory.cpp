﻿#include "SceneFactory.h"
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
#include "Callbacks.h"
#include "ScriptObject.h"
#include "UniformVariableImpl.h"
#include "HograTime.h"
#include "fallingSand/chunk.h"
#include "fallingSand/water.h"
#include "fallingSand/sand.h"
#include "fallingSand/stone.h"
#include "fallingSand/smoke.h"
#include "fallingSand/lava.h"

#include "nlohmann/json.hpp"
#include "jsonParseUtils.h"


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
		auto* scene = Allocator::New<Scene>();
		scene->Init(contextWidth, contextHeight);
		auto* forwardLayer = Allocator::New<RenderLayer>();
		forwardLayer->SetRenderMode(RenderLayer::RenderMode::forwardRenderMode);
		forwardLayer->SetName("ForwardLayer");
		scene->AddRenderLayer(forwardLayer);
		auto* defLayer = Allocator::New<RenderLayer>();
		defLayer->SetRenderMode(RenderLayer::RenderMode::deferredInstancedRenderMode);
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
		
		InitSkyBox(scene);

		InitGround(scene);

		
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
			obj->AddComponent(light);
			auto* provider = Allocator::New<PositionConnector>();
			provider->Init(obj);
			light->SetPositionProvider(provider);
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
		mario->AddComponent(marioColl);
		
		mario->SetPosition(glm::vec3(-11.0f, 4.0f, -20.0f));
		scene->AddSceneObject(mario, "mario_sprite", "ForwardLayer");

		InitLoadedGeometry(scene, glm::vec3(-10.0f, 3.0f, -30.0f), field);

		FirstPersonControl* control = nullptr;
		InitAvatar(scene, field, control);
		InitCaptions(scene);

		InitAudio(scene, control);

		// Post processing:
		{
			auto* stage0 = Allocator::New<PostProcessStage>();
			stage0->Init(AssetFolderPathManager::getInstance()->getShaderFolderPath().append("depthEffects.frag"),
				contextWidth, contextHeight);
			scene->AddPostProcessStage(stage0, "DeferredLayer");

			auto* bloom = Allocator::New<Bloom>();
			bloom->Init(contextWidth, contextHeight);
			scene->AddPostProcessStage(bloom, "DeferredLayer");

			auto* stage1 = Allocator::New<PostProcessStage>();
			stage1->Init(AssetFolderPathManager::getInstance()->getShaderFolderPath().append("hdr.frag"),
				contextWidth, contextHeight);
			scene->AddPostProcessStage(stage1, "DeferredLayer");
		}

		//Control:
		{
			auto* control = static_cast<FirstPersonControl*>(scene->GetUserControl());
			auto* fullScreen = Allocator::New<ButtonKeyAction>();
			fullScreen->Init(GLFW_KEY_TAB, ButtonKeyAction::TriggerType::triggerOnPress);
			fullScreen->SetAction(
				[]() {
					Callbacks::toggleFullScreen();
				}
			);
			ControlActionManager::getInstance()->RegisterKeyAction(fullScreen);

			auto* toggleHide = Allocator::New<ButtonKeyAction>();
			toggleHide->Init(GLFW_KEY_ESCAPE, ButtonKeyAction::TriggerType::triggerOnRelease);
			toggleHide->SetAction(
				[]() {
					GlobalVariables::hideCursor = !GlobalVariables::hideCursor;
				}
			);
			ControlActionManager::getInstance()->RegisterKeyAction(toggleHide);

			auto* moveForward = Allocator::New<ButtonKeyAction>();
			moveForward->Init(GLFW_KEY_W, ButtonKeyAction::TriggerType::triggerContinuosly);
			moveForward->SetAction(
				[control]() {
					control->MoveForward();
				}
			);
			ControlActionManager::getInstance()->RegisterKeyAction(moveForward);

			auto* moveBackward = Allocator::New<ButtonKeyAction>();
			moveBackward->Init(GLFW_KEY_S, ButtonKeyAction::TriggerType::triggerContinuosly);
			moveBackward->SetAction(
				[control]() {
					control->MoveBackward();
				}
			);
			ControlActionManager::getInstance()->RegisterKeyAction(moveBackward);

			auto* moveLeft = Allocator::New<ButtonKeyAction>();
			moveLeft->Init(GLFW_KEY_A, ButtonKeyAction::TriggerType::triggerContinuosly);
			moveLeft->SetAction(
				[control]() {
					control->MoveLeft();
				}
			);
			ControlActionManager::getInstance()->RegisterKeyAction(moveLeft);

			auto* moveRight = Allocator::New<ButtonKeyAction>();
			moveRight->Init(GLFW_KEY_D, ButtonKeyAction::TriggerType::triggerContinuosly);
			moveRight->SetAction(
				[control]() {
					control->MoveRight();
				}
			);
			ControlActionManager::getInstance()->RegisterKeyAction(moveRight);

			auto* jump = Allocator::New<ButtonKeyAction>();
			jump->Init(GLFW_KEY_SPACE, ButtonKeyAction::TriggerType::triggerOnPress);
			jump->SetAction(
				[control]() {
					control->Jump();
				}
			);
			ControlActionManager::getInstance()->RegisterKeyAction(jump);

			auto* moveCam = Allocator::New<AxisMoveAction>();
			moveCam->SetAction(
			[control](const glm::vec2& delta, const glm::vec2& pos) {
					control->Rotate(-delta);
				}
			);
			ControlActionManager::getInstance()->RegisterMouseMoveAction(moveCam);
			
			// Shooting:
			auto* shoot = Allocator::New<ButtonKeyAction>();
			shoot->Init(GLFW_MOUSE_BUTTON_LEFT, ButtonKeyAction::TriggerType::triggerContinuosly);
			shoot->SetAction(
				[control]() {
					control->primaryAction();
				}
			);
			ControlActionManager::getInstance()->RegisterMouseButtonAction(shoot);

		}
		return scene;
	}

	Scene* SceneFactory::CreateEasyScene(int contextWidth, int contextHeight)
	{
		auto* scene = Allocator::New<Scene>();
		scene->Init(contextWidth, contextHeight);
		auto* defLayer = Allocator::New<RenderLayer>();
		defLayer->SetRenderMode(RenderLayer::RenderMode::deferredInstancedRenderMode);
		defLayer->SetName("DeferredLayer");
		scene->AddRenderLayer(defLayer);

		InitSkyBox(scene);
		InitGround(scene);
		InitSphere(scene, glm::vec3(0, 2, 0), nullptr, "planks");
		InitSphere(scene, glm::vec3(1, 2, 1), nullptr, "planks");

		auto* light = Allocator::New<Light>();
		light->Init(glm::normalize(glm::vec4(-1.0f, 1.0f, -1.0f, 0.0f)), glm::vec3(1.0f, 1.0f, 1.0f));
		scene->AddLight(light);	// Directional light
		light = Allocator::New<Light>();
		light->Init(glm::normalize(glm::vec4(-3.0f, 5.0f, 10.0f, 1.0f)), glm::vec3(6.0f, 5.5f, 5.0f));
		scene->AddLight(light);	// Point light

		auto* bloom = Allocator::New<Bloom>();
		bloom->Init(contextWidth, contextHeight);
		scene->AddPostProcessStage(bloom, "DeferredLayer");

		return scene;
	}
	
	static bool isCrop = false;
	Scene* SceneFactory::CreateVoxelDemoScene(int contextWidth, int contextHeight)
	{

		//Test mem allocator:
		Texture2D* textureInstance = Allocator::New<Hogra::Texture2D>();

		Scene* scene = Allocator::New<Scene>();
		scene->Init(contextWidth, contextHeight);
		scene->SetBackgroundColor(glm::vec3(0.91f, 0.93f, 1.0f));
		
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
		const char* dataSetName = "cthead-8bit";
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
			glm::vec3(1000.0f, 1000.0f, 1000.0f)
		);
		volumeLight->SetCastShadow(false);

		auto* bulbSprite = SceneObjectFactory::GetInstance()->Create2DSpriteObject(AssetFolderPathManager::getInstance()->getTextureFolderPath().append("sprites/lightbulb.png"), &scene->GetCamera());
		auto* posConnector = Allocator::New<PositionConnector>();
		posConnector->Init(volumeLight);
		bulbSprite->SetPositionConnector(posConnector);
		scene->AddSceneObject(bulbSprite, "bulbSprite", "ForwardLayer");
		auto* camPosProvider = Allocator::New<PositionConnector>();
		camPosProvider->Init(&(scene->GetCamera()), glm::vec3(5,1,0));
		//volumeLight->SetPositionProvider(camPosProvider);
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
		volumeSceneObj->AddComponent(volumeObject);
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
			/*
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

				{
					auto* enableCrop = Allocator::New<ButtonKeyAction>();
					enableCrop->Init(GLFW_KEY_C, ButtonKeyAction::TriggerType::triggerOnPress);
					enableCrop->SetAction(
						[control, volumeObject]() {
							isCrop = true;
							volumeObject->SetIsPlaneGrabbed(true);
						}
					);
					ControlActionManager::getInstance()->RegisterKeyAction(enableCrop);
					auto* disableCrop = Allocator::New<ButtonKeyAction>();
					disableCrop->Init(GLFW_KEY_C, ButtonKeyAction::TriggerType::triggerOnRelease);
					disableCrop->SetAction(
						[control, volumeObject]() {
							isCrop = false;
							volumeObject->SetIsPlaneGrabbed(false);
						}
					);
					ControlActionManager::getInstance()->RegisterKeyAction(disableCrop);
				}

			auto* nextFeature = Allocator::New<ButtonKeyAction>();
			nextFeature->Init(GLFW_KEY_SPACE, ButtonKeyAction::TriggerType::triggerOnPress);
			nextFeature->SetAction(
			[volumeObject]() {
					volumeObject->CycleSelectedFeature();
				}
			);
			ControlActionManager::getInstance()->RegisterKeyAction(nextFeature);

			auto* fullScreen = Allocator::New<ButtonKeyAction>();
			fullScreen->Init(GLFW_KEY_TAB, ButtonKeyAction::TriggerType::triggerOnPress);
			fullScreen->SetAction(
				[]() {
					Callbacks::toggleFullScreen();
				}
			);
			ControlActionManager::getInstance()->RegisterKeyAction(fullScreen);
			auto* leftClick = Allocator::New<ButtonKeyAction>();
			leftClick->Init(GLFW_MOUSE_BUTTON_LEFT, ButtonKeyAction::TriggerType::triggerContinuosly);
			leftClick->SetAction(
				[control, volumeObject, leftClick]() {
					double x;
					double y;
					glfwGetCursorPos(GlobalVariables::window, &x, &y);
					float ndc_x = x / (double)GlobalVariables::windowWidth * 2.0 - 1.0;
					float ndc_y = 1.0 - y / (double)GlobalVariables::windowHeight * 2.0;

					if (!isCrop) {
						bool isSuccess = volumeObject->SelectTransferFunctionRegion(ndc_x, ndc_y);
					}
					else {
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

			auto* fullScreen = Allocator::New<ButtonKeyAction>();
			fullScreen->Init(GLFW_KEY_TAB, ButtonKeyAction::TriggerType::triggerOnPress);
			fullScreen->SetAction(
				[scene]() {
					Callbacks::toggleFullScreen();
				}
			);
			ControlActionManager::getInstance()->RegisterKeyAction(fullScreen);

			auto* renderMode = Allocator::New<ButtonKeyAction>();
			renderMode->Init(GLFW_KEY_M, ButtonKeyAction::TriggerType::triggerOnPress);
			renderMode->SetAction(
				[volumeObject]() {
					volumeObject->ToggleHalfAngleSlicing();
				}
			);
			ControlActionManager::getInstance()->RegisterKeyAction(renderMode);

			auto* reloadShaders = Allocator::New<ButtonKeyAction>();
			reloadShaders->Init(GLFW_KEY_R, ButtonKeyAction::TriggerType::triggerOnPress);
			reloadShaders->SetAction(
				[]() {
					ShaderProgram::ReloadAll();
				}
			);
			ControlActionManager::getInstance()->RegisterKeyAction(reloadShaders);
		
					auto* rotateLightPos = Allocator::New<ButtonKeyAction>();
			rotateLightPos->Init(GLFW_KEY_EQUAL, ButtonKeyAction::TriggerType::triggerOnPress);
			rotateLightPos->SetAction(
				[volumeObject, volumeLight]() {
					auto pos = volumeLight->GetPosition();
					volumeLight->SetPosition(glm::angleAxis(0.1f, glm::vec3(0, 1, 0)) * pos);
					volumeObject->SetChanged();
				}
			);
			ControlActionManager::getInstance()->RegisterKeyAction(rotateLightPos);
			auto* rotateLightNeg = Allocator::New<ButtonKeyAction>();
			rotateLightNeg->Init(GLFW_KEY_MINUS, ButtonKeyAction::TriggerType::triggerOnPress);
			rotateLightNeg->SetAction(
				[volumeObject, volumeLight]() {
					auto pos = volumeLight->GetPosition();
					volumeLight->SetPosition(glm::angleAxis(-0.1f, glm::vec3(0, 1, 0))* pos);
					volumeObject->SetChanged();
				}
			);
			ControlActionManager::getInstance()->RegisterKeyAction(rotateLightNeg);
*/
		}

		return scene;
	}

	Scene* SceneFactory::CreatePixelPhysicsDemoScene(int contextWidth, int contextHeight) {

		auto scene = Allocator::New<Scene>();
		scene->Init(contextWidth, contextHeight);

		auto* forwardLayer = Allocator::New<RenderLayer>();
		forwardLayer->SetName("ForwardLayer");
		forwardLayer->SetRenderMode(RenderLayer::RenderMode::forwardRenderMode);
		scene->AddRenderLayer(forwardLayer);

		auto* program = Allocator::New<ShaderProgram>();
		program->Init(
			AssetFolderPathManager::getInstance()->getShaderFolderPath().append("fullscreenQuad.vert"),
			"",
			AssetFolderPathManager::getInstance()->getShaderFolderPath().append("bypass.frag")
		);

		auto* material = Allocator::New<Material>();
		material->Init(program);
		material->setAlphaBlend(true);
		auto* fullScreenQuad = GeometryFactory::GetInstance()->getFullScreenQuad();
		auto* mesh = Allocator::New<Mesh>();
		mesh->Init(material, fullScreenQuad);
		mesh->SetDepthTest(false);
		auto chunk = Allocator::New<FallingSand::Chunk>();
		chunk->Init(mesh);

		auto chunkObj = Allocator::New<SceneObject>();
		chunkObj->Init(mesh);


		// Sand:
		chunk->GetGrid().Write(100, 100, Allocator::New<FallingSand::Sand>());
		chunk->GetGrid().Write(100, 101, Allocator::New<FallingSand::Sand>());
		chunk->GetGrid().Write(100, 102, Allocator::New<FallingSand::Sand>());
		chunk->GetGrid().Write(100, 103, Allocator::New<FallingSand::Sand>());
		chunk->GetGrid().Write(100, 104, Allocator::New<FallingSand::Sand>());
		chunk->GetGrid().Write(101, 100, Allocator::New<FallingSand::Sand>());
		chunk->GetGrid().Write(101, 101, Allocator::New<FallingSand::Sand>());
		chunk->GetGrid().Write(101, 102, Allocator::New<FallingSand::Sand>());
		chunk->GetGrid().Write(101, 103, Allocator::New<FallingSand::Sand>());
		chunk->GetGrid().Write(101, 104, Allocator::New<FallingSand::Sand>());
		chunk->GetGrid().Write(102, 100, Allocator::New<FallingSand::Sand>());
		chunk->GetGrid().Write(102, 101, Allocator::New<FallingSand::Sand>());
		chunk->GetGrid().Write(102, 102, Allocator::New<FallingSand::Sand>());
		chunk->GetGrid().Write(102, 103, Allocator::New<FallingSand::Sand>());
		chunk->GetGrid().Write(102, 104, Allocator::New<FallingSand::Sand>());
		chunk->GetGrid().Write(103, 100, Allocator::New<FallingSand::Sand>());
		chunk->GetGrid().Write(103, 101, Allocator::New<FallingSand::Sand>());
		chunk->GetGrid().Write(103, 102, Allocator::New<FallingSand::Sand>());
		chunk->GetGrid().Write(103, 103, Allocator::New<FallingSand::Sand>());
		chunk->GetGrid().Write(103, 104, Allocator::New<FallingSand::Sand>());

		for (int i = 0; i < 256; i++) {
			chunk->GetGrid().Write(i, 3, Allocator::New<FallingSand::Stone>());
			chunk->GetGrid().Write(i, 2, Allocator::New<FallingSand::Stone>());
			chunk->GetGrid().Write(i, 1, Allocator::New<FallingSand::Stone>());
			chunk->GetGrid().Write(i, 0, Allocator::New<FallingSand::Stone>());
		}
		chunk->GetGrid().Write(0, 4, Allocator::New<FallingSand::Stone>());
		chunk->GetGrid().Write(0, 5, Allocator::New<FallingSand::Stone>());
		chunk->GetGrid().Write(255, 4, Allocator::New<FallingSand::Stone>());
		chunk->GetGrid().Write(255, 5, Allocator::New<FallingSand::Stone>());

		chunkObj->AddComponent(chunk);

		auto* bloom = Allocator::New<Bloom>();
			bloom->Init(contextWidth, contextHeight);
		scene->AddPostProcessStage(bloom, "ForwardLayer");

		auto* hdr = Allocator::New<PostProcessStage>();
		hdr->Init(
			AssetFolderPathManager::getInstance()->getShaderFolderPath().append("hdr.frag"),
			contextWidth, contextHeight);
		scene->AddPostProcessStage(hdr, "ForwardLayer");

		auto fbo = Allocator::New<FBO>();
		fbo->Init();
		auto drawMode = Allocator::New<int>();
		*drawMode = 0;


		scene->AddSceneObject(chunkObj, "", "ForwardLayer");

		{
			auto* fullScreen = Allocator::New<ButtonKeyAction>();
			fullScreen->Init(GLFW_KEY_TAB, ButtonKeyAction::TriggerType::triggerOnPress);
			fullScreen->SetAction(
				[]() {
					Callbacks::toggleFullScreen();
				}
			);
			ControlActionManager::getInstance()->RegisterKeyAction(fullScreen);

			auto* reloadShaders = Allocator::New<ButtonKeyAction>();
			reloadShaders->Init(GLFW_KEY_R, ButtonKeyAction::TriggerType::triggerOnPress);
			reloadShaders->SetAction(
				[]() {
					ShaderProgram::ReloadAll();
				}
			);
			ControlActionManager::getInstance()->RegisterKeyAction(reloadShaders);

			auto* switchDrawMode = Allocator::New<ButtonKeyAction>();
			switchDrawMode->Init(GLFW_KEY_T, ButtonKeyAction::TriggerType::triggerOnPress);
			switchDrawMode->SetAction(
				[drawMode]() {
					(*drawMode)++;
					if (*drawMode > 4) {
						*drawMode = 0;
					}
				}
			);
			ControlActionManager::getInstance()->RegisterKeyAction(switchDrawMode);

			auto* leftClick = Allocator::New<ButtonKeyAction>();
			leftClick->Init(GLFW_MOUSE_BUTTON_LEFT, ButtonKeyAction::TriggerType::triggerContinuosly);
			leftClick->SetAction(
				[chunk, drawMode]() {
					double x;
					double y;
					glfwGetCursorPos(GlobalVariables::window, &x, &y);
					float u = x / (double)GlobalVariables::windowWidth * 255.0f;
					float v = 255.0f - y / (double)GlobalVariables::windowHeight * 255.0f;

					FallingSand::Particle* particle[9] = { nullptr };
					switch (*drawMode) {
					case 0:
						for (int i = 0; i < 9; i++) {
							particle[i] = Allocator::New<FallingSand::Water>();
						}
						break;
					case 1:
						for (int i = 0; i < 9; i++) {
							particle[i] = Allocator::New<FallingSand::Sand>();
						}
						break;
					case 2:
						for (int i = 0; i < 9; i++) {
							particle[i] = Allocator::New<FallingSand::Stone>();
						}
						break;
					case 3:
						for (int i = 0; i < 9; i++) {
							particle[i] = Allocator::New<FallingSand::Smoke>();
						}
						break;
					case 4:
						for (int i = 0; i < 9; i++) {
							particle[i] = Allocator::New<FallingSand::Lava>();
						}
						break;
					}
					if (u > 0 && u < 255 && v > 0 && v < 255) {
						chunk->GetGrid().PutIfEmpty(u - 1, v - 1, particle[0]);
						chunk->GetGrid().PutIfEmpty(u + 0, v - 1, particle[1]);
						chunk->GetGrid().PutIfEmpty(u + 1, v - 1, particle[2]);
						chunk->GetGrid().PutIfEmpty(u - 1, v + 0, particle[3]);
						chunk->GetGrid().PutIfEmpty(u + 0, v + 0, particle[4]);
						chunk->GetGrid().PutIfEmpty(u + 1, v + 0, particle[5]);
						chunk->GetGrid().PutIfEmpty(u - 1, v + 1, particle[6]);
						chunk->GetGrid().PutIfEmpty(u + 0, v + 1, particle[7]);
						chunk->GetGrid().PutIfEmpty(u + 1, v + 1, particle[8]);

					}
				}
			);
			ControlActionManager::getInstance()->RegisterMouseButtonAction(leftClick);
		}

		return scene;
	}

	
	ForceField* SceneFactory::InitGravitation(Scene* scene)
	{
		SceneObject* obj = Allocator::New<SceneObject>();
		obj->Init();
		obj->SetIsVisible(false);
		auto* field = Allocator::New<HomogeneForceField>();
		field->SetStrength(9.8f);
		obj->AddComponent(field);
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
		obj->AddComponent(cubePhysics);
		collider->SetPhysics(cubePhysics);
		collider->SetPositionProvider(obj);
		collider->SetOrientationProvider(obj);
		obj->AddComponent(collider);
		scene->AddCollider(collider);
		scene->AddSceneObject(obj, "cube", "DeferredLayer");
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
		physics->setWorldSpaceDrag(glm::vec3(10.0f, 10.0f, 10.0f));
		physics->setMass(50.0f);
		physics->setMomentOfInertia(Physics::getMomentOfInertiaOfSolidSphere(physics->getMass(), 0.5f));
		physics->setRotationalDrag(glm::vec3(5.0f, 5.0f, 5.0f));
		physics->setPositionForcingLevel(1.0f);
		physics->setElasticity(0.95f);
		physics->setFriction(1.0f);
		if (field != nullptr) {
			field->AddListener(physics);
		}
		obj->AddComponent(physics);
		collider->SetPhysics(physics);
		collider->SetPositionProvider(obj);
		collider->SetOrientationProvider(obj);
		obj->AddComponent(collider);
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
	
	void SceneFactory::InitGround(Scene* scene)
	{
		for (int i = 0; i < 10; i++) {
			for (int j = 0; j < 10; j++) {
				auto* material = MaterialFactory::GetInstance()->getPBRMaterial("vinyl");
				Geometry* cubeGeometry = GeometryFactory::GetInstance()->getCube();
				auto* cubeMesh = Allocator::New<Mesh>();
				cubeMesh->Init(material, cubeGeometry);
				cubeMesh->SetDepthTest(true);
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
				obj->AddComponent(cubePhysics);
				collider->SetPhysics(cubePhysics);
				collider->setMinRelToPosition(glm::vec3(-49.99f, -1.0f, -49.99f));
				collider->setMaxRelToPosition(glm::vec3(49.99f, 1.0f, 49.99f));
				collider->SetPositionProvider(obj);
				obj->AddComponent(collider);
				scene->AddCollider(collider, "ground");
				scene->AddSceneObject(obj, "ground", "DeferredLayer");
			}
		}
	}
	
	void SceneFactory::InitSkyBox(Scene* scene)
	{
		ShaderProgram* skyboxShader = Allocator::New<ShaderProgram>();
		skyboxShader->Init(
			AssetFolderPathManager::getInstance()->getShaderFolderPath().append("fullscreenQuad.vert"),
			"",
			AssetFolderPathManager::getInstance()->getShaderFolderPath().append("forwardSkybox.frag")
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
		skyBoxMesh->SetDepthTest(false);
		skyBoxMesh->setStencilTest(false);
		auto* obj = Allocator::New<SceneObject>();
		obj->Init(skyBoxMesh);
		scene->AddSceneObject(obj, "skybox", "ForwardLayer");
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
		obj->AddComponent(physics);
		collider->SetPhysics(physics);
		obj->AddComponent(collider);
		scene->AddSceneObject(obj, "DeferredLayer");
	}
	
	void SceneFactory::InitAvatar(Scene* scene, ForceField* gravitation, FirstPersonControl*& control)
	{
		auto* avatar = Allocator::New<SceneObject>();
		avatar->Init();
		avatar->SetIsVisible(false);
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
		avatar->AddComponent(jumpCollider);
		scene->AddCollider(jumpCollider, "avatar");
		scene->SetUserControl(control);
		avatar->AddComponent(control);
		avatar->AddComponent(physics);
		avatar->AddComponent(collider);
		scene->getShadowCaster()->SetPositionProvider(avatar);
		scene->getShadowCaster()->setPositionOffsetToProvider(glm::vec3(-20, 20, -20));
		scene->AddSceneObject(avatar, "DeferredLayer");
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
		mesh->SetDepthTest(false);
		mesh->Init(material, geom);
		obj->Init(mesh);
		obj->SetPosition(glm::vec3(0,5,0));
		obj->SetIsVisible(true);
		obj->SetIsCastingShadow(false);
		scene->AddSceneObject(obj, "DeferredLayer");
		control->SetLaserObject(obj);
		Light* laserInpactLight = Allocator::New<Light>();
		laserInpactLight->Init(glm::vec4(0.0f, 10.0f, 0.0f, 1.0f), glm::vec3(25.0f, 20.0f, 10.0f));
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

	Scene* SceneFactory::LoadSceneFromFile(const std::string& path)
	{
		auto inputFile = std::ifstream(path);
		if (inputFile.is_open()) {
			auto jsonData = nlohmann::json::parse(inputFile);
			inputFile.close();

			auto* scene = Allocator::New<Scene>();

			//Basic scene:
			scene->id = jsonData["id"];
			scene->name = jsonData["name"];
			scene->pause = jsonData["pause"];
			scene->debugMode = jsonData["debugMode"];
			scene->preferedUp = parseVec3(jsonData["preferedUp"]);
			scene->backgroundColor = parseVec4(jsonData["backgroundColor"]);
			scene->Init(GlobalVariables::renderResolutionWidth, GlobalVariables::renderResolutionHeight);

			//Camera:
			Camera camera;
			auto camData = jsonData["camera"];
			camera.prefUp = parseVec3(camData["prefUp"]);
			camera.FOVdeg = camData["FOVdeg"];
			camera.nearPlane = camData["nearPlane"];
			camera.farPlane = camData["farPlane"];
			camera.Init((float)GlobalVariables::windowWidth / (float)GlobalVariables::windowHeight, parseVec3(camData["eye"]), parseVec3(camData["lookAt"]));
			scene->camera = camera;

			//Lights:
			for (auto& lightData : jsonData["lights"]) {
				auto* l = Allocator::New<Light>();
				l->SetId(lightData["id"]);
				l->SetName(lightData["name"]);
				l->Init(parseVec4(lightData["position"]), parseVec3(lightData["powerDensity"]));
				l->SetIsActive(lightData["isActive"]);
				l->SetCastShadow(lightData["castShadow"]);
				scene->AddLight(l);
			}


			return scene;
		}
		return nullptr;
	}
}
