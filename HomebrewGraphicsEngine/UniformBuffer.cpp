#include "UniformBuffer.h"

namespace hograengine {

	UniformBufferObject::UniformBufferObject(const std::vector<int>& subDataSizes, int _binding) : binding(_binding)
	{
		calculateAlignment(subDataSizes);
		glGenBuffers(1, &ID);
		glBindBuffer(GL_UNIFORM_BUFFER, ID);
		glBufferData(GL_UNIFORM_BUFFER, memorySize, nullptr, GL_STATIC_DRAW);	// Allocate GPU memory
		glBindBufferBase(GL_UNIFORM_BUFFER, binding, ID);						// Specify binding
		glBindBuffer(GL_UNIFORM_BUFFER, 0);
	}

	UniformBufferObject::~UniformBufferObject()
	{
		Delete();
	}

	void UniformBufferObject::Bind()
	{
		glBindBuffer(GL_UNIFORM_BUFFER, ID);
	}

	void UniformBufferObject::Unbind()
	{
		glBindBuffer(GL_UNIFORM_BUFFER, 0);
	}

	void UniformBufferObject::Delete()
	{
		glDeleteBuffers(1, &ID);
	}

	void UniformBufferObject::uploadSubData(const void* subDataPtr, int index)
	{
		glBufferSubData(GL_UNIFORM_BUFFER, memoryAlignments[index].alignedOffset, memoryAlignments[index].baseAlignment, subDataPtr);
	}

	void UniformBufferObject::calculateAlignment(const std::vector<int>& subDataSizes)
	{
		int currentOffset = 0;
		for (auto& subDataSize : subDataSizes) {
			MemoryAlignment alignment;
			alignment.baseAlignment = (subDataSize <= 4)? 4		// scalar
				: ((subDataSize <= 8) ? 8							// vec2
					: ((subDataSize / 16) + ((subDataSize % 16 == 0) ? 0 : 1)) * 16);	//vec3, vec4, or matrix
			alignment.alignedOffset = (currentOffset % alignment.baseAlignment == 0)? currentOffset : (currentOffset / alignment.baseAlignment + 1) * alignment.baseAlignment;
			currentOffset = alignment.alignedOffset + alignment.baseAlignment;
			memoryAlignments.push_back(alignment);
		}
		memorySize = currentOffset;
	}
}
