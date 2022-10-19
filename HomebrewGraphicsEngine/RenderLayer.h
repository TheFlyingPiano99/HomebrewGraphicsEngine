#pragma once
#include <vector>
#include "InstanceGroup.h"
#include "PostProcessStage.h"
#include "DeferredLightingSystem.h"

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

		void Render(FBO& outFbo, const Texture2D& depthTexture, const Camera& camera);

		FBO& GetInFBO() {
			if (renderMode == RenderMode::deferredRenderMode) {
				return deferredFBO;
			}
			else if (renderMode == RenderMode::forwardRenderMode) {
				return (postProcessStages.empty()) ? defaultFBO : postProcessStages.front()->GetFBO();
			}
		}

		void AddObject(SceneObject* obj) {
			objects.push_back(obj);
		}

		void AddInstanceGroup(InstanceGroup* group) {
			groups.push_back(group);
		}

		void AddPostProcessStage(PostProcessStage* stage) {
			postProcessStages.push_back(stage);
		}

		void SetRenderMode(RenderMode mode) {
			renderMode = mode;
		}
		
	private:
		RenderMode renderMode;
		std::vector<SceneObject*> objects;
		std::vector<InstanceGroup*> groups;
		std::vector<PostProcessStage*> postProcessStages;
		FBO deferredFBO;	// Not always used!
		FBO defaultFBO;
	};
}
