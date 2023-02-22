#include "LightManager.h"
#include "GlobalInclude.h"
#include "MemoryManager.h"
#include "HograTime.h"
#include "DebugUtils.h"

namespace Hogra {
	LightManager::LightManager()
	{
		std::vector<int> subDataSizes;
		subDataSizes.push_back(sizeof(int));			// lightCount
		for (int i = 0; i < MAX_LIGHT_COUNT; i++) {
			subDataSizes.push_back(sizeof(glm::vec4));		// position
			subDataSizes.push_back(sizeof(glm::vec3));		// powerDensity
			subDataSizes.push_back(sizeof(int));		// shadowCasterIdx
		}
		ubo.Init(subDataSizes, LIGHTS_UBO_BINDING);
	}

	LightManager::~LightManager()
	{
	}

	void LightManager::ExportData(const std::vector<OmniDirectionalShadowCaster*>& omniDirShadowCasters)
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

	void LightManager::RenderDeferredLighting() {
		deferredLightingSystem.Draw(pointLights, dirLights);
	}

	void LightManager::InitDebug()
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

	void LightManager::DrawDebug()
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
	void LightManager::Clear()
	{
		pointLights.clear();
		dirLights.clear();

	}
	void LightManager::Update()
	{
		for (auto& light : pointLights) {
			light->LatePhysicsUpdate(0.0f);
		}
	}
	const Texture2D& LightManager::GetDepthTexture()
	{
		return deferredLightingSystem.GetDepthTexture();
	}
}
