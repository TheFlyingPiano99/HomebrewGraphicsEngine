#pragma once
#include "FBO.h"
#include "Mesh.h"
#include "AssetFolderPathManager.h"
#include "GeometryFactory.h"

namespace hograengine {

	class PostProcessStage
	{
	public:
		PostProcessStage(std::string& fragmentShaderPath, int contextWidth, int contextHeight);

		~PostProcessStage() {
			fbo.Delete();
			delete mesh;
			delete colorTexture;
			delete depthTexture;
			delete program;
			delete material;
		}

		void Bind() const;
		void Draw(const FBO& fbo) const;
		const FBO& getFBO() const;

		void resize(int contextWidth, int contextHeight);

		void setActive(bool _active);

	private:
		FBO fbo;
		Mesh* mesh;
		Texture2D* colorTexture;
		Texture2D* depthTexture;
		ShaderProgram* program;
		Material* material;
		bool active = true;
	};

}