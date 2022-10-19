#include "RenderLayer.h"

namespace Hogra {
	
	void RenderLayer::Render(FBO& outFbo, const Texture2D& depthTexture, const Camera& camera) {
		FBO* fbo = nullptr;
		if (postProcessStages.empty()) {
			fbo = &outFbo;
		}
		else {
			fbo = &postProcessStages.front()->GetFBO();
		}
		fbo->Bind();
		fbo->LinkTexture(GL_DEPTH_ATTACHMENT, depthTexture, 0);
		switch (renderMode)
		{
		case Hogra::RenderLayer::RenderMode::forwardRenderMode:
			for (auto& object : objects) {
				object->Draw(*fbo, depthTexture, camera);
			}
			break;
		case Hogra::RenderLayer::RenderMode::forwardInstancedRenderMode:
			for (auto& group : groups) {
				group->Draw();
			}
			break;
		case Hogra::RenderLayer::RenderMode::deferredRenderMode:
			for (auto& object : objects) {
				object->Draw(*fbo, depthTexture, camera);
			}

			if (postProcessStages.empty()) {
				outFbo.Bind();
			}
			else {
				postProcessStages.front()->GetFBO().Bind();
			}

			// TODO Draw

			break;
		case Hogra::RenderLayer::RenderMode::deferredInstancedRenderMode:
			deferredFBO.Bind();
			deferredFBO.LinkTexture(GL_DEPTH_ATTACHMENT, depthTexture, 0);
			for (auto& group : groups) {
				group->Draw();
			}

			if (postProcessStages.empty()) {
				outFbo.Bind();
			}
			else {
				postProcessStages.front()->GetFBO().Bind();
			}

			// TODO Draw

			break;
		default:
			break;
		}

		// Post process effects of the layer:
		for (int i = 0; i < postProcessStages.size(); i++) {
			postProcessStages[i]->Draw((i < postProcessStages.size() - 1) ? postProcessStages[i + 1]->GetFBO() : outFbo, depthTexture);
		}
	}

}
