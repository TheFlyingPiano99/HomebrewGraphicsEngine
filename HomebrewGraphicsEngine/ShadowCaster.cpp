#include "ShadowCaster.h"
namespace Hogra {
	ShadowCaster::ShadowCaster() {
	}
	
	void ShadowCaster::Bind() {
		fbo.Bind();
		glClearDepth(1);
		glClear(GL_DEPTH_BUFFER_BIT);
		glEnable(GL_DEPTH_TEST);
		glDepthFunc(GL_LESS);
		program.Activate();
		ExportData();
	}
	
	void ShadowCaster::ExportData() {
		ubo.Bind();
		ubo.UploadSubData((void*)glm::value_ptr(lightSpaceMatrix[0]), 0);
		ubo.UploadSubData((void*)glm::value_ptr(lightSpaceMatrix[1]), 1);
		ubo.UploadSubData((void*)glm::value_ptr(lightSpaceMatrix[2]), 2);
		ubo.UploadSubData((void*)glm::value_ptr(lightSpaceMatrix[3]), 3);
		ubo.Unbind();
		shadowMap.Bind();
	}
}
