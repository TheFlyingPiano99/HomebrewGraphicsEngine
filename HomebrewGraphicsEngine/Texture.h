#pragma once
#include "glad/glad.h"

namespace hograengine {

	class Texture
	{
	public:
		GLuint ID;
		GLuint unit;

		virtual void Bind() const = 0;

		virtual void Unbind() const = 0;

		void Delete() const;

	protected:
		Texture() = default;
	};

}