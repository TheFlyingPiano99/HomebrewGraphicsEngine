#include "TransferFunction.h"

#include<glm/gtc/type_ptr.hpp>
#include<glm/gtc/matrix_transform.hpp>
#include<glm/gtx/rotate_vector.hpp>
#include <queue>
#include "AssetFolderPathManager.h"

namespace Hogra {

	float quadVertices[] =
	{
		//Coord	//texCoords
		1.0f, -1.0f,  1.0f,  0.0f,
	   -1.0f, -1.0f,  0.0f,  0.0f,
	   -1.0f,  1.0f,  0.0f,  1.0f,

		1.0f,  1.0f,  1.0f,  1.0f,
		1.0f, -1.0f,  1.0f,  0.0f,
	   -1.0f,  1.0f,  0.0f,  1.0f
	};

	void TransferFunction::Init() {
		shader.Init(
			AssetFolderPathManager::getInstance()->getShaderFolderPath().append("transfer.vert"),
			"",
			AssetFolderPathManager::getInstance()->getShaderFolderPath().append("transfer.frag")
		);
		quadVAO.Init();
		quadVAO.Bind();
		unsigned int quadVBO;
		glGenBuffers(1, &quadVBO);
		glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));
		quadVAO.Unbind();
		features.clear();
		defaultTransferFunction(glm::ivec2(256, 64));
		setCamSpacePosition(glm::vec2(0.0f, -0.75f));
	}

	void TransferFunction::crop(glm::vec2 min, glm::vec2 max)
	{
		glm::ivec2 dim = getDimensions();
		glm::ivec2 iMin = glm::ivec2(dim.x * min.x, dim.y * min.y);
		glm::ivec2 iMax = glm::ivec2(dim.x * max.x, dim.y * max.y);
		glm::vec2 center = (min + max) / 2.0f;
		glm::ivec2 iCenter = glm::ivec2(dim.x * center.x, dim.y * center.y);
		std::vector<glm::vec4> bytes(dim.x * dim.y);
		float radius = (max.x - min.x) / 2.0f;
		for (int y = 0; y < dim.y; y++) {
			for (int x = 0; x < dim.x; x++) {
				if (x >= iMin.x && x <= iMax.x && y >= iMin.y && y <= iMax.y) {
					bytes[y * dim.x + x] = glm::vec4(1.0f, 0.95f, 0.8f, 1.0f) * (1.0f - glm::length(glm::vec2(center) - glm::vec2(x, y) / glm::vec2(dim.x, dim.y)) / radius);
				}
				else {
					bytes[y * dim.x + x] = glm::vec4(0.0f);
				}
			}
		}
		Allocator<Texture2D>::Delete(texture);
		texture = Allocator<Texture2D>::New();
		texture->Init(bytes, dim, 1, GL_RGBA, GL_FLOAT);
	}

	void TransferFunction::floodFill(glm::vec2 startPos, glm::vec4 color, float threshold)
	{
		if (texture == nullptr)
			return;
		std::cout << "Flood fill" << std::endl;
		glm::ivec2 dim = texture->getDimensions();
		std::vector<glm::vec4> bytes(dim.x * dim.y);
		for (int i = 0; i < dim.x * dim.y; i++) {
			bytes[i] = glm::vec4(0.0f);
		}
		glm::vec4 toReplace = texture->operator()(startPos);
		std::queue<glm::ivec2> queue;
		queue.push(glm::ivec2(startPos.x * (dim.x - 1), startPos.y * (dim.y - 1)));
		while (!queue.empty()) {
			glm::ivec2 current = queue.front();
			queue.pop();
			glm::vec4 cv = bytes[current.y * dim.x + current.x];
			if (length(glm::vec3(cv - color)) > 0.0f) {
				bytes[current.y * dim.x + current.x] = color;
				if (current.x < dim.x - 1) {
					glm::ivec2 n = glm::ivec2(current.x + 1, current.y);
					if (length(glm::vec3(texture->operator()(n) - toReplace)) < threshold) {
						queue.push(n);
					}
				}
				if (current.x > 0) {
					glm::ivec2 n = glm::ivec2(current.x - 1, current.y);
					if (length(glm::vec3(texture->operator()(n) - toReplace)) < threshold) {
						queue.push(n);
					}
				}
				if (current.y < dim.y - 1) {
					glm::ivec2 n = glm::ivec2(current.x, current.y + 1);
					if (length(glm::vec3(texture->operator()(n) - toReplace)) < threshold) {
						queue.push(n);
					}
				}
				if (current.y > 0) {
					glm::ivec2 n = glm::ivec2(current.x, current.y - 1);
					if (length(glm::vec3(texture->operator()(n) - toReplace)) < threshold) {
						queue.push(n);
					}
				}
			}
		}
		Allocator<Texture2D>::Delete(texture);
		texture = Allocator<Texture2D>::New();
		texture->Init(bytes, dim, 1, GL_RGBA, GL_FLOAT);
	}

	void TransferFunction::blur(int kernelSize)
	{
		if (texture == nullptr)
			return;
		std::cout << "Blur" << std::endl;
		glm::ivec2 dim = texture->getDimensions();
		std::vector<glm::vec4> bytes(dim.x * dim.y);
		for (int i = 0; i < dim.x * dim.y; i++) {
			bytes[i] = glm::vec4(0.0f);
		}
		for (int y = 0; y < dim.y; y++) {
			for (int x = 0; x < dim.x; x++) {
				for (int yk = 0; yk < kernelSize; yk++) {
					for (int xk = 0; xk < kernelSize; xk++) {
						int yOffsetted = y + yk - kernelSize / 2;
						int xOffsetted = x + xk - kernelSize / 2;
						if (yOffsetted >= 0 && yOffsetted < dim.y
							&& xOffsetted >= 0 && xOffsetted < dim.x)
						{
							bytes[y * dim.x + x] += texture->operator()(glm::ivec2(xOffsetted, yOffsetted)) / (float)kernelSize / (float)kernelSize;
						}
					}
				}
			}
		}
		Allocator<Texture2D>::Delete(texture);
		texture = Allocator<Texture2D>::New();
		texture->Init(bytes, dim, 1, GL_RGBA, GL_FLOAT);
	}

	void TransferFunction::normalize()
	{
		if (texture == nullptr)
			return;
		std::cout << "Normalize" << std::endl;
		glm::ivec2 dim = texture->getDimensions();
		float max = 0.0f;
		for (int y = 0; y < dim.y; y++) {
			for (int x = 0; x < dim.x; x++) {
				glm::vec4 v = texture->operator()(glm::ivec2(x, y));
				float l = (v.x + v.y + v.z + v.w) / 4.0f;
				if (l > max) {
					max = l;
				}
			}
		}
		if (max == 0.0f)
			return;
		std::vector<glm::vec4> bytes(dim.x * dim.y);
		for (int y = 0; y < dim.y; y++) {
			for (int x = 0; x < dim.x; x++) {
				bytes[y * dim.x + x] = texture->operator()(glm::ivec2(x, y)) / max;
			}
		}
		Allocator<Texture2D>::Delete(texture);
		texture = Allocator<Texture2D>::New();
		texture->Init(bytes, dim, 1, GL_RGBA, GL_FLOAT);
	}

	void TransferFunction::edgeDetect()
	{
		if (texture == nullptr)
			return;
		std::cout << "Edge detect" << std::endl;
		glm::ivec2 dim = texture->getDimensions();
		std::vector<glm::vec4> bytes(dim.x * dim.y);
		for (int y = 0; y < dim.y; y++) {
			for (int x = 0; x < dim.x; x++) {
				bytes[y * dim.x + x] = texture->operator()(glm::ivec2(x, y)) * -4.0f;
				bytes[y * dim.x + x] += texture->operator()(glm::ivec2(x + 1, y));
				bytes[y * dim.x + x] += texture->operator()(glm::ivec2(x - 1, y));
				bytes[y * dim.x + x] += texture->operator()(glm::ivec2(x, y + 1));
				bytes[y * dim.x + x] += texture->operator()(glm::ivec2(x, y - 1));
			}
		}
		Allocator<Texture2D>::Delete(texture);
		texture = Allocator<Texture2D>::New();
		texture->Init(bytes, dim, 1, GL_RGBA, GL_FLOAT);
	}

	void TransferFunction::singleColor(glm::vec3 color)
	{
		if (texture == nullptr)
			return;
		std::cout << "Single color" << std::endl;
		glm::ivec2 dim = texture->getDimensions();
		std::vector<glm::vec4> bytes(dim.x * dim.y);
		for (int y = 0; y < dim.y; y++) {
			for (int x = 0; x < dim.x; x++) {
				bytes[y * dim.x + x] = texture->operator()(glm::ivec2(x, y));
				if (!(glm::vec3(0.0f) == (color - glm::vec3(bytes[y * dim.x + x].x, bytes[y * dim.x + x].y, bytes[y * dim.x + x].z)))) {
					bytes[y * dim.x + x] = glm::vec4(0.0f);
				}
			}
		}
		Allocator<Texture2D>::Delete(texture);
		texture = Allocator<Texture2D>::New();
		texture->Init(bytes, dim, 1, GL_RGBA, GL_FLOAT);
	}

	void TransferFunction::removeColor(glm::vec3 color)
	{
		if (texture == nullptr)
			return;
		std::cout << "Remove color" << std::endl;
		glm::ivec2 dim = texture->getDimensions();
		std::vector<glm::vec4> bytes(dim.x * dim.y);
		for (int y = 0; y < dim.y; y++) {
			for (int x = 0; x < dim.x; x++) {
				bytes[y * dim.x + x] = texture->operator()(glm::ivec2(x, y));
				if (glm::vec3(0.0f) == (color - glm::vec3(bytes[y * dim.x + x].x, bytes[y * dim.x + x].y, bytes[y * dim.x + x].z))) {
					bytes[y * dim.x + x] = glm::vec4(0.0f);
				}
			}
		}
		Allocator<Texture2D>::Delete(texture);
		texture = Allocator<Texture2D>::New();
		texture->Init(bytes, dim, 1, GL_RGBA, GL_FLOAT);
	}

	void TransferFunction::grayscale()
	{
		if (texture == nullptr)
			return;
		std::cout << "Grayscale" << std::endl;
		glm::ivec2 dim = texture->getDimensions();
		std::vector<glm::vec4> bytes(dim.x * dim.y);
		for (int y = 0; y < dim.y; y++) {
			for (int x = 0; x < dim.x; x++) {
				bytes[y * dim.x + x] = texture->operator()(glm::ivec2(x, y));
				float gray = (bytes[y * dim.x + x].r + bytes[y * dim.x + x].g + bytes[y * dim.x + x].b) * 3.0f;
				bytes[y * dim.x + x].r = gray;
				bytes[y * dim.x + x].g = gray;
				bytes[y * dim.x + x].b = gray;
			}
		}
		Allocator<Texture2D>::Delete(texture);
		texture = Allocator<Texture2D>::New();
		texture->Init(bytes, dim, 1, GL_RGBA, GL_FLOAT);
	}

	void TransferFunction::Draw(const FBO& fbo)
	{
		shader.Activate();
		fbo.Bind();
		glDisable(GL_CULL_FACE);
		glDisable(GL_DEPTH_TEST);
		glDisable(GL_BLEND);
		quadVAO.Bind();
		texture->Bind();

		glUniform1f(glGetUniformLocation(shader.ID, "exposure"), displayExposure);
		glUniform1f(glGetUniformLocation(shader.ID, "gamma"), displayGamma);
		glUniformMatrix4fv(glGetUniformLocation(shader.ID, "modelMatrix"), 1, GL_FALSE, glm::value_ptr(modelMatrix));
		glDrawArrays(GL_TRIANGLES, 0, 6);
		fbo.Unbind();
	}

	void TransferFunction::Bind()
	{
		if (texture != nullptr) {
			texture->Bind();
		}
	}

	void TransferFunction::Unbind()
	{
		if (texture != nullptr) {
			texture->Unbind();
		}
	}

	void TransferFunction::clear()
	{
		for (Feature& feature : features) {
			feature.visible = false;
		}
		if (texture == nullptr)
			return;
		glm::ivec2 dim = texture->getDimensions();
		std::vector<glm::vec4> bytes = std::vector<glm::vec4>(dim.x * dim.y);
		for (int y = 0; y < dim.y; y++) {
			for (int x = 0; x < dim.x; x++) {
				bytes[y * dim.x + x].r = 0.0f;
				bytes[y * dim.x + x].g = 0.0f;
				bytes[y * dim.x + x].b = 0.0f;
				bytes[y * dim.x + x].a = 0.0f;
			}
		}
		Allocator<Texture2D>::Delete(texture);
		texture = Allocator<Texture2D>::New();
		texture->Init(bytes, dim, 1, GL_RGBA, GL_FLOAT);
	}

	void TransferFunction::defaultTransferFunction(glm::ivec2 dimensions)
	{
		std::vector<glm::vec4> bytes = std::vector<glm::vec4>(dimensions.x * dimensions.y);
		for (int y = 0; y < dimensions.y; y++) {
			for (int x = 0; x < dimensions.x; x++) {
				if (x > 3 && x < 250) {
					bytes[y * dimensions.x + x].x = x / (float)dimensions.x;
					bytes[y * dimensions.x + x].y = x / (float)dimensions.x * x / (float)dimensions.x;
					bytes[y * dimensions.x + x].z = x / (float)dimensions.x * x / (float)dimensions.x;
					bytes[y * dimensions.x + x].w = std::pow((x - 3) / (float)dimensions.x, 0.01);
				}
				else if (x >= 250) {
					bytes[y * dimensions.x + x].x = x / (float)dimensions.x;
					bytes[y * dimensions.x + x].y = x / (float)dimensions.x * x / (float)dimensions.x;
					bytes[y * dimensions.x + x].z = x / (float)dimensions.x * x / (float)dimensions.x;
					bytes[y * dimensions.x + x].w = std::pow(1.0f - x / (float)dimensions.x, 10.0f);
				}
				else {
					bytes[y * dimensions.x + x].x = 0.0f;
					bytes[y * dimensions.x + x].y = 0.0f;
					bytes[y * dimensions.x + x].z = 0.0f;
					bytes[y * dimensions.x + x].w = 0.0f;
				}
			}
		}
		Allocator<Texture2D>::Delete(texture);
		Allocator<Texture2D>::Delete(texture);
		texture = Allocator<Texture2D>::New();
		texture->Init(bytes, dimensions, 1, GL_RGBA, GL_FLOAT);
	}

	void TransferFunction::spatialTransferFunction(glm::ivec2 dimensions, Texture3D& voxelTexture, float radius, float globalOpacity, float globalEmission) {
		features.clear();
		std::vector<glm::vec4> bytes = std::vector<glm::vec4>(dimensions.x * dimensions.y);
		std::vector<glm::vec3> barycenters = std::vector<glm::vec3>(dimensions.x * dimensions.y);
		std::vector <std::vector<glm::vec3>> contributingPositions = std::vector<std::vector<glm::vec3>>(dimensions.x * dimensions.y);
		std::vector<float> spatialVariances = std::vector<float>(dimensions.x * dimensions.y);
		for (int i = 0; i < dimensions.x * dimensions.y; i++) {
			bytes[i] = glm::vec4(0.0f);
			barycenters[i] = glm::vec3(0.0f);
		}
		for (int y = 0; y < dimensions.y; y++) {
			for (int x = 0; x < dimensions.x; x++) {
				bytes[y * dimensions.x + x].r = 0.0f;
				bytes[y * dimensions.x + x].g = 0.0f;
				bytes[y * dimensions.x + x].b = 0.0f;
				bytes[y * dimensions.x + x].a = x / (float)dimensions.x * globalOpacity;
			}
		}
		Dimensions voxelDim = voxelTexture.GetDimensions();
		glm::vec3 posDivider = glm::vec3(voxelDim.width, voxelDim.height, voxelDim.depth);

		//Calculate barycenters:
		for (int z = 0; z < voxelDim.depth; z++) {
			for (int y = 0; y < voxelDim.height; y++) {
				for (int x = 0; x < voxelDim.width; x++) {
					glm::vec4 gradientIntensity = voxelTexture.ResampleGradientAndDensity(glm::ivec3(x, y, z));
					int xb = gradientIntensity.w * (dimensions.x - 1);
					int yb = glm::length(glm::vec3(gradientIntensity.x, gradientIntensity.y, gradientIntensity.z)) * (dimensions.y - 1);
					if (xb >= 0 && xb < dimensions.x && yb >= 0 && yb < dimensions.y) {
						glm::vec3 p = glm::vec3(x, y, z) / posDivider;
						barycenters[yb * dimensions.x + xb] += p;
						contributingPositions[yb * dimensions.x + xb].push_back(p);
					}
				}
			}
		}
		//Calculate spatial variances:
		for (int i = 0; i < dimensions.x * dimensions.y; i++) {
			if (contributingPositions[i].size() > 0) {
				barycenters[i] /= (float)contributingPositions[i].size();
				spatialVariances[i] = 0.0f;
				for (int j = 0; j < contributingPositions[i].size(); j++) {
					spatialVariances[i] += glm::length(contributingPositions[i][j] - barycenters[i]);
				}
				spatialVariances[i] /= (float)contributingPositions[i].size();
			}
		}

		// Classification:
		srand(0);
		int featureCunter = 0;
		for (int i = 0; i < dimensions.x * dimensions.y; i++) {

			if (contributingPositions[i].size() > 0 && bytes[i].r == 0.0f && bytes[i].g == 0.0f && bytes[i].b == 0.0f) {
				glm::vec3 Crgb = glm::vec3(rand() % 255 / 255.0f, rand() % 255 / 255.0f, rand() % 255 / 255.0f);
				glm::vec3 b0 = barycenters[i];
				float v0 = spatialVariances[i];
				Feature feature;
				feature.color = Crgb;
				feature.visible = true;
				feature.name = std::string("Feature").append(std::to_string(featureCunter));
				feature.opacity = globalOpacity;
				feature.emission = globalEmission;
				for (int j = 0; j < dimensions.x * dimensions.y; j++) {
					if (contributingPositions[j].size() > 0 && bytes[j].r == 0.0f && bytes[j].g == 0.0f && bytes[j].b == 0.0f) {
						float distanceNorm = glm::length(barycenters[j] - b0) + std::abs(v0 - spatialVariances[j]);
						if (distanceNorm < radius) {
							bytes[j].r = Crgb.r * globalEmission;
							bytes[j].g = Crgb.g * globalEmission;
							bytes[j].b = Crgb.b * globalEmission;
							feature.elements.push_back(glm::ivec2(j % dimensions.x, j / dimensions.x));
						}
					}
				}
				if (0 < feature.elements.size()) {
					features.push_back(feature);
					featureCunter++;
				}
			}
		}

		Allocator<Texture2D>::Delete(texture);
		texture = Allocator<Texture2D>::New();
		texture->Init(bytes, dimensions, 1, GL_RGBA, GL_FLOAT);
	}

	void TransferFunction::gradientWeighted(glm::ivec2 dimensions, float globalOpacity)
	{
		std::vector<glm::vec4> bytes = std::vector<glm::vec4>(dimensions.x * dimensions.y);
		for (int y = 0; y < dimensions.y; y++) {
			for (int x = 0; x < dimensions.x; x++) {
				bytes[y * dimensions.x + x] = glm::vec4(y / (float)dimensions.y * globalOpacity);
			}
		}
		Allocator<Texture2D>::Delete(texture);
		texture = Allocator<Texture2D>::New();
		texture->Init(bytes, dimensions, 1, GL_RGBA, GL_FLOAT);
	}

	void TransferFunction::colorFeature(Feature& feature, glm::vec3 color)
	{
		glm::ivec2 dimensions = getDimensions();
		if (dimensions.x * dimensions.y == 0) {
			return;
		}
		std::vector<glm::vec4> bytes = std::vector<glm::vec4>(dimensions.x * dimensions.y);
		for (int i = 0; i < dimensions.x * dimensions.y; i++) {
			bytes[i] = texture->GetBytes()[i];
		}
		for (auto p : feature.elements) {
			if (p.x < dimensions.x && p.y < dimensions.y) {
				bytes[p.y * dimensions.x + p.x].r = color.r * feature.emission;
				bytes[p.y * dimensions.x + p.x].g = color.g * feature.emission;
				bytes[p.y * dimensions.x + p.x].b = color.b * feature.emission;
				bytes[p.y * dimensions.x + p.x].w = std::pow((p.x - 3) / (float)dimensions.x, 0.01);
			}
		}

		Allocator<Texture2D>::Delete(texture);
		texture = Allocator<Texture2D>::New();
		texture->Init(bytes, dimensions, 1, GL_RGBA, GL_FLOAT);
	}

	bool TransferFunction::setFeatureVisibility(Feature& feature, bool visibility)
	{
		if (feature.visible == visibility)
			return false;
		feature.visible = visibility;
		colorFeature(feature, (visibility) ? feature.color : glm::vec3(0, 0, 0));
		return true;
	}

	bool TransferFunction::setFeatureOpacity(Feature& feature, float opacity)
	{
		if (opacity == feature.opacity) {
			return false;
		}
		feature.opacity = opacity;
		if (!feature.visible) {
			return false;
		}
		colorFeature(feature, feature.color);
		return true;
	}

	Feature* TransferFunction::getFeatureFromPosition(glm::vec2 pos)
	{
		glm::ivec2 dimensions = getDimensions();
		glm::ivec2 iPos = glm::ivec2(dimensions.x * pos.x, dimensions.y * pos.y);
		for (Feature& feature : features) {
			if (std::find(feature.elements.begin(), feature.elements.end(), iPos) != feature.elements.end()) {
				return &feature;
			}
		}
		return nullptr;
	}

	void TransferFunction::operator=(TransferFunction& transferFunction)
	{
		shader = transferFunction.shader;
		features = transferFunction.features;
		glm::ivec2 dim = transferFunction.getDimensions();
		if (dim.x == 0 || dim.y == 0) {	// Clear if empty.
			Allocator<Texture2D>::Delete(texture);
			return;
		}
		std::vector<glm::vec4> bytes(dim.x * dim.y);
		for (int y = 0; y < dim.y; y++) {
			for (int x = 0; x < dim.x; x++) {
				bytes[y * dim.x + x] = transferFunction(glm::ivec2(x, y));
			}
		}
		Allocator<Texture2D>::Delete(texture);
		texture = Allocator<Texture2D>::New();
		texture->Init(bytes, dim, 1, GL_RGBA, GL_FLOAT);
	}

	void TransferFunction::setCamSpacePosition(glm::vec2 camPos)
	{
		preferedCameraSpacePosition = camPos;
	}

	Feature* TransferFunction::findFeatureByName(const char* name) {
		for (Feature& feature : features) {
			if (std::string(name).compare(feature.name) == 0) {
				return &feature;
			}
		}
		return nullptr;
	}

	Feature* TransferFunction::nextFeature(Feature* current)
	{
		if (features.empty()) {
			return nullptr;
		}
		if (nullptr == current) {
			return &features[0];
		}
		std::vector<Feature>::iterator iter = std::find(features.begin(), features.end(), *current);
		iter++;
		if (features.end() == iter) {
			return &features[0];
		}
		return &*iter;
	}

	void TransferFunction::showAll()
	{
		for (Feature& feature : features) {
			feature.visible = true;
			colorFeature(feature, feature.color);
		}
	}

	void TransferFunction::showVisible()
	{
		for (Feature& feature : features) {
			if (feature.visible) {
				colorFeature(feature, feature.color);
			}
		}
	}

	static float timer = 0.0f;
	static bool prevVisible = false;
	void TransferFunction::Animate(float dt)
	{
		const float maxTime = 1.0f;
		if (prevVisible != visible) {
			prevVisible = visible;
			timer = maxTime;
		}
		if (timer > 0.0f) {
			if (visible) {
				cameraSpacePosition = (1.0f - timer / maxTime) * preferedCameraSpacePosition
					+ timer / maxTime * glm::vec2(0, -1.2);
			}
			else {
				cameraSpacePosition = (timer / maxTime) * preferedCameraSpacePosition
					+ (1.0f - timer / maxTime) * glm::vec2(0, -1.2);
			}

			timer -= dt;
			if (timer < 0.0f) {
				timer = 0.0f;
			}
			float aspectRatio = getDimensions().x / (float)getDimensions().y;
			modelMatrix = glm::translate(glm::vec3(cameraSpacePosition.x, cameraSpacePosition.y, 0.0f)) * glm::scale(glm::vec3(0.5f, 0.2f, 1.0f));
			invModelMatrix = glm::inverse(modelMatrix);
		}

	}
}
