/*
* ~~~~~~~~~~~~~~~~~~~~~~~~~
* Homebrew Graphics Engine
* Zoltan Simon 2023
* Repository: https://github.com/TheFlyingPiano99/HomebrewGraphicsEngine
* ~~~~~~~~~~~~~~~~~~~~~~~~~
*/
#pragma once
#include <glad/glad.h>
#include <vector>


namespace Hogra {

    enum class ShaderStorageDataAccess {
        READ_ONLY = 0,
        WRITE_ONLY = 1,
        READ_AND_WRITE = 2,
        ERROR = 3
    };


    template<typename T>
    class ShaderStorageBuffer;


    template<typename T>
    class ShaderStorageData {
        friend class ShaderStorageBuffer<T>;
        T* data = nullptr;
        size_t size = 0;
        ShaderStorageDataAccess access = ShaderStorageDataAccess::ERROR;
        ShaderStorageBuffer<T>* buffer = nullptr;
    public:

        ~ShaderStorageData() {
            if (nullptr != buffer) {
                buffer->isAccessed = false;
                buffer->Bind();
                glUnmapBuffer(GL_SHADER_STORAGE_BUFFER);
                buffer->Unbind();
            }
        }

        T* GetData() const {
            return data;
        }

        size_t GetSize() const {
            return size;
        }

        ShaderStorageDataAccess GetAccess() const {
            return access;
        }
    };


    template<typename T>
    class ShaderStorageBuffer
    {
        friend class ShaderStorageData<T>;
    public:
        void Init(size_t data_count);

        void Bind();

        void Unbind();

        void WriteData(const std::vector<T>& data);

        ShaderStorageData<T> GetDataReadOnly();

    private:
        GLuint glID = 0;
        bool isAccessed = false;
        size_t m_data_count = 0;
    };

}
