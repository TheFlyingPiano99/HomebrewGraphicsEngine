#pragma once
#include "FBO.h"
#include "Mesh.h"
#include "AssetFolderPathManager.h"
#include "GeometryFactories.h"

class PostProcessStage
{
public:
	PostProcessStage(std::string& fragmentShaderPath, int contextWidth, int contextHeight) {
		colorTexture = new Texture2D(GL_RGBA, glm::ivec2(contextWidth, contextHeight), 0, GL_RGBA, GL_FLOAT);
		depthTexture = new Texture2D(GL_DEPTH_COMPONENT, glm::ivec2(contextWidth, contextHeight), 1, GL_DEPTH_COMPONENT, GL_FLOAT);
		program = new ShaderProgram(
			AssetFolderPathManager::getInstance()->getShaderFolderPath().append("fullscreenQuad.vert"),
			fragmentShaderPath
		);
		material = new Material(program);
		material->addTexture(colorTexture);
		material->addTexture(depthTexture);
		mesh = new Mesh(material, GeometryFactory::FullScreenQuad::getInstance());
		fbo.LinkTexture(GL_COLOR_ATTACHMENT0, *colorTexture, 0);
		fbo.LinkTexture(GL_DEPTH_ATTACHMENT, *depthTexture, 0);
		//RBO stencilRBO(GL_STENCIL_COMPONENTS, contextWidth, contextHeight);
		//fbo.LinkRBO(GL_STENCIL_ATTACHMENT, stencilRBO);
	}

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

