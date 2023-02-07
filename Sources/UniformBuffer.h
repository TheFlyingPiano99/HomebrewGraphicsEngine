#pragma once
#include<glm/glm.hpp>
#include<glad/glad.h>
#include <vector>

namespace Hogra {
	class UniformBufferObject
	{
	public:
		void Init(const std::vector<int>& subDataSizes, int binding);

		~UniformBufferObject();

		void Bind() const;

		void Unbind();

		void Delete();

		void UploadSubData(const void* subDataPtr, int index);

		struct MemoryAlignment {
			int baseAlignment;
			int alignedOffset;
		};

	private:
		GLuint glID;
		int binding;
		int memorySize;
		std::vector<MemoryAlignment> memoryAlignments;
		void CalculateAlignment(const std::vector<int>& subDataSizes);
	};
}

