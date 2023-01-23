#pragma once
#include <vector>
#include <string>
#include "InstanceGroup.h"
#include "PostProcessStage.h"
#include "DeferredLightingSystem.h"
#include "LightManager.h"

namespace Hogra {
	
	class RenderLayer {
	public:
		RenderLayer() {
			defaultFBO = FBO::GetDefault();
		}

		enum class RenderMode {
			forwardRenderMode = 0,
			forwardInstancedRenderMode = 1,
			deferredRenderMode = 2,
			deferredInstancedRenderMode = 3
		};

		void Render(FBO& outFbo, const Texture2D& depthTexture, const Camera& camera, LightManager& lightManager);

		FBO* GetInFBO() {
			return (postProcessStages.empty()) ? nullptr : &postProcessStages.front()->GetFBO();
		}

		void AddObject(SceneObject* obj) {
			objects.push_back(obj);
		}

		void AddInstanceGroup(InstanceGroup* group) {
			instanceGroups.push_back(group);
		}

		void AddPostProcessStage(PostProcessStage* stage) {
			postProcessStages.push_back(stage);
		}

		void SetRenderMode(RenderMode mode) {
			renderMode = mode;
			switch (renderMode)
			{
			case Hogra::RenderLayer::RenderMode::forwardRenderMode:
				break;
			case Hogra::RenderLayer::RenderMode::forwardInstancedRenderMode:
				break;
			case Hogra::RenderLayer::RenderMode::deferredRenderMode:
				break;
			case Hogra::RenderLayer::RenderMode::deferredInstancedRenderMode:
				break;
			default:
				break;
			}
		}
		
		const std::string& GetName() const {
			return name;
		}

		void SetName(const std::string& _name) {
			name = _name;
		}

		bool IsInstanced() {
			return renderMode == RenderMode::forwardInstancedRenderMode 
				|| renderMode == RenderMode::deferredInstancedRenderMode;
		}

	private:
		std::string name;
		RenderMode renderMode;
		std::vector<SceneObject*> objects;
		std::vector<InstanceGroup*> instanceGroups;
		std::vector<PostProcessStage*> postProcessStages;
		FBO deferredFBO;	// Not always used!
		FBO defaultFBO;
	};
}
