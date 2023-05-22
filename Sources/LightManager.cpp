#include "LightManager.h"
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
			subDataSizes.push_back(sizeof(int));		// shadowCasterIdx
		}
		ubo.Init(subDataSizes, LIGHTS_UBO_BINDING);

		brdfLUT.Init(GL_RG16F, glm::ivec2(512, 512), BRDF_LUT_UNIT, GL_RG, GL_FLOAT);

		FBO captureFBO;
		captureFBO.Init();
		captureFBO.LinkTexture(GL_COLOR_ATTACHMENT0, brdfLUT);
		captureFBO.Bind();
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		auto shader = ShaderProgram();
		shader.Init(
			AssetFolderPathManager::getInstance()->getShaderFolderPath().append("fullScreenQuad.vert"),
			"",
			AssetFolderPathManager::getInstance()->getShaderFolderPath().append("precalculateIndirectBRDF.frag")
		);
		shader.Activate();

		auto quad = GeometryFactory::GetInstance()->GetSimpleQuad();
		quad->Draw();
		captureFBO.saveToPPM(
			AssetFolderPathManager::getInstance()->getSavesFolderPath().append("brdfLUT.ppm")
		);
		captureFBO.Unbind();
	}

	Renderer::~Renderer()
	{
	}

	void Renderer::ExportData(const std::vector<OmniDirectionalShadowCaster*>& omniDirShadowCasters)
	{
		deferredLightingSystem.ExportShadowMaps(omniDirShadowCasters);

		int count = pointLights.size();
		ubo.Bind();
		unsigned int idx = 0;
		ubo.UploadSubData((void*)&count, idx++);
		for (auto* light : pointLights) {
			light->ExportData(ubo, idx);
		}
		ubo.Unbind();
	}

	void Renderer::SetSkybox(TextureCube* envMap) {
		if (this->environmentMap != envMap && nullptr != envMap) { // Generate irradiance and prefilter map
			this->environmentMap = envMap;
			// Irradiance map:
			auto irradianceConvolutionShader = ShaderProgram();
			irradianceConvolutionShader.Init(
				AssetFolderPathManager::getInstance()->getShaderFolderPath().append("environmentToIrradiance.vert"),
				"",
				AssetFolderPathManager::getInstance()->getShaderFolderPath().append("environmentToIrradiance.frag")
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
				AssetFolderPathManager::getInstance()->getShaderFolderPath().append("environmentToIrradiance.vert"),
				"",
				AssetFolderPathManager::getInstance()->getShaderFolderPath().append("environmentPrefilter.frag")
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
			AssetFolderPathManager::getInstance()->getShaderFolderPath().append("debug.vert"),
			"",
			AssetFolderPathManager::getInstance()->getShaderFolderPath().append("debug.frag"));
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
	const Texture2D& Renderer::GetDepthTexture()
	{
		return deferredLightingSystem.GetDepthTexture();
	}
}
