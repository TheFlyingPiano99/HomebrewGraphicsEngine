#pragma once
#include "FBO.h"
#include "Mesh.h"
#include "AssetFolderPathManager.h"
#include "GeometryFactory.h"
#include "UniformVariable.h"


namespace Hogra {

	class PostProcessStage
	{
	public:

		PostProcessStage() = default;

		void Init(std::string& fragmentShaderPath, int contextWidth, int contextHeight);

		virtual ~PostProcessStage() {
			fbo.Delete();
		}

		virtual void Bind();
		virtual void Draw(const FBO& outFBO, const Texture2D& depthTexture, const Camera& camera);
		virtual FBO& GetFBO();

		virtual void OnContextResize(unsigned int contextWidth, unsigned int contextHeight);

		void SetActive(bool _active);

		const Texture2D& GetColorTexture() const;

		void AddUniformVariable(AbstractUniformVariable* variable);

	protected:
		FBO fbo;
		Mesh* mesh = nullptr;
		Texture2D colorTexture;
		ShaderProgram program;
		Material* volumeMaterial;
		bool active = true;
		std::vector<AbstractUniformVariable*> uniformVariables;
	};

}