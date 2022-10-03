#include "VolumeObject.h"
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/rotate_vector.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "AssetFolderPathManager.h"
#include "GeometryFactory.h"
#include <iostream>

#define VOXEL_ATTENUATION_TEXTURE_WIDTH 2000
#define VOXEL_ATTENUATION_TEXTURE_HEIGHT 2000

namespace Hogra {

	VolumeObject::VolumeObject() 
		: STFradius(0.25f),
		STFEmission(1.0f),
		STFOpacity(1.0f),
		transferFloodFillTreshold(4.0f)
	{

	}


	void VolumeObject::Init(Texture3D* _voxels, const glm::vec3& _pos, const glm::vec3& _scale, const glm::quat& _orientation, Light* _light, const glm::ivec2& contextSize)
	{
		voxels = _voxels;
		w_position = _pos;
		scale = _scale;
		orientation = _orientation;
		light = _light;
		InitBoundingBox(voxels->GetDimensions(), boundingBox);
		resolution = glm::vec3(voxels->GetDimensions().width, voxels->GetDimensions().height, voxels->GetDimensions().depth);
		scale *= glm::vec3(voxels->GetDimensions().widthScale, voxels->GetDimensions().heightScale, voxels->GetDimensions().depthScale);
		w_diameter = glm::length(resolution * scale);

		pingpongFBO.Init();
		colorProgram.Init(
			AssetFolderPathManager::getInstance()->getShaderFolderPath().append("proxyGeometryViewCamera.vert"), 
			"",
			AssetFolderPathManager::getInstance()->getShaderFolderPath().append("voxelColor.frag")
		);
		attenuationProgram.Init(
			AssetFolderPathManager::getInstance()->getShaderFolderPath().append("proxyGeometryLightCamera.vert"),
			"",
			AssetFolderPathManager::getInstance()->getShaderFolderPath().append("voxelAttenuation.frag")
		);

		colorTextures[0].Init(GL_RGBA16F, contextSize, 0, GL_RGBA, GL_FLOAT);
		colorTextures[1].Init(GL_RGBA16F, contextSize, 0, GL_RGBA, GL_FLOAT);
		// Unit 1 is reserved for depth texture!
		attenuationTextures[0].Init(GL_RGBA16F, glm::ivec2(VOXEL_ATTENUATION_TEXTURE_WIDTH, VOXEL_ATTENUATION_TEXTURE_HEIGHT), 2, GL_RGBA, GL_FLOAT);
		attenuationTextures[1].Init(GL_RGBA16F, glm::ivec2(VOXEL_ATTENUATION_TEXTURE_WIDTH, VOXEL_ATTENUATION_TEXTURE_HEIGHT), 2, GL_RGBA, GL_FLOAT);

		prevCompleteImage.Init(GL_RGBA16F, contextSize, 0, GL_RGBA, GL_FLOAT);
		prevCompleteImageFBO.Init();
		prevCompleteImageFBO.LinkTexture(GL_COLOR_ATTACHMENT0, prevCompleteImage);

		// Full screen quad mesh for combine scene with volume:
		combineProgram.Init(
			AssetFolderPathManager::getInstance()->getShaderFolderPath().append("fullscreenQuad.vert"),
			"",
			AssetFolderPathManager::getInstance()->getShaderFolderPath().append("bypass.frag")
		);
		fullScreenQuad = GeometryFactory::GetInstance()->getFullScreenQuad();
		
		transferFunction.Init();
		LoadFeatures();
	}

	void VolumeObject::Draw(const FBO& outputFBO, Camera& camera, const Texture2D& depthTexture)
	{
		bool isCameraMoved = camera.PopIsMoved();
		bool isFinishedVolume = false;
		if (isCameraMoved || isChanged || levelOfDetail < 0.999999f || firstSlice > 0) {
			isFinishedVolume = true;
			if (isCameraMoved || isChanged) {
				levelOfDetail = 0.01f;
				firstSlice = 0;
				isChanged = false;
			}
			else if (0 == firstSlice) {
				levelOfDetail *= (1.5f + levelOfDetail);
				if (1.0f < levelOfDetail) {	// max
					levelOfDetail = 1.0f;
				}
			}
			sliceCount = (int)(glm::length(resolution) * levelOfDetail * 4.0f);

			// Calculate directions and transformations:
			auto lightDir = glm::normalize(glm::vec3(light->GetPosition()) - this->w_position);
			auto viewDir = glm::normalize(glm::vec3(camera.GetPosition()) - this->w_position);
			bool isBackToFront = false;
			if (glm::dot(lightDir, viewDir) < 0.0f) {	// Negate viewDir if the camera is on the opposite side of the volume as the light source.
				viewDir *= -1.0f;
				isBackToFront = true;
			}
			auto w_halfway = (lightDir + viewDir) * 0.5f;
			glm::vec3 w_sliceDelta = 1.0f / (float)sliceCount * w_diameter * w_halfway;

			// Clear textures: (working)
			pingpongFBO.Bind();
			glEnable(GL_BLEND);
			glDisable(GL_DEPTH_TEST);
			glDisable(GL_CULL_FACE);
			if (0 == firstSlice) {
				glClearColor(0.0, 0.0, 0.0, 0.0);
				pingpongFBO.LinkTexture(GL_COLOR_ATTACHMENT0, colorTextures[0], 0);
				glClear(GL_COLOR_BUFFER_BIT);
				pingpongFBO.LinkTexture(GL_COLOR_ATTACHMENT0, colorTextures[1], 0);
				glClear(GL_COLOR_BUFFER_BIT);
				pingpongFBO.LinkTexture(GL_COLOR_ATTACHMENT0, attenuationTextures[0], 0);
				glClear(GL_COLOR_BUFFER_BIT);
				pingpongFBO.LinkTexture(GL_COLOR_ATTACHMENT0, attenuationTextures[1], 0);
				glClear(GL_COLOR_BUFFER_BIT);

				// Export matrices:
				glm::mat4 view = glm::lookAt(
					glm::vec3(light->GetPosition().x, light->GetPosition().y, light->GetPosition().z),
					this->w_position,
					camera.getPreferedUp()
				);
				glm::mat4 projection = glm::perspective(
					glm::radians(45.0f),
					(float)VOXEL_ATTENUATION_TEXTURE_WIDTH / (float)VOXEL_ATTENUATION_TEXTURE_HEIGHT,
					0.1f,
					1000.0f
				);

				light->SetPowerDensity(glm::vec3(lightPower));

				glm::mat4 lightViewProjMatrix = projection * view;
				ExportData(colorProgram, lightViewProjMatrix, isBackToFront, camera, w_sliceDelta);
				ExportData(attenuationProgram, lightViewProjMatrix, isBackToFront, camera, w_sliceDelta);
			}


			// Half-angle slicing:
			int in = 0;
			int slicePerCurrentFrame = 0;
			int maxSlicePerFrame = 50;
			for (int slice = firstSlice; slice < sliceCount; slice++) {
				in = slice % 2;
				out = (slice + 1) % 2;
				auto w_slicePos = w_position + w_halfway * (1.0f - (float)slice / (float)sliceCount * 2.0f) * w_diameter * 0.5f;
				auto m_slicePos = invModelMatrix * glm::vec4(w_slicePos, 1.0f);
				auto m_sliceNorm = glm::normalize(invModelMatrix * glm::vec4(w_halfway, 0.0f));
				DrawProxyGeometry(camera,
					depthTexture,
					isBackToFront,
					in,
					out,
					glm::vec3(m_slicePos) / m_slicePos.w,
					m_sliceNorm
				);
				slicePerCurrentFrame++;
				if (slicePerCurrentFrame >= maxSlicePerFrame && sliceCount > maxSlicePerFrame) {
					firstSlice = slice + 1;
					isFinishedVolume = false;
					break;
				}
			}
			pingpongFBO.Unbind();
			if (isFinishedVolume) {
				firstSlice = 0;
			}
		}

		// Combine volume with the earlier rendered scened
		combineProgram.Activate();
		fullScreenQuad->BindVAO();
		if (isFinishedVolume) {
			prevCompleteImageFBO.Bind();
			glDisable(GL_DEPTH_TEST);
			glEnable(GL_BLEND);
			glBlendFunc(GL_ONE, GL_ZERO);
			colorTextures[out].Bind();
			fullScreenQuad->Draw();
		}
		outputFBO.Bind();
		glDisable(GL_DEPTH_TEST);
		glEnable(GL_BLEND);
		glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
		prevCompleteImage.Bind();
		fullScreenQuad->Draw();

		transferFunction.Draw(outputFBO);
		outputFBO.Unbind();
	}

	const glm::vec3& VolumeObject::GetPosition() const {
		return w_position;
	}

	void VolumeObject::SetPosition(const glm::vec3& pos) {
		w_position = pos;
		isChanged = true;
	}

	const glm::vec3 VolumeObject::GetScale() const {
		return scale;
	}

	void VolumeObject::SetScale(const glm::vec3& _scale) {
		scale = _scale;
		isChanged = true;
	}

	const glm::quat& VolumeObject::GetOrientation() const {
		return orientation;
	}

	void VolumeObject::SetOrientation(const glm::quat& _orientation) {
		orientation = _orientation;
		isChanged = true;
	}

	Light* VolumeObject::GetLight() const
	{
		return light;
	}

	void VolumeObject::SetLight(Light* _light)
	{
		light = _light;
		isChanged = true;
	}

	void VolumeObject::Update()
	{
		modelMatrix = glm::translate(w_position) * glm::toMat4(orientation) * glm::scale(scale);
		invModelMatrix = glm::inverse(modelMatrix);
	}

	void VolumeObject::LatePhysicsUpdate(float dt) {
		transferFunction.Animate(dt);
	}


	void VolumeObject::DrawProxyGeometry(
		const Camera& camera, 
		const Texture2D& depthTexture, 
		bool isBackToFront, 
		int in, 
		int out, 
		const glm::vec3& modelSlicePosition, 
		const glm::vec3& m_sliceNormal
	) {
		glm::vec3 m_intersectionPoint;
		std::vector<glm::vec3> vertices;
		for (int i = 0; i < 12; i++) {
			if (IntersectPlane(boundingBox.edges[i], modelSlicePosition, m_sliceNormal, m_intersectionPoint)) {
				vertices.push_back(m_intersectionPoint);
			}
		}
		if (vertices.size() >= 3) {
			std::vector<GLint> indices;

			if (vertices.size() == 3) {	// Single triangle
				indices.push_back(0);
				indices.push_back(1);
				indices.push_back(2);
			}
			else {	// Triangle fan
				glm::vec4 ndc_center = camera.GetViewProjMatrix() * modelMatrix * glm::vec4(modelSlicePosition, 1.0f);
				ndc_center /= ndc_center.w;
				const glm::mat4& modelMatrix = this->modelMatrix;
				auto* cam_ptr = &camera;	// Capturing camera by value caused error: locked up the scene (Didn't understand...)
				auto compareAngleFunc = [cam_ptr, modelSlicePosition, modelMatrix, ndc_center](glm::vec3& vertex1, glm::vec3& vertex2)-> float {
					glm::vec4 ndc_vert = cam_ptr->GetViewProjMatrix() * modelMatrix * glm::vec4(vertex1.x, vertex1.y, vertex1.z, 1.0f);
					ndc_vert /= ndc_vert.w;
					glm::vec2 dir1 = glm::normalize(glm::vec2(ndc_vert.x, ndc_vert.y) - glm::vec2(ndc_center.x, ndc_center.y));

					ndc_vert = cam_ptr->GetViewProjMatrix() * modelMatrix * glm::vec4(vertex2.x, vertex2.y, vertex2.z, 1.0f);
					ndc_vert /= ndc_vert.w;
					glm::vec2 dir2 = glm::normalize(glm::vec2(ndc_vert.x, ndc_vert.y) - glm::vec2(ndc_center.x, ndc_center.y));
					return atan2f(dir1.x, dir1.y) > atan2f(dir2.x, dir2.y);
				};

				std::sort(vertices.begin(), vertices.end(), compareAngleFunc);

				for (int i = 0; i < vertices.size() - 2; i++) {
					indices.push_back(0);
					indices.push_back(i + 1);
					indices.push_back(i + 2);
				}
			}
			VAO VAO;
			VAO.Init();
			VBO VBO;
			VBO.Init(vertices);
			EBO EBO;
			EBO.Init(indices);
			EBO.Bind();
			VAO.LinkAttrib(VBO, 0, 3, GL_FLOAT, sizeof(glm::vec3), (void*)0);

			// Color:
			colorProgram.Activate();
			transferFunction.Bind();
			voxels->Bind();
			colorTextures[in].Bind();
			attenuationTextures[in].Bind();
			pingpongFBO.LinkTexture(GL_COLOR_ATTACHMENT0, colorTextures[out], 0);
			if (isBackToFront) {
				glBlendFuncSeparate(GL_ONE, GL_ONE_MINUS_SRC_ALPHA, GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
				//glBlendFuncSeparate(GL_ONE, GL_ZERO, GL_ONE, GL_ZERO);
			}
			else {
				glBlendFuncSeparate(GL_ONE_MINUS_DST_ALPHA, GL_ONE, GL_ONE_MINUS_DST_ALPHA, GL_ONE);
				//glBlendFuncSeparate(GL_ONE, GL_ZERO, GL_ONE, GL_ZERO);
			}

			VAO.Bind();
			VBO.Bind();
			EBO.Bind();
			glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);

			// Attenuation:
			attenuationProgram.Activate();
			transferFunction.Bind();
			attenuationTextures[in].Bind();
			pingpongFBO.LinkTexture(GL_COLOR_ATTACHMENT0, attenuationTextures[out], 0);

			// For light always front-to-back
			glBlendFuncSeparate(GL_ONE_MINUS_DST_COLOR, GL_ONE, GL_ONE_MINUS_DST_ALPHA, GL_ONE);

			glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
			VBO.Delete();
			EBO.Delete();
			VAO.Delete();
			VAO.Unbind();
			VAO.Delete();
		}
	}

	void VolumeObject::InitBoundingBox(const Dimensions& dim, BoundingBox& box)
	{
		glm::vec3 resolution = glm::vec3(dim.width, dim.height, dim.depth);

		// Base quad:
		box.edges[0].position = glm::vec3(-0.5f, -0.5f, -0.5f) * resolution;
		box.edges[0].direction = glm::vec3(1, 0, 0);
		box.edges[0].length = resolution.x;

		box.edges[1].position = glm::vec3(0.5, -0.5, -0.5) * resolution;
		box.edges[1].direction = glm::vec3(0, 0, 1);
		box.edges[1].length = resolution.z;

		box.edges[2].position = glm::vec3(0.5, -0.5, 0.5) * resolution;
		box.edges[2].direction = glm::vec3(-1, 0, 0);
		box.edges[2].length = resolution.x;

		box.edges[3].position = glm::vec3(-0.5, -0.5, 0.5) * resolution;
		box.edges[3].direction = glm::vec3(0, 0, -1);
		box.edges[3].length = resolution.z;

		// Vertical edges:
		box.edges[4].position = glm::vec3(-0.5f, -0.5f, -0.5f) * resolution;
		box.edges[4].direction = glm::vec3(0, 1, 0);
		box.edges[4].length = resolution.y;

		box.edges[5].position = glm::vec3(0.5, -0.5f, -0.5f) * resolution;
		box.edges[5].direction = glm::vec3(0, 1, 0);
		box.edges[5].length = resolution.y;

		box.edges[6].position = glm::vec3(0.5f, -0.5f, 0.5f) * resolution;
		box.edges[6].direction = glm::vec3(0, 1, 0);
		box.edges[6].length = resolution.y;

		box.edges[7].position = glm::vec3(-0.5f, -0.5f, 0.5f) * resolution;
		box.edges[7].direction = glm::vec3(0, 1, 0);
		box.edges[7].length = resolution.y;

		// Top quad:
		box.edges[8].position = glm::vec3(-0.5f, 0.5f, -0.5f) * resolution;
		box.edges[8].direction = glm::vec3(1, 0, 0);
		box.edges[8].length = resolution.x;

		box.edges[9].position = glm::vec3(0.5f, 0.5f, -0.5f) * resolution;
		box.edges[9].direction = glm::vec3(0, 0, 1);
		box.edges[9].length = resolution.z;

		box.edges[10].position = glm::vec3(0.5f, 0.5f, 0.5f) * resolution;
		box.edges[10].direction = glm::vec3(-1, 0, 0);
		box.edges[10].length = resolution.x;

		box.edges[11].position = glm::vec3(-0.5f, 0.5f, 0.5f) * resolution;
		box.edges[11].direction = glm::vec3(0, 0, -1);
		box.edges[11].length = resolution.z;
	}

	void VolumeObject::ExportData(const ShaderProgram& program, const glm::mat4& lightViewProjMatrix, bool isBackToFront, const Camera&  camera, const glm::vec3& w_sliceDelta)
	{
		program.Activate();
		glUniformMatrix4fv(glGetUniformLocation(program.ID, "sceneObject.modelMatrix"), 1, GL_FALSE, glm::value_ptr(modelMatrix));
		glUniformMatrix4fv(glGetUniformLocation(program.ID, "sceneObject.invModelMatrix"), 1, GL_FALSE, glm::value_ptr(invModelMatrix));
		glUniformMatrix4fv(glGetUniformLocation(program.ID, "light.viewProjMatrix"), 1, GL_FALSE, glm::value_ptr(lightViewProjMatrix));
		glUniform3f(glGetUniformLocation(program.ID, "resolution"), resolution.x, resolution.y, resolution.z);
		glUniform3f(glGetUniformLocation(program.ID, "scale"), scale.x, scale.y, scale.z);
		glUniform3f(glGetUniformLocation(program.ID, "w_sliceDelta"), w_sliceDelta.x, w_sliceDelta.y, w_sliceDelta.z);
		glUniform3f(glGetUniformLocation(program.ID, "light.position"), light->GetPosition().x, light->GetPosition().y, light->GetPosition().z);
		glUniform3f(glGetUniformLocation(program.ID, "light.powerDensity"), light->getPowerDensity().x, light->getPowerDensity().y, light->getPowerDensity().z);
		glUniform1i(glGetUniformLocation(program.ID, "isBackToFront"), (isBackToFront)? 1 : 0);
		glUniform1f(glGetUniformLocation(program.ID, "opacityScale"), opacityScale);
	}
}



