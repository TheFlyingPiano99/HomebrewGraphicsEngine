#include "RenderLayer.h"

namespace Hogra {
	
	void RenderLayer::Render(FBO& outFbo, const Texture2D& depthTexture, const Camera& camera, LightManager& lightManager) {
		FBO* fbo = nullptr;
		if (postProcessStages.empty()) {
			fbo = &outFbo;
		}
		else {
			fbo = &postProcessStages.front()->GetFBO();
		}
		switch (renderMode)
		{
		case Hogra::RenderLayer::RenderMode::forwardRenderMode:
			fbo->LinkTexture(GL_DEPTH_ATTACHMENT, depthTexture, 0);
			fbo->Bind();
			for (auto& object : objects) {
				object->Draw(*fbo, depthTexture, camera);
			}
			break;
		case Hogra::RenderLayer::RenderMode::forwardInstancedRenderMode:
			for (auto& group : instanceGroups) {
				group->GatherInstanceData();
			}
			fbo->LinkTexture(GL_DEPTH_ATTACHMENT, depthTexture, 0);
			fbo->Bind();
			for (auto& group : instanceGroups) {
				group->Draw();
			}
			break;
		case Hogra::RenderLayer::RenderMode::deferredRenderMode:
		
			// Geometry pass:
			lightManager.BindGBuffer();
			for (auto& object : objects) {
				object->Draw(*fbo, depthTexture, camera);
			}

			// Lighting pass:
			fbo->Bind();
			lightManager.RenderDeferredLighting();
			
			break;
		case Hogra::RenderLayer::RenderMode::deferredInstancedRenderMode:
			for (auto& group : instanceGroups) {
				group->GatherInstanceData();
			}

			// Geometry pass:
			lightManager.BindGBuffer();
			for (auto& group : instanceGroups) {
				group->Draw();
			}

			// Lighting pass:
			fbo->Bind();
			lightManager.RenderDeferredLighting();
			break;
		default:
			break;
		}

		// Post process effects of the layer:
		for (int i = 0; i < postProcessStages.size(); i++) {
			postProcessStages[i]->Draw((i < postProcessStages.size() - 1) ? postProcessStages[i + 1]->GetFBO() : outFbo, depthTexture, camera);
		}
	}

}
