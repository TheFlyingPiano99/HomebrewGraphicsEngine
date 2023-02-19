#pragma once

#include "TransferFunction.h"
#include "../VAO.h"
#include "../Camera.h"
#include "../Texture3D.h"
#include "../Texture2D.h"
#include "../PointLight.h"
#include "../EBO.h"
#include "../FBO.h"
#include "../ShaderProgram.h"

namespace Hogra::Volumetric {

	class BoundingGeometry
	{
	public:
		BoundingGeometry() = default;

		~BoundingGeometry() {
		}

		void Init();

		struct FullBox {};	// Tag

		void UpdateGeometry(FullBox _, const Texture3D& voxelTexture, const TransferFunction& transferFunction);

		void UpdateGeometry(const Texture3D& voxelTexture, const TransferFunction& transferFunction, float threshold, bool forceRestart);

		void RenderFrontAndBack(
			const Camera& camera, 
			const glm::mat4& modelMatrix, 
			const glm::mat4& invModelMatrix, 
			const glm::mat4& lightViewProjMatrix,
			const glm::vec4& lightPos
		);

		void DrawOnScreen(
			const FBO& fbo, 
			const Camera& camera, 
			const glm::mat4& modelMatrix, 
			const glm::mat4& invModelMatrix, 
			float opacity
		);

		void BindTextures();

	private:
		ShaderProgram modelPosShader;
		ShaderProgram flatColorShader;
		VAO vao;
		VBO vbo;
		EBO ebo;
		FBO enterFBO;
		FBO exitFBO;
		FBO lightEnterFBO;
		RBO depthRBO;
		Texture2D frontFaceTexture;
		Texture2D backFaceTexture;
		Texture2D lightFaceTexture;

		std::vector<glm::vec3> vertices;
		std::vector<GLuint> indices;
		float threshold;
		glm::vec4 flatColor;

		void AddCuboid(const glm::vec3& scale, const glm::vec3& translation);

		void CalculateDivision(const Dimensions& dimensions, unsigned int& xDivision, unsigned int& yDivision, unsigned int& zDivision);

		void CreateVertexGrid(const Dimensions& dimensions, const unsigned int& xDivision, const unsigned int& yDivision, const unsigned int& zDivision);

		void CalculateFilled(
			const Dimensions& dimensions,
			const unsigned int& xDivision,
			const unsigned int& yDivision,
			const unsigned int& zDivision,
			bool* isFilled,
			const Texture3D& voxelTexture,
			const TransferFunction& transferFunction,
			unsigned int step = 0
		);

		void CreateIndices(const unsigned int& xDivision, const unsigned int& yDivision, const unsigned int& zDivision, bool* isFilled);

		void AddPlusZSide(const int x, const int y, const int z, const unsigned int& xDivision, const unsigned int& yDivision, const unsigned int& zDivision);

		void AddMinusZSide(const int x, const int y, const int z, const unsigned int& xDivision, const unsigned int& yDivision, const unsigned int& zDivision);

		void AddPlusYSide(const int x, const int y, const int z, const unsigned int& xDivision, const unsigned int& yDivision, const unsigned int& zDivision);

		void AddMinusYSide(const int x, const int y, const int z, const unsigned int& xDivision, const unsigned int& yDivision, const unsigned int& zDivision);

		void AddPlusXSide(const int x, const int y, const int z, const unsigned int& xDivision, const unsigned int& yDivision, const unsigned int& zDivision);

		void AddMinusXSide(const int x, const int y, const int z, const unsigned int& xDivision, const unsigned int& yDivision, const unsigned int& zDivision);

		unsigned int IndexVertices(const int x, const int y, const int z, const unsigned int& xDivision, const unsigned int& yDivision, const unsigned int& zDivision);

		unsigned int IndexDivisionSized(const int x, const int y, const int z, const unsigned int& xDivision, const unsigned int& yDivision, const unsigned int& zDivision);
	};
}
