#pragma once
#include "glm/glm.hpp"
#include "glm/gtx/quaternion.hpp"
#include "glm/gtc/quaternion.hpp"
#include "Texture2D.h"
#include "Light.h"
#include "Texture3D.h"
#include "FBO.h"
#include "Camera.h"
#include <algorithm>
#include "VAO.h"
#include "EBO.h"
#include "Geometry.h"
#include <iostream>

#define VOLUME_TEXTURE_WIDTH 1024
#define VOLUME_TEXTURE_HEIGHT 1024

namespace Hogra {
	class VolumeObject
	{
	public:
		VolumeObject() = default;

		void Init(Texture3D* voxels, const glm::vec3& _pos, const glm::vec3& _scale, const glm::quat& _orientation, Light* _light, const glm::ivec2& contextSize);

		void Draw(const FBO& outputFBO, const Camera& camera, const Texture2D& depthTexture);

		const glm::vec3& GetPosition() const;

		void SetPosition(const glm::vec3& pos);

		const glm::vec3 GetScale() const;

		void SetScale(const glm::vec3& _scale);

		const glm::quat& GetOrientation() const;

		void SetOrientation(const glm::quat& _orientation);

		Light* GetLight() const;

		void SetLight(Light* _light);

		Texture3D* GetTexture() const {
			return voxels;
		}

		void Update();

		void SetTexture(Texture3D* texture) {
			voxels = texture;
		}



	private:

		struct BoxEdge {
			glm::vec3 position;
			glm::vec3 direction;
			float length;
		};

		struct BoundingBox {
			BoxEdge edges[12];
		};
		BoundingBox boundingBox;

		bool intersectPlane(const BoxEdge& edge, const glm::vec3& planePos, const glm::vec3& planeNormal, glm::vec3& intersection) {
			float t = dot(planePos - edge.position, planeNormal) / dot(edge.direction, planeNormal);
			if (t >= 0 && t < edge.length) {
				intersection = edge.position + edge.direction * t;
				return true;
			}
			return false;
		}

		void DrawProxyGeometry(
			const Camera& camera,
			const Texture2D& depthTexture,
			bool isBackToFront,
			int in,
			int out,
			const glm::vec3& modelSlicePosition,
			const glm::vec3& modelSliceNormal
		);

		void InitBoundingBox(const Dimensions& dimensions, BoundingBox& boundingBox);

		void ExportData(const ShaderProgram& program);

		glm::vec3 w_position;
		glm::vec3 scale;
		glm::quat orientation;
		glm::mat4 modelMatrix;
		glm::mat4 invModelMatrix;
		int sliceCount = 0;
		float w_diameter = 0;

		Light* light;
		Texture3D* voxels = nullptr;
		Texture2D colorTextures[2];
		Texture2D attenuationTextures[2];
		FBO pingpongFBO;
		ShaderProgram colorProgram;
		ShaderProgram attenuationProgram;
		ShaderProgram combineProgram;
		Geometry* fullScreenQuad = nullptr;
		glm::vec3 resolution = glm::vec3(1.0f);
	};
}

