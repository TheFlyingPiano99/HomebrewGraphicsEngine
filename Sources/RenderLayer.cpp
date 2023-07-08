#include "RenderLayer.h"
#include "DebugUtils.h"

namespace Hogra {
	
	void RenderLayer::Render(FBO& outFbo, const Camera& camera, Renderer& renderer) {
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
		{
			if (0 != fbo->glID) {
				fbo->LinkTexture(GL_DEPTH_ATTACHMENT, renderer.GetDepthTexture(), 0);
			}
			fbo->Bind();
			for (auto& object : objects) {
				object->Draw(*fbo, renderer.GetDepthTexture(), camera);
			}
			break;

		}
		case Hogra::RenderLayer::RenderMode::forwardInstancedRenderMode:
		{
			for (auto& group : instanceGroups) {
				group->GatherInstanceData();
			}
			if (0 != fbo->glID) {
				fbo->LinkTexture(GL_DEPTH_ATTACHMENT, renderer.GetDepthTexture(), 0);
			}
			fbo->Bind();
			for (auto& group : instanceGroups) {
				group->Draw();
			}
			break;
		}
		case Hogra::RenderLayer::RenderMode::deferredRenderMode:
		{
			// Geometry pass:
			renderer.BindGBuffer();
			for (auto& object : objects) {
				object->Draw(*fbo, renderer.GetDepthTexture(), camera);
			}

			// Lighting pass:
			fbo->Bind();
			renderer.RenderDeferredLighting();

			break;
		}
		case Hogra::RenderLayer::RenderMode::deferredInstancedRenderMode:
		{
			// Geometry pass:
			renderer.BindGBuffer();
			for (auto& group : instanceGroups) {
				group->GatherInstanceData();
				group->Draw();
			}

			// Lighting pass:
			fbo->Bind();
			renderer.RenderDeferredLighting();
			break;
		}
		default:
			break;
		}

		// Post process effects of the layer:
		for (int i = 0; i < postProcessStages.size(); i++) {
			fbo = (i < postProcessStages.size() - 1) ? &postProcessStages[i + 1]->GetFBO() : &outFbo;
			postProcessStages[i]->Draw(*fbo, renderer.GetDepthTexture(), camera);
		}
	}

	void RenderLayer::OnContextResize(unsigned int w, unsigned int h)
	{
		for (auto pps : postProcessStages) {
			pps->OnContextResize(w, h);
		}
	}
}
