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
#include "Callbacks.h"

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
	
	static bool isCrop = false;
	Scene* SceneFactory::CreateVoxelDemoScene(int contextWidth, int contextHeight, int argc, char* argv[])
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

		auto* light = Allocator::New<Light>();
		light->Init(glm::normalize(glm::vec4(-1.0f, 1.0f, -1.0f, 0.0f)), glm::vec3(1.0f, 1.0f, 1.0f));
		scene->AddLight(light);	// Directional light

		// Volume:
		std::string dataSetName;
		if (argc == 1) {	// Load default
			dataSetName = "cthead-8bit";
		}
		else {
			dataSetName = argv[1];
		}

		auto* voxelTexture = Allocator::New<Texture3D>();
		
		voxelTexture->Init(
			AssetFolderPathManager::getInstance()->getTextureFolderPath().append(dataSetName),
			3,
			GL_RED
		);

		auto* volumeLight = Allocator::New<Light>();
		volumeLight->Init(
			glm::vec4(10, 10, -10, 1.0),
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
		volumeSceneObj->addComponent(volumeObject);
		scene->AddSceneObject(volumeSceneObj, "volumeObj", "VolumeLayer");

		auto* control = InitObjectObserverControl(scene, volumeObject);
		InitVoxelCaption(scene, dataSetName.c_str());
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
		}

		{
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
		}

		{
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
		}
			

		return scene;
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
	
}
