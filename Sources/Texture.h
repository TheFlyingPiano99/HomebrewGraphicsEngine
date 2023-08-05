/*
* ~~~~~~~~~~~~~~~~~~~~~~~~~
* Homebrew Graphics Engine
* Zoltan Simon 2023
* Repository: https://github.com/TheFlyingPiano99/HomebrewGraphicsEngine
* ~~~~~~~~~~~~~~~~~~~~~~~~~
*/
#pragma once
#include "glad/glad.h"
#include <memory>
#include <vector>
#include "Identifiable.h"
#include "MemoryManager.h"

namespace Hogra {

	class Texture : public Identifiable
	{
		ALLOCATOR_CONSTRUCTIBLE
	public:
		GLuint glID = 0;
		GLuint unit = 0;
		GLenum internalFormat = GL_RGBA8;
		GLenum clientDataFormat = GL_RGBA;
		GLenum clientDataType = GL_UNSIGNED_BYTE;
		GLboolean isLayered = GL_FALSE;

		virtual ~Texture() = default;

		virtual void Bind() const = 0;

		virtual void Unbind() const = 0;
		
		virtual void BindToImageUnit() const;

		virtual void SetFiltering(GLenum filtering) const = 0;

		void Delete() const;

		virtual void WriteData(void* dataPtr) = 0;

		virtual void ReadData(void* dataPtr) const = 0;

		unsigned int GetNumberOfClientSideChannels() const;

		unsigned int GetNumberOfClientSideBytesPerChannel() const;

	protected:
		//inline void* operator new(std::size_t size) { return ::operator new(size); }

		Texture() = default;
	};

}