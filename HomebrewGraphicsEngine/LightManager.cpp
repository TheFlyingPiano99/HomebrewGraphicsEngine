#include "LightManager.h"
#include "GlobalInclude.h"

namespace hograengine {
	LightManager::LightManager()
	{
		std::vector<int> subDataSizes;
		subDataSizes.push_back(sizeof(int));			// lightCount
		for (int i = 0; i < MAX_LIGHT_COUNT; i++) {
			subDataSizes.push_back(sizeof(glm::vec4));		// position
			subDataSizes.push_back(sizeof(glm::vec3));		// powerDensity
		}
		ubo = new UniformBufferObject(subDataSizes, LIGHTS_UBO_BINDING);
	}

	LightManager::~LightManager()
	{
		delete ubo;
		delete shaderProgram;
		delete debugMaterial;
		delete debugGeometry;
		delete debugLightVolumeMesh;
	}

	void LightManager::exportData()
	{
		int count = lights.size();
		ubo->Bind();
		unsigned int idx = 0;
		ubo->uploadSubData((void*)&count, idx++);
		for (auto* light : lights) {
			light->exportData(ubo, idx);
		}
		ubo->Unbind();
	}

	void LightManager::initDebug()
	{
		if (nullptr != debugLightVolumeMesh) {
			return;
		}
		shaderProgram = new ShaderProgram(
			AssetFolderPathManager::getInstance()->getShaderFolderPath().append("debug.vert"),
			"",
			AssetFolderPathManager::getInstance()->getShaderFolderPath().append("debug.frag"));
		debugMaterial = new Material(shaderProgram);
		debugGeometry = GeometryFactory::getInstance()->getWireFrameSphere();
		debugLightVolumeMesh = new Mesh(debugMaterial, debugGeometry);
		debugLightVolumeMesh->setDepthTest(false);
	}

	void LightManager::drawDebug()
	{
		if (nullptr == debugLightVolumeMesh) {
			return;
		}
		debugLightVolumeMesh->Bind();
		std::vector<Geometry::InstanceData> data;
		for (int i = 1; i < lights.size(); i++) {
			data.push_back({ lights[i]->getVolumeModelMatrix(), glm::mat4(1.0f)});
		}
		debugLightVolumeMesh->DrawInstanced(data);

	}
}
