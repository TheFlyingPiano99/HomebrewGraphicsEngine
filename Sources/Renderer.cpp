#include "Renderer.h"
#include "GlobalInclude.h"
#include "MemoryManager.h"
#include "HograTime.h"
#include "DebugUtils.h"

namespace Hogra {
	Renderer::Renderer()
	{
		std::vector<int> subDataSizes;
		subDataSizes.push_back(sizeof(int));			// lightCount
		for (int i = 0; i < MAX_LIGHT_COUNT; i++) {
			subDataSizes.push_back(sizeof(glm::vec4));		// position
			subDataSizes.push_back(sizeof(glm::vec3));		// powerDensity
			subDataSizes.push_back(sizeof(glm::vec4));		// shadowCasterIdx [-1 if no shadow caster]
		}
		lightsUBO.Init(subDataSizes, LIGHTS_UBO_BINDING);

		brdfLUT.Init(glm::ivec2(512, 512), BRDF_LUT_UNIT, GL_RG16F, GL_RG, GL_FLOAT);

		FBO captureFBO;
		captureFBO.Init();
		captureFBO.LinkTexture(GL_COLOR_ATTACHMENT0, brdfLUT);
		captureFBO.Bind();
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		auto shader = ShaderProgram();
		shader.Init(
			AssetFolderPathManager::getInstance()->getShaderFolderPath().append("DefaultPipeline/fullScreenQuad.vert"),
			"",
			AssetFolderPathManager::getInstance()->getShaderFolderPath().append("Utility/EnvironmentMap/precalculateIndirectBRDF.frag")
		);
		shader.Activate();

		auto quad = GeometryFactory::GetInstance()->GetSimpleQuad();
		quad->Draw();
		captureFBO.Unbind();

		finalImageFBO.Init();
		finalImage.Init({ GlobalVariables::windowWidth, GlobalVariables::windowHeight }, 0, GL_RGB8, GL_RGB, GL_UNSIGNED_BYTE, false);
		finalImageFBO.LinkTexture(GL_COLOR_ATTACHMENT0, finalImage, 0);
		finalImageFBO.LinkTexture(GL_DEPTH_ATTACHMENT, deferredLightingSystem.GetDepthTexture(), 0);
		finalImageFBO.Unbind();
	}

	Renderer::~Renderer()
	{
	}

	void Renderer::ExportData(const std::vector<OmniDirectionalShadowCaster*>& omniDirShadowCasters)
	{
		deferredLightingSystem.ExportShadowMaps(omniDirShadowCasters);

		int count = pointLights.size();
		count += dirLights.size();
		lightsUBO.Bind();
		unsigned int idx = 0;
		lightsUBO.UploadSubData((void*)&count, idx++);
		for (auto* light : pointLights) {
			light->ExportData(lightsUBO, idx);
		}
		for (auto* light : dirLights) {
			light->ExportData(lightsUBO, idx);
		}
		lightsUBO.Unbind();
	}

	void Renderer::SetSkybox(TextureCube* envMap) {
		if (this->environmentMap != envMap && nullptr != envMap) { // Generate irradiance and prefilter map
			this->environmentMap = envMap;
			// Irradiance map:
			auto irradianceConvolutionShader = ShaderProgram();
			irradianceConvolutionShader.Init(
				AssetFolderPathManager::getInstance()->getShaderFolderPath().append("Utility/EnvironmentMap/environmentToIrradiance.vert"),
				"",
				AssetFolderPathManager::getInstance()->getShaderFolderPath().append("Utility/EnvironmentMap/environmentToIrradiance.frag")
			);
			DebugUtils::PrintMsg("Renderer", "Generating irradiance map.");
			irradianceMap = Allocator::New<TextureCube>();
			irradianceMap->InitFromCubeMap(
				*environmentMap,
				irradianceConvolutionShader,
					32,
				IRRADIANCE_MAP_UNIT,
				GL_RGB, 
				GL_FLOAT
			);
			DebugUtils::PrintMsg("Renderer", "Irradiance map complete.");

			// Prefilter map:
			auto prefilterConvolutionShader = ShaderProgram();
			prefilterConvolutionShader.Init(
				AssetFolderPathManager::getInstance()->getShaderFolderPath().append("Utility/EnvironmentMap/environmentToIrradiance.vert"),
				"",
				AssetFolderPathManager::getInstance()->getShaderFolderPath().append("Utility/EnvironmentMap/environmentPrefilter.frag")
			);
			DebugUtils::PrintMsg("Renderer", "Generating prefilter map.");

			prefilterMap = Allocator::New<TextureCube>();
			prefilterMap->InitFromCubeMap(
				*environmentMap,
				prefilterConvolutionShader,
				512,
				PREFILTER_MAP_UNIT,
				GL_RGB,
				GL_FLOAT,
				5
			);
			DebugUtils::PrintMsg("Renderer", "Prefilter map complete.");
		}
		else {
			this->environmentMap = envMap;
		}
	}

	void Renderer::OnContextResize(unsigned int contextWidth, unsigned int contextHeight) {
		deferredLightingSystem.OnContextResize(contextWidth, contextHeight);

		finalImage.Delete();
		finalImageFBO.Delete();
		finalImageFBO.Init();
		finalImage.Init({ GlobalVariables::windowWidth, GlobalVariables::windowHeight }, 0, GL_RGB8, GL_RGB, GL_UNSIGNED_BYTE, false);
		finalImageFBO.LinkTexture(GL_COLOR_ATTACHMENT0, finalImage, 0);
		finalImageFBO.LinkTexture(GL_DEPTH_ATTACHMENT, deferredLightingSystem.GetDepthTexture(), 0);
		finalImageFBO.Unbind();
	}

	void Renderer::RenderDeferredLighting() {
		deferredLightingSystem.Draw(
			pointLights,
			dirLights,
			environmentMap,
			irradianceMap,
			prefilterMap,
			&brdfLUT
		);
	}

	void Renderer::InitDebug()
	{
		if (nullptr != debugLightVolumeMesh) {
			return;
		}
		glyphProgram.Init(
			AssetFolderPathManager::getInstance()->getShaderFolderPath().append("Debug/debug.vert"),
			"",
			AssetFolderPathManager::getInstance()->getShaderFolderPath().append("Debug/debug.frag"));
		debugMaterial.Init(&glyphProgram);
		debugGeometry = GeometryFactory::GetInstance()->GetWireFrameSphere();
		debugLightVolumeMesh = Allocator::New<Mesh>();
		debugLightVolumeMesh->Init(&debugMaterial, debugGeometry);
		debugLightVolumeMesh->SetDepthTest(false);
	}

	void Renderer::DrawDebug()
	{
		if (nullptr == debugLightVolumeMesh) {
			return;
		}
		debugLightVolumeMesh->Bind();
		std::vector<Geometry::InstanceData> data;
		for (int i = 1; i < pointLights.size(); i++) {
			if (pointLights[i]->IsActive()) {
				data.push_back({ pointLights[i]->GetVolumeModelMatrix(), glm::mat4(1.0f) });
			}
		}
		debugLightVolumeMesh->DrawInstanced(data, data.size());

	}
	void Renderer::Clear()
	{
		pointLights.clear();
		dirLights.clear();

	}
	void Renderer::Update()
	{
		for (auto& light : pointLights) {
			light->LatePhysicsUpdate(0.0f);
		}
	}

	void Renderer::PrepareFirstFBOForRendering(FBO* fbo, const glm::vec4 &backgroundColor)
	{
		if (fbo != &finalImageFBO) {
			finalImageFBO.Bind();
			glClearColor(backgroundColor.x, backgroundColor.y, backgroundColor.z, backgroundColor.w);
			glEnable(GL_DEPTH_TEST);
			glDepthMask(GL_TRUE);
			glDepthFunc(GL_LESS);
			glClearDepth(1);
			glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
			glStencilMask(GL_FALSE);
		}

		fbo->Bind();
		if (0 != fbo->glID) {
			fbo->LinkTexture(GL_DEPTH_ATTACHMENT, deferredLightingSystem.GetDepthTexture(), 0);
		}
		glClearColor(backgroundColor.x, backgroundColor.y, backgroundColor.z, backgroundColor.w);
		glEnable(GL_DEPTH_TEST);
		glDepthMask(GL_TRUE);
		glDepthFunc(GL_LESS);
		glClearDepth(1);
		glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
		glStencilMask(GL_FALSE);
	}

	const Texture2D& Renderer::GetDepthTexture()
	{
		return deferredLightingSystem.GetDepthTexture();
	}

	void Renderer::BlitOnScreen()
	{
		glBindFramebuffer(GL_READ_FRAMEBUFFER, finalImage.glID);
		glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
		glBlitFramebuffer(0, 0, GlobalVariables::windowWidth, GlobalVariables::windowHeight, 0, 0, GlobalVariables::windowWidth, GlobalVariables::windowHeight, GL_COLOR_BUFFER_BIT, GL_NEAREST);

	}
}
