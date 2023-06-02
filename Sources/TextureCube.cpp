#include "TextureCube.h"
#include "MemoryManager.h"
#include "DebugUtils.h"
#include "GeometryFactory.h"
#include "FBO.h"
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/rotate_vector.hpp>


namespace Hogra {
	
	void TextureCube::Init(std::vector<std::filesystem::path>& images, GLuint _unit, GLuint _pixelType)
	{
		this->unit = _unit;
		this->clientDataType = _pixelType;


		glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);
		glGenTextures(1, &glID);
		glActiveTexture(GL_TEXTURE0 + unit);
		glBindTexture(GL_TEXTURE_CUBE_MAP, glID);
		glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);

		// Stores the width, height, and the number of color channels of the image
		int widthImg, heightImg, numColCh;
		// Flips the image so it appears right side up
		//stbi_set_flip_vertically_on_load(true);

		stbi_set_flip_vertically_on_load(false);
		for (int i = 0; i < 6; i++) {
			unsigned char* imgBytes = stbi_load(images[i].string().c_str(), &widthImg, &heightImg, &numColCh, 0);
			if (imgBytes == nullptr) {
				DebugUtils::PrintError("TextureCube", std::string("Failed loading cube texture side:\n\"").append(images[i].string()).append("\"").c_str());
				throw std::exception();
			}
			this->internalFormat = GL_RGBA;
			this->clientDataFormat = GL_RGBA;
			switch (numColCh)
			{
				case 4: {internalFormat = (GL_FLOAT == clientDataType)? GL_RGBA16F : GL_RGBA; clientDataFormat = GL_RGBA; break; }
				case 3: {internalFormat = (GL_FLOAT == clientDataType) ? GL_RGB16F : GL_RGB; clientDataFormat = GL_RGB; break; }
				case 2: {internalFormat = (GL_FLOAT == clientDataType) ? GL_RG16F : GL_RG; clientDataFormat = GL_RG; break; }
				case 1: {internalFormat = (GL_FLOAT == clientDataType) ? GL_R16F : GL_R; clientDataFormat = GL_R; break; }
				default: break;
			}
			glTexImage2D(
				GL_TEXTURE_CUBE_MAP_POSITIVE_X + i,
				0, internalFormat, widthImg, heightImg, 0, clientDataFormat, clientDataType, imgBytes);
			stbi_image_free(imgBytes);
		}

		this->dimensions.x = widthImg;
		this->dimensions.y = heightImg;

		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAX_LEVEL, 8);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_BASE_LEVEL, 0);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
		glGenerateMipmap(GL_TEXTURE_CUBE_MAP);
		glBindTexture(GL_TEXTURE_CUBE_MAP, 0);

	}

	void TextureCube::Init(unsigned int resolution, GLuint _unit, GLenum _internalFormat, GLenum _clientDataFormat, GLenum _clientDataType, bool useLinearFiltering)
	{
		this->unit = _unit;
		this->clientDataFormat = _clientDataFormat;
		this->clientDataType = _clientDataType;
		this->internalFormat = _internalFormat;

		glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);
		glGenTextures(1, &glID);
		glActiveTexture(GL_TEXTURE0 + unit);
		glBindTexture(GL_TEXTURE_CUBE_MAP, glID);

		this->dimensions.x = resolution;
		this->dimensions.y = resolution;
		
		for (unsigned int i = 0; i < 6; ++i) {
			glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, internalFormat,
				resolution, resolution, 0, clientDataFormat, clientDataType, NULL);
		}
		if (useLinearFiltering) {
			glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		}
		else {
			glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
			glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		}
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
		glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
	}

	void TextureCube::InitFromEquirectangular(const Texture2D& equirectangularMap, unsigned int _unit, GLenum _internalFormat, GLenum _clientDataFormat, GLenum _clientDataType)
	{
		unsigned int resolution = 1024;
		this->dimensions = { resolution , resolution };
		this->unit = _unit;
		this->internalFormat = _internalFormat;
		this->clientDataFormat = _clientDataFormat;
		this->clientDataType = _clientDataType;

		equirectangularMap.GenerateMipmap();

		ShaderProgram equirectangularToCubemapShader;
		equirectangularToCubemapShader.Init(
			AssetFolderPathManager::getInstance()->getShaderFolderPath().append("Utility/EnvironmentMap/equirectangularToCubemap.vert"),
			"",
			AssetFolderPathManager::getInstance()->getShaderFolderPath().append("Utility/EnvironmentMap/equirectangularToCubemap.frag")
			);
		FBO captureFBO;
		captureFBO.Init();
		captureFBO.SetViewport(0, 0, resolution, resolution);
		this->Init(resolution, unit, internalFormat, clientDataFormat, clientDataType, true);	// Init empty cubemap
		auto cubeGeometry = GeometryFactory::GetInstance()->GetSimple1X1Cube();
		cubeGeometry->SetFaceCulling(false);
		glm::mat4 captureProjection = glm::perspective(glm::radians(90.0f), 1.0f, 0.1f, 10.0f);
		glm::mat4 captureViews[] =
		{
		   glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(1.0f,  0.0f,  0.0f), glm::vec3(0.0f, -1.0f,  0.0f)),
		   glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(-1.0f,  0.0f,  0.0f), glm::vec3(0.0f, -1.0f,  0.0f)),
		   glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f,  1.0f,  0.0f), glm::vec3(0.0f,  0.0f,  1.0f)),
		   glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f,  0.0f), glm::vec3(0.0f,  0.0f, -1.0f)),
		   glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f,  0.0f,  1.0f), glm::vec3(0.0f, -1.0f,  0.0f)),
		   glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f,  0.0f, -1.0f), glm::vec3(0.0f, -1.0f,  0.0f))
		};

		// convert HDR equirectangular environment map to cubemap equivalent
		equirectangularToCubemapShader.Activate();
		equirectangularToCubemapShader.SetUniform("equirectangularMap", &equirectangularMap);
		equirectangularToCubemapShader.SetUniform("projection", captureProjection);
		equirectangularMap.Bind();
		
		captureFBO.Bind();
		for (unsigned int i = 0; i < 6; i++)
		{
			equirectangularToCubemapShader.SetUniform("view", captureViews[i]);
			//captureFBO.LinkTexture(GL_COLOR_ATTACHMENT0, *this);
			
			glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
				GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, this->glID, 0);
			
			glClearColor(0.5, 0, 1, 1);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
			
			cubeGeometry->Draw(); // renders a 1x1 cube
		}
		Allocator::Delete(cubeGeometry);
		captureFBO.Unbind();
		
		/*
		this->Bind();
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glGenerateMipmap(GL_TEXTURE_CUBE_MAP);
		this->Unbind();
		*/
	}

	void TextureCube::InitFromCubeMap(
		const TextureCube& cubemap, 
		ShaderProgram& conversionShader, 
		unsigned int resolution, 
		unsigned int _unit, 
		GLuint _format, 
		GLuint _pixelType,
		unsigned int maxMipLevels
	)
	{
		this->unit = _unit;
		this->clientDataFormat = _format;
		this->clientDataType = _pixelType;
		this->internalFormat = cubemap.internalFormat;

		this->Init(resolution, unit, internalFormat, clientDataFormat, clientDataType, true);	// Init empty cubemap
		if (1 < maxMipLevels) {
			this->Bind();
			glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
			glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			glGenerateMipmap(GL_TEXTURE_CUBE_MAP);
			this->Unbind();
		}
		auto cubeGeometry = GeometryFactory::GetInstance()->GetSimple1X1Cube();
		cubeGeometry->SetFaceCulling(false);
		glm::mat4 captureProjection = glm::perspective(glm::radians(90.0f), 1.0f, 0.1f, 10.0f);
		glm::mat4 captureViews[] =
		{
		   glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(1.0f,  0.0f,  0.0f), glm::vec3(0.0f, -1.0f,  0.0f)),
		   glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(-1.0f,  0.0f,  0.0f), glm::vec3(0.0f, -1.0f,  0.0f)),
		   glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f,  1.0f,  0.0f), glm::vec3(0.0f,  0.0f,  1.0f)),
		   glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f,  0.0f), glm::vec3(0.0f,  0.0f, -1.0f)),
		   glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f,  0.0f,  1.0f), glm::vec3(0.0f, -1.0f,  0.0f)),
		   glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f,  0.0f, -1.0f), glm::vec3(0.0f, -1.0f,  0.0f))
		};

		FBO captureFBO;
		captureFBO.Init();
		RBO depthRBP;
		depthRBP.Init(GL_DEPTH_COMPONENT, resolution, resolution);
		captureFBO.LinkRBO(GL_DEPTH_ATTACHMENT, depthRBP);
		
		// convert HDR equirectangular environment map to cubemap equivalent
		conversionShader.Activate();
		conversionShader.SetUniform("projection", captureProjection);
		cubemap.Bind();
		captureFBO.Bind();
		glDisable(GL_DEPTH_TEST);
		for (unsigned int mip = 0; mip < maxMipLevels; mip++) {
			for (int i = -1; i < 6; i++)
			{
				int side = (0 > i)? 0 : i;
				// reisze framebuffer according to mip-level size.
				unsigned int mipWidth = resolution * std::pow(0.5, mip);
				unsigned int mipHeight = resolution * std::pow(0.5, mip);
				captureFBO.SetViewport(0, 0, mipWidth, mipHeight);
				conversionShader.SetUniform("view", captureViews[side]);
				float roughness = (float)mip / (float)(maxMipLevels - 1);
				conversionShader.SetUniform("roughness", roughness);

				glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
					GL_TEXTURE_CUBE_MAP_POSITIVE_X + side, this->glID, mip);

				glClearColor(0.5, 0, 1, 1);
				glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

				cubeGeometry->Draw(); // renders a 1x1 cube
				captureFBO.Bind();
			}
		}

		Allocator::Delete(cubeGeometry);
		captureFBO.Unbind();
	}

	void TextureCube::Bind()
	{
		glActiveTexture(GL_TEXTURE0 + unit);
		glBindTexture(GL_TEXTURE_CUBE_MAP, glID);
	}

	void TextureCube::Unbind()
	{
		glBindTexture(GL_TEXTURE_2D, 0);
	}

	void TextureCube::Delete()
	{
		glDeleteTextures(1, &glID);
	}

	void TextureCube::Bind() const
	{
		glActiveTexture(GL_TEXTURE0 + unit);
		glBindTexture(GL_TEXTURE_CUBE_MAP, glID);
	}

	void TextureCube::Unbind() const
	{
		glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
	}

	void TextureCube::SetFiltering(GLenum filtering) const
	{
		Bind();
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, filtering);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, filtering);
		Unbind();
	}
	Texture2D TextureCube::GetFace(unsigned int face)
	{
		Texture2D texture;
		// TODO
		return texture;
	}

	void TextureCube::WriteData(void* dataPtr)
	{
		DebugUtils::PrintError("TextureCube", "Unimplemented function: WriteData");
	}

	void TextureCube::ReadData(void* dataPtr) const
	{
		DebugUtils::PrintError("TextureCube", "Unimplemented function: ReadData");
	}
}