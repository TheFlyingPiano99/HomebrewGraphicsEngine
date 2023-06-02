#include "Texture.h"
#include "glad/glad.h"
#include "DebugUtils.h"

namespace Hogra {
	void Texture::BindToImageUnit() const
	{
		glBindImageTexture(unit, glID, 0, isLayered, 0, GL_READ_WRITE, internalFormat);
	}

	void Texture::Delete() const
	{
		glDeleteTextures(1, &glID);
	}
	unsigned int Texture::GetNumberOfClientSideChannels() const
	{
		int channels = 0;
		switch (clientDataFormat)
		{
		case GL_RGBA: {channels = 4; break; }
		case GL_RGB: {channels = 3; break; }
		case GL_RG: {channels = 2; break; }
		case GL_R: {channels = 1; break; }
		case GL_RED: {channels = 1; break; }
		case GL_GREEN: {channels = 1; break; }
		case GL_BLUE: {channels = 1; break; }
		case GL_DEPTH_COMPONENT: {channels = 1; break; }
		case GL_RGBA_INTEGER: {channels = 4; break; }
		case GL_RGB_INTEGER: {channels = 3; break; }
		case GL_RG_INTEGER: {channels = 2; break; }
		case GL_RED_INTEGER: {channels = 1; break; }
		case GL_GREEN_INTEGER: {channels = 1; break; }
		case GL_BLUE_INTEGER: {channels = 1; break; }
		}
		return channels;
	}
	unsigned int Texture::GetNumberOfClientSideBytesPerChannel() const
	{
		int bytesPerChannel = 0;
		switch (clientDataType)
		{
		case GL_DOUBLE: {bytesPerChannel = 8; break; }
		case GL_FLOAT: {bytesPerChannel = 4; break; }
		case GL_HALF_FLOAT: {bytesPerChannel = 2; break; }
		case GL_BYTE: {bytesPerChannel = 1; break; }
		case GL_UNSIGNED_BYTE: {bytesPerChannel = 1; break; }
		case GL_SHORT: {bytesPerChannel = 2; break; }
		case GL_UNSIGNED_SHORT: {bytesPerChannel = 2; break; }
		case GL_INT: {bytesPerChannel = 4; break; }
		case GL_UNSIGNED_INT: {bytesPerChannel = 4; break; }
		}
		return bytesPerChannel;
	}
}