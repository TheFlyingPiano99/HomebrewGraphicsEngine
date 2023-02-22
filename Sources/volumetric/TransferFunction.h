#pragma once

#include<glm/glm.hpp>
#include <iostream>
#include <fstream>
#include "../ShaderProgram.h"
#include "../Texture2D.h"
#include "../VAO.h"
#include "../Texture3D.h"
#include "../FBO.h"

namespace Hogra::Volumetric {
	struct Feature {
		std::vector<glm::ivec2> elements;
		glm::vec3 color;
		std::string name;
		bool visible;
		float opacity;
		float emission;

		bool operator==(Feature feature) {
			return (name.compare(feature.name) == 0);
		}

		void save(std::ostream& stream) {
			if (name.compare("") == 0) {
				return;
			}
			stream << "feature" << std::endl;
			stream << "name:" << name << std::endl;
			stream << "color:" << color.r << ":" << color.g << ":" << color.b << std::endl;
			stream << "opacity:" << opacity << std::endl;
			stream << "emission:" << emission << std::endl;
			stream << "elements:";
			for (int i = 0; i < elements.size(); i++) {
				glm::ivec2 v = elements[i];
				stream << v.x << ":";
				stream << v.y;
				if (i < elements.size() - 1) {
					stream << ":";
				}
			}
			stream << std::endl;
			stream << "/feature" << std::endl << std::endl;
		}

		/*
		* Returns true if loading is successful.
		*/
		bool load(std::istream& stream) {
			std::string line;
			std::string delimiter = ":";
			bool nameLoaded = false;
			bool colorLoaded = false;
			bool opacityLoaded = false;
			bool emissionLoaded = false;
			bool elementsLoaded = false;
			while (std::getline(stream, line)) {
				std::vector<std::string> tokens;
				while (!line.empty()) {
					tokens.push_back(line.substr(0, line.find(delimiter)));
					line.erase(0, (*tokens.rbegin()).length());
					line.erase(0, line.find(delimiter) + delimiter.length());
				}
				if (tokens.size() > 1) {
					if (tokens[0].compare("name") == 0) {
						name = tokens[1];
						nameLoaded = true;
					}
					if (tokens[0].compare("color") == 0) {
						color.x = std::stof(tokens[1]);
						color.y = std::stof(tokens[2]);
						color.z = std::stof(tokens[3]);
						colorLoaded = true;
					}
					else if (tokens[0].compare("opacity") == 0) {
						opacity = std::stof(tokens[1]);
						opacityLoaded = true;
					}
					else if (tokens[0].compare("emission") == 0) {
						emission = std::stof(tokens[1]);
						emissionLoaded = true;
					}
					else if (tokens[0].compare("elements") == 0) {
						elements.clear();
						for (int i = 1; i < tokens.size() - 1; i += 2) {
							glm::ivec2 v(std::stoi(tokens[i]), std::stoi(tokens[i + 1]));
							elements.push_back(v);
						}
						elementsLoaded = true;
					}
				}
				else if (tokens.size() > 0 && tokens[0].compare("/feature") == 0) {
					return nameLoaded && colorLoaded && opacityLoaded && emissionLoaded && elementsLoaded;
				}
			}
		}
	};

	struct FeatureGroup {
		std::vector<Feature*> features;
		std::string name;
		bool serialize = true;

		void Serialize(std::ostream& stream) {
			if (name.compare("") == 0 || !serialize) {
				return;
			}
			stream << "featureGroup" << std::endl;
			stream << "name:" << name << std::endl;
			stream << "features:";
			for (int i = 0; i < features.size(); i++) {
				stream << features[i]->name;
				if (i < features.size() - 1) {
					stream << ":";
				}
			}
			stream << std::endl;
			stream << "/featureGroup" << std::endl << std::endl;
		}

		bool load(std::istream& stream, std::vector<Feature>& features) {
			std::string line;
			std::string delimiter = ":";
			bool nameLoaded = false;
			bool featuresLoaded = false;
			while (std::getline(stream, line)) {
				std::vector<std::string> tokens;
				while (!line.empty()) {
					tokens.push_back(line.substr(0, line.find(delimiter)));
					line.erase(0, (*tokens.rbegin()).length());
					line.erase(0, line.find(delimiter) + delimiter.length());
				}
				if (tokens.size() > 1) {
					if (tokens[0].compare("name") == 0) {
						name = tokens[1];
						nameLoaded = true;
					}
					else if (tokens[0].compare("features") == 0) {
						for (int i = 1; i < tokens.size(); i++) {
							for (int j = 0; j < features.size(); j++) {
								if (features[j].name.compare(tokens[i]) == 0) {
									this->features.push_back(&features[j]);
								}
							}
						}
						featuresLoaded = true;
					}
					else if (tokens[0].compare("/featureGroup")) {
						return nameLoaded && featuresLoaded;
					}
				}
			}
		}
	};

	class TransferFunction
	{
	private:
		ShaderProgram shader;
		glm::mat4 modelMatrix;
		glm::mat4 invModelMatrix;
		Texture2D* texture = nullptr;
		VAO quadVAO;
		float displayExposure = 1.0f;
		float displayGamma = 1.0f;
		glm::vec4 nullVector = glm::vec4(0.0f);
		std::vector<Feature> features;
		std::vector<glm::vec4> fixedColor;
		bool visible = true;
		glm::vec2 preferedCameraSpacePosition = glm::vec2(0, 0);
		glm::vec2 cameraSpacePosition = glm::vec2(0, 0);
		glm::ivec2 dimensions = glm::ivec2(256, 128);

	public:
		void Init();

		~TransferFunction() {
			Allocator::Delete(texture);
		}

		void generalArea(glm::vec2 min, glm::vec2 max, const glm::vec4& color);
		void floodFill(glm::vec2 startPos, glm::vec4 color, float threshold);
		void blur(int kernelSize);
		void normalize();
		void edgeDetect();
		void singleColor(glm::vec3 color);
		void removeColor(glm::vec3 color);
		void grayscale();
		void Draw(const FBO& fbo);
		void Bind();
		void Unbind();
		void clear();
		void intensityBand(glm::vec2 min, glm::vec2 max, glm::vec4& color);

		const glm::ivec2 GetDimensions() {
			if (texture == nullptr)
				return glm::ivec2(0);
			return texture->GetDimensions();
		}

		const glm::mat4 GetModelMatrix() {
			return modelMatrix;
		}

		const glm::mat4 GetInvModelMatrix() {
			return invModelMatrix;
		}

		void FixateFunction() {
			if (texture != nullptr) {
				for (int i = 0; i < fixedColor.size(); i++) {
					fixedColor[i] = texture->GetBytes()[i];
				}
			}
		}

		void defaultTransferFunction();
		void SpatialTransferFunction(Texture3D& voxelTexture, float radius, float globalOpacity, float globalEmission, int minimalContributions = 1);
		void gradientWeighted(glm::ivec2 dimensions, float globalOpacity);

		void colorFeature(Feature& feature, glm::vec3 color);

		bool setFeatureVisibility(Feature& feature, bool visibility);

		bool setFeatureOpacity(Feature& feature, float opacity);

		Feature* getFeatureFromPosition(glm::vec2 pos);

		const glm::vec4& operator()(glm::ivec2 position) const {
			if (texture == nullptr)
				return nullVector;
			return texture->operator()(position);
		}

		const glm::vec4& operator()(glm::vec2 normalisedPosition) const {
			if (texture == nullptr)
				return nullVector;
			return texture->operator()(normalisedPosition);
		}

		void operator=(TransferFunction& transferFunction);

		void setCamSpacePosition(glm::vec2 camPos);

		float& getExposure() {
			return displayExposure;
		}

		float& getGamma() {
			return displayGamma;
		}

		std::vector<Feature>& GetFeatures() {
			return features;
		}

		void setFeatures(std::vector<Feature> _features) {
			features = _features;
		}

		Feature* findFeatureByName(const char* name);

		Feature* nextFeature(Feature* current);

		void showAll();

		void showVisible();

		void saveFeatures(std::ostream& stream) {
			for (Feature& feature : features) {
				feature.save(stream);
			}
		}

		void loadFeatures(std::istream& stream, std::vector<FeatureGroup*>& groups) {
			features.clear();
			std::string line;
			while (std::getline(stream, line)) {
				if (line.compare("feature") == 0) {
					Feature feature;
					if (feature.load(stream)) {
						feature.visible = false;
						features.push_back(feature);
					}
				}
				else if (line.compare("featureGroup") == 0) {
					auto* group = Allocator::New<FeatureGroup>();
					if (group->load(stream, features)) {
						groups.push_back(group);
					}
				}
			}
		}

		void ToggleVisibility() {
			visible = !visible;
		}

		void Animate(float dt);

		bool IsVisible() {
			return visible;
		}
	};


}
