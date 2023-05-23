#include "ShaderStorageBuffer.h"
#include "DebugUtils.h"


namespace Hogra {

	template<typename T>
	void ShaderStorageBuffer<T>::Init(size_t data_count)
	{
		m_data_count = data_count;
		glGenBuffers(1, &glID);
		glBindBuffer(GL_SHADER_STORAGE_BUFFER, glID);
		glBufferData(GL_SHADER_STORAGE_BUFFER, m_data_count * sizeof(T), nullptr, GL_STATIC_DRAW);
	}

	template<>
	void ShaderStorageBuffer<glm::vec4>::Init(size_t data_count)
	{
		m_data_count = data_count;
		glGenBuffers(1, &glID);
		glBindBuffer(GL_SHADER_STORAGE_BUFFER, glID);
		glBufferData(GL_SHADER_STORAGE_BUFFER, m_data_count * sizeof(glm::vec4), nullptr, GL_STATIC_DRAW);
	}


	template<typename T>
	void ShaderStorageBuffer<T>::Bind()
	{
		glBindBuffer(GL_SHADER_STORAGE_BUFFER, glID);
	}


	template<typename T>
	void ShaderStorageBuffer<T>::Unbind()
	{
		glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
	}


	template<typename T>
	void ShaderStorageBuffer<T>::WriteData(const std::vector<T>& data)
	{
		GLint bufMask = GL_MAP_WRITE_BIT | GL_MAP_INVALIDATE_BUFFER_BIT; // the invalidate makes a big difference when re-writing
		T* data = (T*)glMapBufferRange(GL_SHADER_STORAGE_BUFFER, 0, m_data_count * sizeof(T), bufMask);	// Access data
		int i = 0;
		for (auto item : data) {
			T[i] = *item;
			i++;
		}
		glUnmapBuffer(GL_SHADER_STORAGE_BUFFER);
	}


	template<>
	void ShaderStorageBuffer<glm::vec4>::WriteData(const std::vector<glm::vec4>& data)
	{
		GLint bufMask = GL_MAP_WRITE_BIT | GL_MAP_INVALIDATE_BUFFER_BIT; // the invalidate makes a big difference when re-writing
		glm::vec4* p_data = (glm::vec4*)glMapBufferRange(GL_SHADER_STORAGE_BUFFER, 0, m_data_count * sizeof(glm::vec4), bufMask);	// Access data
		int i = 0;
		for (auto item : data) {
			p_data[i] = item;
			i++;
		}
		glUnmapBuffer(GL_SHADER_STORAGE_BUFFER);
	}


	template<typename T>
	ShaderStorageData<T> ShaderStorageBuffer<T>::GetDataReadOnly()
	{
		if (isAccessed) {
			DebugUtils::PrintError(
				"ShaderStorageBuffer",
				"Trying to access buffer data while data is already accessed. Only one ShaderStorageData object can exist accessing a given storage buffer!"
			);
			return ShaderStorageData();
		}
		isAccessed = true;
		GLint bufMask = GL_MAP_READ_BIT;
		T* data = (T*)glMapBufferRange(GL_SHADER_STORAGE_BUFFER, 0, m_data_count * sizeof(T), bufMask);	// Access data
		ShaderStorageData dataObj;
		dataObj.data = data;
		dataObj.size = m_data_count;
		dataObj.access = ShaderStorageDataAccess.READ_ONLY;
		dataObj.buffer = this;
		return dataObj;
	}

}