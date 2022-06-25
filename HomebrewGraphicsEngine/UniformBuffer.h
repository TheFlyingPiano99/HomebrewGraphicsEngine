#pragma once
#include<glm/glm.hpp>
#include<glad/glad.h>
#include <vector>

namespace hograengine {
	class UniformBufferObject
	{
	public:
		UniformBufferObject(const std::vector<int>& subDataSizes, int binding);
		~UniformBufferObject();

		void Bind();

		void Unbind();

		void Delete();

		void uploadSubData(const void* subDataPtr, int index);

		struct MemoryAlignment {
			int baseAlignment;
			int alignedOffset;
		};

	private:
		GLuint ID;
		int binding;
		int memorySize;
		std::vector<MemoryAlignment> memoryAlignments;
		void calculateAlignment(const std::vector<int>& subDataSizes);
	};
}

