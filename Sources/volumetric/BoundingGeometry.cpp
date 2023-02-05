#include "BoundingGeometry.h"
#include"../EBO.h"
#include "../VBO.h"
#include "../AssetFolderPathManager.h"
#include "../GlobalInclude.h"
#include<glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>

#define BOX_STEP_SIZE 1

namespace Hogra::Volumetric {

	glm::vec3 cubeVertices[] =
	{
		glm::vec3(0.0f, 0.0f,  1.0f),
		glm::vec3(0.0f, 0.0f, 0.0f),
		glm::vec3(1.0f, 0.0f, 0.0f),
		glm::vec3(1.0f, 0.0f,  1.0f),
		glm::vec3(0.0f,  1.0f,  1.0f),
		glm::vec3(0.0f,  1.0f, 0.0f),
		glm::vec3(1.0f,  1.0f, 0.0f),
		glm::vec3(1.0f,  1.0f,  1.0f)
	};

	GLuint cubeIndices[] =
	{
		0, 2, 1,
		0, 3, 2,
		0, 4, 7,
		0, 7, 3,
		3, 7, 6,
		3, 6, 2,
		2, 6, 5,
		2, 5, 1,
		1, 5, 4,
		1, 4, 0,
		4, 5, 6,
		4, 6, 7
	};

	void BoundingGeometry::Init() {
		vao.Init();
		enterFBO.Init();
		exitFBO.Init();
		lightEnterFBO.Init();

		depthRBO.Init(
			GL_DEPTH_COMPONENT,
			GlobalVariables::windowWidth,
			GlobalVariables::windowHeight
		);

		frontFaceTexture.Init(
			GL_RGBA16F,
			glm::ivec2(GlobalVariables::windowWidth, GlobalVariables::windowHeight),
			0,
			GL_RGBA,
			GL_FLOAT
		);
		backFaceTexture.Init(
			GL_RGBA16F,
			glm::ivec2(GlobalVariables::windowWidth, GlobalVariables::windowHeight),
			2,
			GL_RGBA,
			GL_FLOAT
		);
		lightFaceTexture.Init(
			GL_RGBA16F,
			glm::ivec2(GlobalVariables::windowWidth, GlobalVariables::windowHeight),
			4,
			GL_RGBA,
			GL_FLOAT
		);
		enterFBO.LinkTexture(GL_COLOR_ATTACHMENT0, frontFaceTexture);
		enterFBO.LinkRBO(GL_DEPTH_ATTACHMENT, depthRBO);
		exitFBO.LinkTexture(GL_COLOR_ATTACHMENT0, backFaceTexture);
		exitFBO.LinkRBO(GL_DEPTH_ATTACHMENT, depthRBO);
		lightEnterFBO.LinkTexture(GL_COLOR_ATTACHMENT0, lightFaceTexture);
		lightEnterFBO.LinkRBO(GL_DEPTH_ATTACHMENT, depthRBO);

		modelPosShader.Init(
			AssetFolderPathManager::getInstance()->getShaderFolderPath().append("volumeBoundingGeometry.vert").c_str(),
			"",
			AssetFolderPathManager::getInstance()->getShaderFolderPath().append("volumeBoundingGeometry.frag").c_str()
		);
		flatColorShader.Init(
			AssetFolderPathManager::getInstance()->getShaderFolderPath().append("volumeBoundingGeometry.vert").c_str(),
			"",
			AssetFolderPathManager::getInstance()->getShaderFolderPath().append("boundingGeometryFlatColor.frag").c_str()
		);
		threshold = 0.01f;
	}

	void BoundingGeometry::AddCuboid(
		const glm::vec3& scale, 
		const glm::vec3& translation
	) 
	{
		int indexOffset = vertices.size();
		for (int i = 0; i < 8; i++) {
			vertices.push_back(cubeVertices[i] * scale + translation);
		}
		for (int i = 0; i < 36; i++) {
			indices.push_back(cubeIndices[i] + indexOffset);
		}
	}


	void BoundingGeometry::CalculateDivision(
		const Dimensions& dimensions, 
		unsigned int& xDivision, 
		unsigned int& yDivision, 
		unsigned int& zDivision
	)
	{
		unsigned int initX = 64;
		unsigned int initY = 64;
		unsigned int initZ = 64;
		for (unsigned int i = initX; i >= 1; i--) {
			if (dimensions.width % i == 0) {
				xDivision = i;
				break;
			}
		}
		for (unsigned int i = initY; i >= 1; i--) {
			if (dimensions.height % i == 0) {
				yDivision = i;
				break;
			}
		}
		for (unsigned int i = initZ; i >= 1; i--) {
			if (dimensions.depth % i == 0) {
				zDivision = i;
				break;
			}
		}
	}


	void BoundingGeometry::CreateVertexGrid(
		const Dimensions& dimensions, 
		const unsigned int& xDivision, 
		const unsigned int& yDivision, 
		const unsigned int& zDivision
	)
	{
		glm::vec3 blockSize = glm::vec3(dimensions.width, dimensions.height, dimensions.depth)
							/ glm::vec3(xDivision, yDivision, zDivision);

		for (unsigned int z = 0; z < zDivision + 1; z++) {
			for (unsigned int y = 0; y < yDivision + 1; y++) {
				for (unsigned int x = 0; x < xDivision + 1; x++) {
					vertices.push_back(glm::vec3(x, y, z) * blockSize - glm::vec3(dimensions.width, dimensions.height, dimensions.depth) / 2.0f);
				}
			}
		}
	}


	void BoundingGeometry::AddPlusZSide(
		const int x, 
		const int y, 
		const int z, 
		const unsigned int& xDivision, 
		const unsigned int& yDivision, 
		const unsigned int& zDivision
	)
	{
		//Lower polygon:
		indices.push_back(IndexVertices(x + 1, y, z + 1, xDivision, yDivision, zDivision));
		indices.push_back(IndexVertices(x, y, z + 1, xDivision, yDivision, zDivision));
		indices.push_back(IndexVertices(x, y + 1, z + 1, xDivision, yDivision, zDivision));

		//Upper polygon:
		indices.push_back(IndexVertices(x + 1, y, z + 1, xDivision, yDivision, zDivision));
		indices.push_back(IndexVertices(x, y + 1, z + 1, xDivision, yDivision, zDivision));
		indices.push_back(IndexVertices(x + 1, y + 1, z + 1, xDivision, yDivision, zDivision));
	}


	void BoundingGeometry::AddMinusZSide(
		const int x, 
		const int y, 
		const int z, 
		const unsigned int& xDivision, 
		const unsigned int& yDivision, 
		const unsigned int& zDivision
	)
	{
		//Lower polygon:
		indices.push_back(IndexVertices(x, y, z, xDivision, yDivision, zDivision));
		indices.push_back(IndexVertices(x + 1, y, z, xDivision, yDivision, zDivision));
		indices.push_back(IndexVertices(x + 1, y + 1, z, xDivision, yDivision, zDivision));

		//Upper polygon:
		indices.push_back(IndexVertices(x, y, z, xDivision, yDivision, zDivision));
		indices.push_back(IndexVertices(x + 1, y + 1, z, xDivision, yDivision, zDivision));
		indices.push_back(IndexVertices(x, y + 1, z, xDivision, yDivision, zDivision));
	}


	void BoundingGeometry::AddPlusYSide(
		const int x, 
		const int y, 
		const int z, 
		const unsigned int& xDivision, 
		const unsigned int& yDivision, 
		const unsigned int& zDivision
	)
	{
		//Lower polygon:
		indices.push_back(IndexVertices(x, y + 1, z, xDivision, yDivision, zDivision));
		indices.push_back(IndexVertices(x + 1, y + 1, z, xDivision, yDivision, zDivision));
		indices.push_back(IndexVertices(x + 1, y + 1, z + 1, xDivision, yDivision, zDivision));

		//Upper polygon:
		indices.push_back(IndexVertices(x, y + 1, z, xDivision, yDivision, zDivision));
		indices.push_back(IndexVertices(x + 1, y + 1, z + 1, xDivision, yDivision, zDivision));
		indices.push_back(IndexVertices(x, y + 1, z + 1, xDivision, yDivision, zDivision));
	}


	void BoundingGeometry::AddMinusYSide(
		const int x, 
		const int y, 
		const int z, 
		const unsigned int& xDivision, 
		const unsigned int& yDivision, 
		const unsigned int& zDivision
	)
	{
		//Lower polygon:
		indices.push_back(IndexVertices(x, y, z, xDivision, yDivision, zDivision));
		indices.push_back(IndexVertices(x + 1, y, z + 1, xDivision, yDivision, zDivision));
		indices.push_back(IndexVertices(x + 1, y, z, xDivision, yDivision, zDivision));

		//Upper polygon:
		indices.push_back(IndexVertices(x, y, z, xDivision, yDivision, zDivision));
		indices.push_back(IndexVertices(x, y, z + 1, xDivision, yDivision, zDivision));
		indices.push_back(IndexVertices(x + 1, y, z + 1, xDivision, yDivision, zDivision));
	}


	void BoundingGeometry::AddPlusXSide(
		const int x, 
		const int y, 
		const int z, 
		const unsigned int& xDivision, 
		const unsigned int& yDivision, 
		const unsigned int& zDivision
	)
	{
		//Lower polygon:
		indices.push_back(IndexVertices(x + 1, y, z, xDivision, yDivision, zDivision));
		indices.push_back(IndexVertices(x + 1, y, z + 1, xDivision, yDivision, zDivision));
		indices.push_back(IndexVertices(x + 1, y + 1, z + 1, xDivision, yDivision, zDivision));

		//Upper polygon:
		indices.push_back(IndexVertices(x + 1, y, z, xDivision, yDivision, zDivision));
		indices.push_back(IndexVertices(x + 1, y + 1, z + 1, xDivision, yDivision, zDivision));
		indices.push_back(IndexVertices(x + 1, y + 1, z, xDivision, yDivision, zDivision));
	}


	void BoundingGeometry::AddMinusXSide(
		const int x, 
		const int y, 
		const int z, 
		const unsigned int& xDivision, 
		const unsigned int& yDivision, 
		const unsigned int& zDivision
	)
	{
		//Lower polygon:
		indices.push_back(IndexVertices(x, y, z + 1, xDivision, yDivision, zDivision));
		indices.push_back(IndexVertices(x, y, z, xDivision, yDivision, zDivision));
		indices.push_back(IndexVertices(x, y + 1, z, xDivision, yDivision, zDivision));

		//Upper polygon:
		indices.push_back(IndexVertices(x, y, z + 1, xDivision, yDivision, zDivision));
		indices.push_back(IndexVertices(x, y + 1, z, xDivision, yDivision, zDivision));
		indices.push_back(IndexVertices(x, y + 1, z + 1, xDivision, yDivision, zDivision));
	}


	unsigned int BoundingGeometry::IndexVertices(
		const int x,
		const int y, 
		const int z, 
		const unsigned int& xDivision, 
		const unsigned int& yDivision, 
		const unsigned int& zDivision
	)
	{
		return z * (yDivision + 1) * (xDivision + 1) + y * (xDivision + 1) + x;
	}


	unsigned int BoundingGeometry::IndexDivisionSized(
		const int x, 
		const int y, 
		const int z, 
		const unsigned int& xDivision, 
		const unsigned int& yDivision, 
		const unsigned int& zDivision
	)
	{

		return z * yDivision * xDivision + y * xDivision + x;
	}


	void BoundingGeometry::CalculateFilled(
		const Dimensions& dimensions,
		const unsigned int& xDivision,
		const unsigned int& yDivision,
		const unsigned int& zDivision,
		bool* isFilled,
		const Texture3D& voxelTexture,
		const TransferFunction& transferFunction,
		unsigned int step
	)
	{
		std::vector<float> averageOpacity = std::vector<float>(xDivision * yDivision * zDivision);
		for (int i = 0; i < xDivision * yDivision * zDivision; i++) {
			averageOpacity[i] = 0.0f;
		}
		int xBlockSize = dimensions.width / xDivision;
		int yBlockSize = dimensions.height / yDivision;
		int zBlockSize = dimensions.depth / zDivision;
		int voxelsPerBlock = xBlockSize * yBlockSize * zBlockSize;
		for (int z = step; z < dimensions.depth && z < step + BOX_STEP_SIZE; z++) {
			if (z % 20 == 0) {
				std::cout << "Completion: " << z / (float)dimensions.depth * 100.0f << "%" << std::endl;
			}
			for (int y = 0; y < dimensions.height; y++) {
				for (int x = 0; x < dimensions.width; x++) {
					glm::vec4 gradientIntensity = voxelTexture.ResampleGradientAndDensity(glm::ivec3(x, y, z));
					float gradientLength = glm::length(glm::vec3(gradientIntensity.x, gradientIntensity.y, gradientIntensity.z));
					float intensity = gradientIntensity.a;
					glm::vec4 colorOpacity = transferFunction(glm::vec2(intensity, gradientLength));
					float opacity = colorOpacity.w;
					flatColor.r += colorOpacity.r;
					flatColor.g += colorOpacity.g;
					flatColor.b += colorOpacity.b;
					averageOpacity[IndexDivisionSized((x / xBlockSize), (y / yBlockSize), (z / zBlockSize), xDivision, yDivision, zDivision)]
						+= opacity / (float)voxelsPerBlock;
					isFilled[IndexDivisionSized((x / xBlockSize), (y / yBlockSize), (z / zBlockSize), xDivision, yDivision, zDivision)] 
						= (threshold <= averageOpacity[IndexDivisionSized((x / xBlockSize), (y / yBlockSize), (z / zBlockSize), xDivision, yDivision, zDivision)]);
				}
			}
		}
	}


	void BoundingGeometry::CreateIndices(const unsigned int& xDivision, const unsigned int& yDivision, const unsigned int& zDivision, bool* isFilled)
	{
		for (int z = 0; z < zDivision; z++) {
			for (int y = 0; y < yDivision; y++) {
				for (int x = 0; x < xDivision; x++) {
					if (isFilled[z * yDivision * xDivision + y * xDivision + x]) {
						if (z < zDivision - 1 && !isFilled[(z + 1) * yDivision * xDivision + y * xDivision + x]
							|| z == zDivision - 1) {
							AddPlusZSide(x, y, z, xDivision, yDivision, zDivision);
						}
						if (z > 0 && !isFilled[(z - 1) * yDivision * xDivision + y * xDivision + x]
							|| z == 0) {
							AddMinusZSide(x, y, z, xDivision, yDivision, zDivision);
						}
						if (y < yDivision - 1 && !isFilled[z * yDivision * xDivision + (y + 1) * xDivision + x]
							|| y == yDivision - 1) {
							AddPlusYSide(x, y, z, xDivision, yDivision, zDivision);
						}
						if (y > 0 && !isFilled[z * yDivision * xDivision + (y - 1) * xDivision + x]
							|| y == 0) {
							AddMinusYSide(x, y, z, xDivision, yDivision, zDivision);
						}
						if (x < xDivision - 1 && !isFilled[z * yDivision * xDivision + y * xDivision + x + 1]
							|| x == xDivision - 1) {
							AddPlusXSide(x, y, z, xDivision, yDivision, zDivision);
						}
						if (x > 0 && !isFilled[z * yDivision * xDivision + y * xDivision + x - 1]
							|| x == 0) {
							AddMinusXSide(x, y, z, xDivision, yDivision, zDivision);
						}
					}
				}
			}
		};
	}

	void BoundingGeometry::UpdateGeometry(FullBox _, const Texture3D& voxelTexture, const TransferFunction& transferFunction) {
		this->threshold = threshold;
		vertices.clear();
		indices.clear();

		Dimensions dimensions = voxelTexture.GetDimensions();
		unsigned int xDivision, yDivision, zDivision;
		CalculateDivision(dimensions, xDivision, yDivision, zDivision);
		bool* isFilled = new bool[xDivision * yDivision * zDivision];
		for (int i = 0; i < xDivision * yDivision * zDivision; i++) {
			isFilled[i] = true;
		}
		CreateVertexGrid(dimensions, xDivision, yDivision, zDivision);
		CreateIndices(xDivision, yDivision, zDivision, isFilled);

		delete[] isFilled;

		vao.Bind();
		vbo.Init(vertices);
		ebo.Init(indices);
		vao.LinkAttrib(vbo, 0, 3, GL_FLOAT, sizeof(glm::vec3), (void*)0);
		std::cout << "Bounding geometry is ready." << std::endl;
	}


	void BoundingGeometry::UpdateGeometry(const Texture3D& voxelTexture, const TransferFunction& transferFunction, float threshold, bool forceRestart)
	{
		static unsigned int step = 0;
		static bool isFinished = false;
		static bool* isFilled = nullptr;
		Dimensions dimensions = voxelTexture.GetDimensions();
		unsigned int xDivision, yDivision, zDivision;
		CalculateDivision(dimensions, xDivision, yDivision, zDivision);
		if (forceRestart) {
			step = 0;
			isFinished = false;
		}
		if (step == 0) {
			isFilled = new bool[xDivision * yDivision * zDivision];
		}
		if (isFinished) {
			return;
		}
		//std::cout << "Updating bounding geometry." << std::endl;
		this->threshold = threshold;
		vertices.clear();
		indices.clear();


		CalculateFilled(dimensions,
			xDivision,
			yDivision,
			zDivision,
			isFilled,
			voxelTexture,
			transferFunction,
			step);

		if (step >= dimensions.depth) {
			isFinished = true;
			flatColor.a = 0.0f;
			flatColor = normalize(flatColor);
			flatColor.a = 1.0f;
			CreateVertexGrid(dimensions, xDivision, yDivision, zDivision);
			CreateIndices(xDivision, yDivision, zDivision, isFilled);
			delete[] isFilled;
			vao.Bind();
			vbo.Init(vertices);
			ebo.Init(indices);
			vao.LinkAttrib(vbo, 0, 3, GL_FLOAT, sizeof(glm::vec3), (void*)0);
			std::cout << "Bounding geometry is ready." << std::endl;
		}
		else {
			step += BOX_STEP_SIZE;
		}
	}


	void BoundingGeometry::RenderFrontAndBack(
		const Camera& camera, 
		const glm::mat4& modelMatrix, 
		const glm::mat4& invModelMatrix, 
		const glm::mat4& lightViewProjMatrix,
		const glm::vec4& w_lightPos
	)
	{
		modelPosShader.Activate();
		vao.Bind();
		modelPosShader.SetUniform("sceneObject.modelMatrix", modelMatrix);

		glm::vec4 m_cameraPos = invModelMatrix * glm::vec4(camera.GetPosition(), 1.0f);
		m_cameraPos /= m_cameraPos.w;

		// Back face:
		exitFBO.Bind();
		modelPosShader.SetUniform("camera.viewProjMatrix", camera.GetViewProjMatrix());
		glClearColor(m_cameraPos.x, m_cameraPos.y, m_cameraPos.z, 1.0);
		glClearDepth(0);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glDisable(GL_CULL_FACE);
		glDisable(GL_BLEND);
		glEnable(GL_DEPTH_TEST);
		glDepthMask(GL_TRUE);
		glDepthFunc(GL_GREATER);
		glDrawElements(GL_TRIANGLES, (GLsizei)indices.size(), GL_UNSIGNED_INT, 0);

		// Front face:
		enterFBO.Bind();
		glClearColor(m_cameraPos.x, m_cameraPos.y, m_cameraPos.z, 1.0);
		glClear(GL_COLOR_BUFFER_BIT);
		glDisable(GL_CULL_FACE);
		glEnable(GL_DEPTH_TEST);
		glDepthFunc(GL_LESS);
		glDrawElements(GL_TRIANGLES, (GLsizei)indices.size(), GL_UNSIGNED_INT, 0);

		// From light's POV:
		lightEnterFBO.Bind();
		modelPosShader.SetUniform("camera.viewProjMatrix", lightViewProjMatrix);
		glm::vec4 m_lightPos = invModelMatrix * w_lightPos;
		m_lightPos /= m_lightPos.w;
		glClearColor(m_lightPos.x, m_lightPos.y, m_lightPos.z, 1.0);
		glClearDepth(1);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glDisable(GL_CULL_FACE);
		glEnable(GL_DEPTH_TEST);
		glDepthFunc(GL_LESS);
		glDrawElements(GL_TRIANGLES, (GLsizei)indices.size(), GL_UNSIGNED_INT, 0);
		vao.Unbind();
		FBO::BindDefault();
	}


	void BoundingGeometry::DrawOnScreen(const FBO& fbo, const Camera& camera, const glm::mat4& modelMatrix, const glm::mat4& invModelMatrix, float opacity)
	{
		fbo.Bind();
		flatColorShader.Activate();
		vao.Bind();
		glEnable(GL_DEPTH_TEST);
		glDepthFunc(GL_LESS);
		glDepthMask(GL_FALSE);
		glDisable(GL_CULL_FACE);
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

		flatColorShader.SetUniform("camera.viewProjMatrix", camera.GetViewProjMatrix());
		flatColorShader.SetUniform("sceneObject.modelMatrix", modelMatrix);
		flatColorShader.SetUniform("color", glm::vec4(flatColor.r, flatColor.g, flatColor.b, opacity));
		glm::vec4 m_cameraPos = invModelMatrix * glm::vec4(camera.GetPosition(), 1.0f);
		m_cameraPos /= m_cameraPos.w;
		flatColorShader.SetUniform("modelEye", m_cameraPos);

		glDrawElements(GL_TRIANGLES, (GLsizei)indices.size(), GL_UNSIGNED_INT, 0);
		glDepthMask(GL_TRUE);
		vao.Unbind();
		fbo.Unbind();
	}

	void BoundingGeometry::BindTextures() {
		frontFaceTexture.Bind();
		backFaceTexture.Bind();
		lightFaceTexture.Bind();
	}
}
