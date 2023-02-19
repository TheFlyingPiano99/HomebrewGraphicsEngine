#pragma once
#include "glm/glm.hpp"
#include "glm/gtx/quaternion.hpp"
#include "glm/gtc/quaternion.hpp"
#include "TransferFunction.h"
#include "BoundingGeometry.h"
#include "../Component.h"
#include "../Texture2D.h"
#include "../PointLight.h"
#include "../Texture3D.h"
#include "../FBO.h"
#include "../Camera.h"
#include "../VAO.h"
#include "../EBO.h"
#include "../Geometry.h"
#include "../Mesh.h"
#include "../AssetFolderPathManager.h"
#include <algorithm>
#include <iostream>
#include <fstream>

#define VOLUME_TEXTURE_WIDTH 1024
#define VOLUME_TEXTURE_HEIGHT 1024
#define TRANSFER_MODE_COUNT 5
#define ALL_FEATURES_STR "All features"

namespace Hogra::Volumetric {
	class VolumeObject : public Component
	{
	public:

		VolumeObject();

		~VolumeObject();

		void Init(Texture3D* voxels, const glm::vec3& _pos, const glm::vec3& _scale, const glm::quat& _orientation, PointLight* _light, const glm::ivec2& contextSize);

		void Draw(FBO& outFBO, const Texture2D& depthTexture, const Camera& camera);

		const glm::vec3& GetPosition() const;

		void SetPosition(const glm::vec3& pos);

		const glm::vec3 GetScale() const;

		void SetScale(const glm::vec3& _scale);

		const glm::quat& GetOrientation() const;

		void SetOrientation(const glm::quat& _orientation);

		PointLight* GetLight() const;

		void SetLight(PointLight* _light);

		Texture3D* GetTexture() const {
			return voxels;
		}

		const glm::mat4& GetModelMatrix() const {
			return modelMatrix;
		}

		void BeforePhysicsLoopUpdate() override;

		void LatePhysicsUpdate(float dt) override;

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

				nextGroupIdx = featureGroups.size() + 1;

				auto* all = Allocator::New<FeatureGroup>();
				all->name = ALL_FEATURES_STR;
				for (Feature& feature : transferFunction.GetFeatures()) {
					all->features.push_back(&feature);
				}
				all->serialize = false;
				featureGroups.emplace(featureGroups.begin(), all);
				selectedFeature = nullptr;
				ShowAll();
			}
		}

		void Serialize() override;

		void CycleSelectedFeature() {
			const auto* prevSelected = selectedFeature;
			selectedFeature = transferFunction.nextFeature(selectedFeature);
			if (prevSelected != selectedFeature) {
				std::cout << "Next: " << selectedFeature->name << std::endl;
				SetSelectedFeature(selectedFeature->name.c_str());
			}
		}

		void CreateBoundingBoxWireFrameGeometry();

		void ShowAll() {
			SetSelectedFeatureGroup(ALL_FEATURES_STR);
		}

		FeatureGroup* GetSelectedFeatureGroup() {
			return selectedFeatureGroup;
		}

		std::vector<FeatureGroup*>& GetFeatureGroups() {
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
			for (FeatureGroup* group : featureGroups) {
				if (group->name.compare(name) == 0) {
					if (selectedFeatureGroup != group) {
						selectedFeature = nullptr;
					}
					selectedFeatureGroup = group;
					if (selectedFeatureGroup != group) {
						selectedFeature = nullptr;
						ShowSelectedFeatureGroup();
					}
					break;
				}
			}
			ShowSelectedFeatureGroup();

			isChanged = true;
		}

		void FixateTransferFunction() {
			transferFunction.FixateFunction();
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

		void RemoveFeatureFromGroup(Feature* feature);

		void ResetToDefault()
		{
			transferFunction.clear();
			transferFunction.defaultTransferFunction();
			isChanged = true;
		}

		void CreateFeatureGroup()
		{
			auto* group = Allocator::New<FeatureGroup>();
			group->features.clear();
			group->name = std::string("Group").append(std::to_string(nextGroupIdx++));
			featureGroups.push_back(group);
			SetSelectedFeatureGroup(group->name.c_str());
		}

		void DeleteSelectedGroup();

		void RemoveSelectedFeatureFromFeatureGroup()
		{
			if (nullptr != selectedFeature && nullptr != selectedFeatureGroup) {
				selectedFeatureGroup->features.erase(std::find(selectedFeatureGroup->features.begin(),
					selectedFeatureGroup->features.end(),
					selectedFeature));
				selectedFeature = nullptr;
			}
		}

		float& GetDensity() {
			return density;
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
			isChanged = true;
		}

		void RotateModelAroundY(float rad_angle) {
			orientation = glm::angleAxis(rad_angle, glm::vec3(0.0f, 1.0f, 0.0f)) * orientation;
			isChanged = true;
		}

		void RotateModelAroundZ(float rad_angle) {
			orientation = glm::angleAxis(rad_angle, glm::vec3(0.0f, 0.0f, 1.0f)) * orientation;
			isChanged = true;
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

		int& GetSTFMinContributions() {
			return STFMinContributions;
		}

		void ToggleUsePBR() {
			usePBR = !usePBR;
		}

		void GenerateSTF()
		{
			transferFunction.SpatialTransferFunction(*voxels, STFradius, STFOpacity, STFEmission, STFMinContributions);
			std::cout << "STF feature count: " << transferFunction.GetFeatures().size() << std::endl;
			featureGroups.clear();

			nextGroupIdx = featureGroups.size() + 1;

			auto* all = Allocator::New<FeatureGroup>();
			all->name = ALL_FEATURES_STR;
			for (Feature& feature : transferFunction.GetFeatures()) {
				all->features.push_back(&feature);
			}
			all->serialize = false;
			featureGroups.emplace(featureGroups.begin(), all);
			selectedFeature = nullptr;
			ShowAll();
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

		TransferFunction& GetTransferFunction() {
			return transferFunction;
		}

		bool SelectTransferFunctionRegion(double x, double y);

		const char* GetCurrentTransferRegionSelectMode() const {
			return currentTransferRegionSelectMode;
		}

		const char** GetTransferRegionSelectModes() {
			return transferRegionSelectModes;
		}

		void SetCurrentTransferRegionSelectModes(const char* mode) {
			currentTransferRegionSelectMode = mode;
		}

		float& GetLightPower() {
			return lightPower;
		}

		glm::vec3& GetLightColor() {
			return lightColor;
		}

		void ForceRedraw() {
			isChanged = true;
		}

		void ResizeDisplayBoundingBox(const glm::vec3& w_min, const glm::vec3& w_max);

		void GetMinAndMax(glm::vec3& w_min, glm::vec3& w_max);

		void UpdateGui() override;

		bool GetShowNormals() const {
			return showNormals;
		}

		void SetShowNormals(bool b) {
			showNormals = b;
			isChanged = true;
		}

		bool GetHalfAngleSlicing() const {
			return useHalfAngleSlicing;
		}

		void ToggleHalfAngleSlicing() {
			useHalfAngleSlicing = !useHalfAngleSlicing;
			isChanged = true;
		}

		float& GetLocalShadows() {
			return localShadows;
		}

		float& GetGradientBasedIllumination() {
			return gradientBasedLocalIllumination;
		}

		void SetChanged() {
			isChanged = true;
		}

		void SetIsPlaneGrabbed(bool b) {
			isPlaneGrabbed = b;
		}

		glm::vec4& GetTransferDrawColor() {
			return transferDrawColor;
		}

	private:

		const char* transferRegionSelectModes[TRANSFER_MODE_COUNT] = { "Flood fill", "General area", "Select class", "Remove class", "Intensity band" };
		const char* currentTransferRegionSelectMode = "Select class";

		struct BoxEdge {
			glm::vec3 position;
			glm::vec3 direction;
			float length;
		};

		struct BoundingBox {
			BoxEdge edges[12];
			glm::vec3 corners[8];
		};

		bool IntersectPlane(const BoxEdge& edge, const glm::vec3& planePos, const glm::vec3& planeNormal, glm::vec3& intersection) {
			float t = dot(planePos - edge.position, planeNormal) / dot(edge.direction, planeNormal);
			if (t >= 0 && t < edge.length) {
				intersection = edge.position + edge.direction * t;
				return true;
			}
			return false;
		}

		void RayCasting(const Camera& camera, const Texture2D& depthTexture);

		void HalfAngleSlicing(const Camera& camera, const Texture2D& depthTexture);

		void DrawProxyGeometry(
			const Camera& camera,
			const Texture2D& depthTexture,
			bool isBackToFront,
			int in,
			int out,
			const glm::vec3& modelSlicePosition,
			const glm::vec3& modelSliceNormal,
			bool isCheapRender
		);

		void InitBoundingBox(const Dimensions& dimensions, BoundingBox& boundingBox);

		void ExportHalfAngleData(const ShaderProgram& program, const glm::mat4& lightViewProjMatrix, bool isBackToFront, const Camera& camera, const glm::vec3& w_sliceDelta);

		void ExportRayCastData(const ShaderProgram& program, const glm::mat4& quadModelMatrix, const glm::mat4& lightViewProjMatrix, const Camera& camera, float w_delta);

		void DrawBoundingBox();

		BoundingBox originalBoundingBox;
		BoundingBox boundingBox;			// Actually used box
		Mesh boundingBoxMesh;				// To display bounding box
		bool isPlaneGrabbed = false;

		BoundingGeometry boundingGeometry;	// Optimised box grid
		glm::vec3 w_position;
		glm::vec3 scale;
		glm::quat orientation;
		glm::mat4 modelMatrix;
		glm::mat4 invModelMatrix;

		// Spatial Transfer function:
		float STFradius;
		float STFOpacity;
		float STFEmission;
		int STFMinContributions = 500;

		PointLight* light;
		Texture3D* voxels = nullptr;

		glm::vec3 resolution = glm::vec3(1.0f);
		std::vector<FeatureGroup*> featureGroups;
		TransferFunction transferFunction;
		Feature* selectedFeature = nullptr;
		FeatureGroup* selectedFeatureGroup = nullptr;
		float density = 30.0f;
		float transferFloodFillTreshold;
		float lightPower = 100.0f;
		glm::vec3 lightColor = glm::vec3(1.0f, 1.0f, 0.9f);
		float levelOfDetail = 1.0f;		// (0..1]
		int nextGroupIdx = 1;
		bool showNormals = false;
		bool usePBR = false;
		float localShadows = 0.2f;
		float gradientBasedLocalIllumination = 0.6f;
		bool isChanged = true;
		glm::mat4 lightViewProjMatrix;

		glm::vec4 transferDrawColor = glm::vec4(1.0f, 0.95f, 0.8f, 1.0f);


		// For rendering finished image on screen:
		FBO prevCompleteImageFBO;	// Stores the last finished render of the volume
		Texture2D prevCompleteImage;
		ShaderProgram combineProgram;
		Geometry* quad = nullptr;

		//---------------------------------------------
		// For Half-angle slicing:
		FBO pingpongFBO;
		Texture2D colorTextures[2];
		Texture2D attenuationTextures[2];
		
		ShaderProgram colorProgram;
		ShaderProgram attenuationProgram;
		ShaderProgram colorCheapProgram;
		ShaderProgram attenuationCheapProgram;

		int sliceCount = 0;
		int firstSlice = 0;
		int out = 0;				// Idx of Out texture from the pingpong buffer
		float w_diameter = 0;

		bool isBackToFront = true;
		bool useHalfAngleSlicing = true;

		glm::vec3 w_center = glm::vec3(0.0f);
		glm::vec3 w_halfway = glm::vec3(1,0,0);

		//---------------------------------------------
		// For Ray casting:
		FBO rayCastOutFBO;
		Texture2D rayCastOutTexture;
		
		glm::ivec2 quadrantToRender = glm::ivec2(0, 0);
		ShaderProgram rayCastProgram;
		ShaderProgram rayCastCheapProgram;
		unsigned int xQuadCount = 32;
		unsigned int yQuadCount = 16;
		float w_delta = 0.05f;
	};
}

