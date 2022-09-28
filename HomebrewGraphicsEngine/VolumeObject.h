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
#include "TransferFunction.h"
#include "AssetFolderPathManager.h"
#include <iostream>
#include <fstream>

#define VOLUME_TEXTURE_WIDTH 1024
#define VOLUME_TEXTURE_HEIGHT 1024

namespace Hogra {
	class VolumeObject
	{
	public:

		VolumeObject();

		void Init(Texture3D* voxels, const glm::vec3& _pos, const glm::vec3& _scale, const glm::quat& _orientation, Light* _light, const glm::ivec2& contextSize);

		void Draw(const FBO& outputFBO, Camera& camera, const Texture2D& depthTexture);

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

		void LatePhysicsUpdate(float dt);

		void SetTexture(Texture3D* texture) {
			voxels = texture;
			isChanged = true;
		}

		void LoadFeatures() {
			std::ifstream stream(AssetFolderPathManager::getInstance()->getSavesFolderPath().append("/features.txt"));
			if (stream.is_open()) {
				featureGroups.clear();
				transferFunction.loadFeatures(stream, featureGroups);
				stream.close();
				FeatureGroup all;
				all.name = "All features";
				for (Feature& feature : transferFunction.GetFeatures()) {
					all.features.push_back(&feature);
				}
				all.serialize = false;
				featureGroups.push_back(all);
				selectedFeature = nullptr;
				selectedFeatureGroup = nullptr;
			}
		}

		void CycleSelectedFeature() {
			const auto* prevSelected = selectedFeature;
			selectedFeature = transferFunction.nextFeature(selectedFeature);
			if (prevSelected != selectedFeature) {
				std::cout << "Next: " << selectedFeature->name << std::endl;
				transferFunction.clear();
				transferFunction.setFeatureVisibility(*selectedFeature, true);
				transferFunction.blur(3);
			}
			isChanged = true;
		}

		void ShowAll() {
			transferFunction.showAll();
			isChanged = true;
		}

		FeatureGroup* GetSelectedFeatureGroup() {
			return selectedFeatureGroup;
		}

		std::vector<FeatureGroup>& GetFeatureGroups() {
			return featureGroups;
		}

		void ShowSelectedFeatureGroup()
		{
			if (nullptr != selectedFeatureGroup) {
				transferFunction.clear();
				for (Feature* feature : selectedFeatureGroup->features) {
					feature->visible = true;
				}
				transferFunction.showVisible();
				transferFunction.blur(3);
				isChanged = true;
			}
		}

		void SetSelectedFeatureGroup(const char* name) {
			for (FeatureGroup& group : featureGroups) {
				if (group.name.compare(name) == 0) {
					if (selectedFeatureGroup != &group) {
						selectedFeature = nullptr;
					}
					selectedFeatureGroup = &group;
					if (selectedFeatureGroup != &group) {
						selectedFeature = nullptr;
						ShowSelectedFeatureGroup();
					}
					break;
				}
			}
			isChanged = true;
		}

		Feature* GetSelectedFeature() {
			return selectedFeature;
		}

		void SetSelectedFeature(const char* name) {
			selectedFeature = transferFunction.findFeatureByName(name);
			transferFunction.clear();
			if (selectedFeature != nullptr) {
				if (transferFunction.setFeatureVisibility(*selectedFeature, true)) {
					transferFunction.blur(3);
					isChanged = true;
				};
			}

		}

		void ResetToDefault()
		{
			transferFunction.clear();
			transferFunction.defaultTransferFunction(glm::ivec2(256, 128));
			isChanged = true;
		}


		void CreateFeatureGroup()
		{
			static int nextGroupIdx = 1;
			FeatureGroup group;
			group.features.clear();
			group.name = std::string("Group").append(std::to_string(nextGroupIdx++));
			featureGroups.push_back(group);
		}

		void RemoveSelectedFeatureFromFeatureGroup()
		{
			if (nullptr != selectedFeature && nullptr != selectedFeatureGroup) {
				selectedFeatureGroup->features.erase(std::find(selectedFeatureGroup->features.begin(),
					selectedFeatureGroup->features.end(),
					selectedFeature));
				selectedFeature = nullptr;
			}
		}

		void RedrawSelected()
		{
			transferFunction.clear();
			transferFunction.setFeatureVisibility(*selectedFeature, true);
			transferFunction.blur(3);
			isChanged = true;
		}

		void RotateModelAroundX(float rad_angle) {
			orientation = glm::angleAxis(rad_angle, glm::vec3(1.0f,0.0f,0.0f)) * orientation;
		}

		void RotateModelAroundY(float rad_angle) {
			orientation = glm::angleAxis(rad_angle, glm::vec3(0.0f, 1.0f, 0.0f)) * orientation;
		}

		void RotateModelAroundZ(float rad_angle) {
			orientation = glm::angleAxis(rad_angle, glm::vec3(0.0f, 0.0f, 1.0f)) * orientation;
		}

		float& GetSTFradius() {
			return STFradius;
		}

		float& GetSTFOpacity() {
			return STFOpacity;
		}
		float& GetSTFEmission() {
			return STFEmission;
		}

		void ResetToSTF()
		{
			transferFunction.spatialTransferFunction(glm::ivec2(256, 128), *voxels, STFradius, STFOpacity, STFEmission);
			std::cout << "STF feature count: " << transferFunction.GetFeatures().size() << std::endl;
			isChanged = true;
		}

		std::vector<Feature>& GetFeatures() {
			return transferFunction.GetFeatures();
		}

		void AddFeatureToFeatureGroup(Feature* feature) {
			if (nullptr != feature && nullptr != selectedFeatureGroup) {
				if (std::ranges::find(selectedFeatureGroup->features.begin(), selectedFeatureGroup->features.end(), feature)
					== selectedFeatureGroup->features.end()) {
					selectedFeatureGroup->features.push_back(feature);
				}
			}
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

		bool IntersectPlane(const BoxEdge& edge, const glm::vec3& planePos, const glm::vec3& planeNormal, glm::vec3& intersection) {
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

		void ExportData(const ShaderProgram& program, const glm::mat4& lightViewProjMatrix, bool isBackToFront, const Camera& camera, const glm::vec3& w_sliceDelta);

		glm::vec3 w_position;
		glm::vec3 scale;
		glm::quat orientation;
		glm::mat4 modelMatrix;
		glm::mat4 invModelMatrix;
		int sliceCount = 0;
		float w_diameter = 0;

		float STFradius;
		float STFOpacity;
		float STFEmission;

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
		std::vector<FeatureGroup> featureGroups;
		TransferFunction transferFunction;
		Feature* selectedFeature = nullptr;
		FeatureGroup* selectedFeatureGroup = nullptr;
		bool isChanged = true;

	};
}

