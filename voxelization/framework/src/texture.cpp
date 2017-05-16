#include "texture.hpp"

#include <iostream>
#include <algorithm>

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

static GLsizei computeCorrectedMipmapLevels(GLsizei _numMipLevels, GLsizei _maxResolution)
{
	GLsizei maxMip = 1;
	while((_maxResolution /= 2) > 0) ++maxMip;
	if(_numMipLevels == 0)
		return maxMip;
	else
		return std::min(maxMip, _numMipLevels);
}

gpupro::Texture::Texture(Layout _layout, InternalFormat _format) :
	m_id(0),
	m_layout(_layout),
	m_format(_format)
{
	m_size[0] = m_size[1] = m_size[2] = 0;
}

gpupro::Texture::Texture(InternalFormat _format, const char* _fileName) :
	Texture(Layout::TEX_2D, _format)
{
	load(_fileName);
}

gpupro::Texture::Texture(Layout _layout, GLsizei _width, InternalFormat _format, GLsizei _numMipLevels) :
	m_layout(_layout),
	m_format(_format)
{
	glGenTextures(1, &m_id);

	m_numMipLevels = computeCorrectedMipmapLevels(_numMipLevels, _width);

	switch(_layout)
	{
	case Layout::TEX_1D:
		m_size[0] = _width;
		m_size[1] = m_size[2] = 1;
		break;
	case Layout::TEX_2D:
	case Layout::CUBE_MAP:
		m_size[0] = m_size[1] = _width;
		m_size[2] = 1;
		break;
	case Layout::TEX_3D:
		m_size[0] = m_size[1] = m_size[2] = _width;
		break;
	default:
		std::cerr << "ERR: Invalid layout for single parameter texture creation!";
		return;
	}

	allocateMemory();
}

gpupro::Texture::Texture(Layout _layout, GLsizei _width, GLsizei _height, InternalFormat _format, GLsizei _numMipLevels) :
	m_layout(_layout),
	m_format(_format)
{
	glGenTextures(1, &m_id);

	switch(_layout)
	{
	case Layout::TEX_2D:
		m_size[0] = _width;
		m_size[1] = _height;
		m_size[2] = 1;
		m_numMipLevels = computeCorrectedMipmapLevels(_numMipLevels, std::max(_width, _height));
		break;
	case Layout::CUBE_MAP_ARRAY:
		m_size[0] = m_size[1] = _width;
		m_size[2] = _height;
		m_numMipLevels = computeCorrectedMipmapLevels(_numMipLevels, _width);
		break;
	default:
		std::cerr << "ERR: Invalid layout for two parameter texture creation!";
		return;
	}

	allocateMemory();
}

gpupro::Texture::Texture(Layout _layout, GLsizei _width, GLsizei _height, GLsizei _depth, InternalFormat _format, GLsizei _numMipLevels) :
	m_layout(_layout),
	m_format(_format)
{
	glGenTextures(1, &m_id);

	switch(_layout)
	{
	case Layout::TEX_3D:
	case Layout::TEX_2D_ARRAY:
		m_size[0] = _width;
		m_size[1] = _height;
		m_size[2] = _depth;
		m_numMipLevels = computeCorrectedMipmapLevels(_numMipLevels, std::max(std::max(_width, _height), _depth));
		break;
	default:
		std::cerr << "ERR: Invalid layout for two parameter texture creation!";
	}

	allocateMemory();
}

gpupro::Texture::~Texture()
{
	glDeleteTextures(1, &m_id);
}

gpupro::Texture::Texture(Texture&& _rhs) :
	m_id(_rhs.m_id),
	m_layout(_rhs.m_layout),
	m_format(_rhs.m_format),
	m_numMipLevels(_rhs.m_numMipLevels)
{
	for(int i = 0; i < 3; ++i) m_size[i] = _rhs.m_size[i];
	_rhs.m_id = 0;
}

gpupro::Texture& gpupro::Texture::operator=(Texture&& _rhs)
{
	glDeleteTextures(1, &m_id);

	m_id = _rhs.m_id;
	m_layout = _rhs.m_layout;
	m_format = _rhs.m_format;
	m_numMipLevels = _rhs.m_numMipLevels;
	for(int i = 0; i < 3; ++i) m_size[i] = _rhs.m_size[i];
	_rhs.m_id = 0;
	return *this;
}


void gpupro::Texture::load(const char* _fileName, GLuint _layer, bool _generateMipMaps)
{
	int width = -1;
	int height = -1;
	int numComps = -1;
	stbi_uc* textureData = stbi_load(_fileName, &width, &height, &numComps, 4);
	if(!textureData)
	{
		std::cerr << "ERR: Cannot load texture: " << _fileName << '\n';
		return;
	}

	if(width != m_size[0] || height != m_size[1])
	{
		if((m_layout == Layout::CUBE_MAP_ARRAY || m_layout == Layout::TEX_2D_ARRAY) && _layer != 0)
		{
			std::cerr << "ERR: All textures in an array must have the same size!\n";
			return;
		}

		// Reallocate memory
		if(m_id) {
			// The texture already has a memory, which is immutable -> create
			// entire new texture.
			*this = std::move(Texture(m_layout, width, height, m_format, _generateMipMaps ? 0 : 1));
		} else {
			// Texture was created without allocation
			glGenTextures(1, &m_id);
			m_size[0] = width;
			m_size[1] = height;
			m_size[2] = 1;
			m_numMipLevels = computeCorrectedMipmapLevels(_generateMipMaps ? 0 : 1, std::max(width, height));
			allocateMemory();
		}
	}

	setData(0, _layer, SetDataFormat::RGBA, isSignedFormat(m_format) ? SetDataType::INT8 : SetDataType::UINT8, textureData);
	stbi_image_free(textureData);

	if(_generateMipMaps) {
		if(m_layout != Layout::CUBE_MAP || _layer == 5)
		{
			glBindTexture(static_cast<GLenum>(m_layout), m_id);
			glGenerateMipmap(static_cast<GLenum>(m_layout));
		}
	}
}

void gpupro::Texture::setData(GLuint _mipLevel, GLuint _layer, SetDataFormat _format, SetDataType _type, const void* _data)
{
	glBindTexture(static_cast<GLenum>(m_layout), m_id);
	switch(m_layout)
	{
	case Layout::TEX_1D:
		glTexSubImage1D(static_cast<GLenum>(m_layout), _mipLevel, 0, m_size[0], static_cast<GLenum>(_format), static_cast<GLenum>(_type), _data);
		break;
	case Layout::TEX_2D:
		glTexSubImage2D(static_cast<GLenum>(m_layout), _mipLevel, 0, 0, m_size[0], m_size[1], static_cast<GLenum>(_format), static_cast<GLenum>(_type), _data);
		break;
	case Layout::TEX_3D:
		glTexSubImage3D(static_cast<GLenum>(m_layout), _mipLevel, 0, 0, 0, m_size[0], m_size[1], m_size[2], static_cast<GLenum>(_format), static_cast<GLenum>(_type), _data);
		break;
	case Layout::CUBE_MAP:
		glTexSubImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + _layer, _mipLevel, 0, 0, m_size[0], m_size[1], static_cast<GLenum>(_format), static_cast<GLenum>(_type), _data);
		break;
	case Layout::TEX_2D_ARRAY:
	case Layout::CUBE_MAP_ARRAY:
		glTexSubImage3D(static_cast<GLenum>(m_layout), _mipLevel, 0, 0, _layer, m_size[0], m_size[1], 1, static_cast<GLenum>(_format), static_cast<GLenum>(_type), _data);
		break;
	}
}

void gpupro::Texture::bindAsTexture(GLuint _bindingIndex)
{
	glActiveTexture(GL_TEXTURE0 + _bindingIndex);
	glBindTexture(static_cast<GLenum>(m_layout), m_id);
}

void gpupro::Texture::allocateMemory()
{
	glBindTexture(static_cast<GLenum>(m_layout), m_id);

	switch(m_layout)
	{
	case Layout::TEX_1D:
		glTexStorage1D(GL_TEXTURE_1D, m_numMipLevels, static_cast<GLenum>(m_format), m_size[0]);
		break;
	case Layout::TEX_2D:
		glTexStorage2D(GL_TEXTURE_2D, m_numMipLevels, static_cast<GLenum>(m_format), m_size[0], m_size[1]);
		break;
	case Layout::TEX_3D:
		glTexStorage3D(GL_TEXTURE_3D, m_numMipLevels, static_cast<GLenum>(m_format), m_size[0], m_size[1], m_size[2]);
		break;
	case Layout::CUBE_MAP:
		glTexStorage2D(GL_TEXTURE_CUBE_MAP, m_numMipLevels, static_cast<GLenum>(m_format), m_size[0], m_size[1]);
		break;
	case Layout::TEX_2D_ARRAY:
		glTexStorage3D(GL_TEXTURE_2D_ARRAY, m_numMipLevels, static_cast<GLenum>(m_format), m_size[0], m_size[1], m_size[2]);
		break;
	case Layout::CUBE_MAP_ARRAY:
		glTexStorage3D(GL_TEXTURE_CUBE_MAP_ARRAY, m_numMipLevels, static_cast<GLenum>(m_format), m_size[0], m_size[1], m_size[2] * 6);
		break;
	}
}
