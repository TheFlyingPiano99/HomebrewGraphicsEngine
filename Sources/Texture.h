#pragma once
#include "glad/glad.h"
#include <memory>
#include <vector>
#include "Identifiable.h"

namespace Hogra {

	class Texture : public Identifiable
	{
	public:
		GLuint glID;
		GLuint unit;

		virtual ~Texture() = default;

		virtual void Bind() const = 0;

		virtual void Unbind() const = 0;
		
		virtual void SetFiltering(GLenum filtering) const = 0;

		void Delete() const;


	protected:
		//inline void* operator new(std::size_t size) { return ::operator new(size); }

		Texture() = default;
	};

}