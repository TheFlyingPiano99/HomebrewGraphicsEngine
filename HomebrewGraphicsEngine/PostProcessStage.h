#pragma once
#include "FBO.h"
#include "Mesh.h"
#include "AssetFolderPathManager.h"
#include "GeometryFactory.h"

namespace Hogra {

	class PostProcessStage
	{
	public:
		PostProcessStage(std::string& fragmentShaderPath, int contextWidth, int contextHeight);

		~PostProcessStage() {
			fbo.Delete();
			Mesh::Deallocate(mesh);
			Material::Deallocate(material);
		}

		void Bind() const;
		void Draw(const FBO& fbo) const;
		const FBO& getFBO() const;

		void onResize(int contextWidth, int contextHeight);

		void setActive(bool _active);

		const Texture2D& getColorTexture() const {
			return colorTexture;
		}

	private:
		FBO fbo;
		Mesh* mesh = nullptr;
		Texture2D colorTexture;
		Texture2D depthTexture;
		ShaderProgram program;
		Material* material;
		bool active = true;
	};

}