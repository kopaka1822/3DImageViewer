#pragma once

#include "gl.hpp"
#include "format.hpp"

namespace gpupro {

	// A texture. Can also be used as render-target and image (random
	// read write access).
	// There are features not covered by this class:
	//	* no multi-sampling
	//	* no compressed formats
	//	* sub-rectangle updates
	class Texture
	{
	public:
		enum class Layout
		{
			TEX_1D = GL_TEXTURE_1D,
			TEX_2D = GL_TEXTURE_2D,
			TEX_3D = GL_TEXTURE_3D,
			CUBE_MAP = GL_TEXTURE_CUBE_MAP,
			TEX_2D_ARRAY = GL_TEXTURE_2D_ARRAY,
			CUBE_MAP_ARRAY = GL_TEXTURE_CUBE_MAP_ARRAY,
		};

		enum class ImageAccess
		{
			READ_ONLY = GL_READ_ONLY,
			WRITE_ONLY = GL_WRITE_ONLY,
			READ_WRITE = GL_READ_WRITE
		};

		// Create a texture without memory. Use this only in conjunction with load(),
		// if you don't know the size of the image.
		Texture(Layout _layout, InternalFormat _format);

		// Create a 2D texture directly from file (including mip-maps).
		// This method is equivalent to
		//		Texture(Layout::TEX_2D, _format);
		//		load(_fileName);
		// This is simply a shortcut for the most common case.
		Texture(InternalFormat _format, const char* _fileName);

		// Create a 1D texture or cube map
		// _layout: must be TEX_1D, TEX_2D, TEX_3D or CUBE_MAP
		// _numMipLevels: number of mip-map levels to allocate. The default 0
		//		causes an auto-detection of the maximum number.
		Texture(Layout _layout, GLsizei _width, InternalFormat _format, GLsizei _numMipLevels = 0);
		// Create a 2D texture
		// _layout: must be TEX_2D or ARRAY_CUBE_MAP
		Texture(Layout _layout, GLsizei _width, GLsizei _height, InternalFormat _format, GLsizei _numMipLevels = 0);
		// Create a 3D texture
		// _layout: must be TEX_3D or ARRAY_2D
		Texture(Layout _layout, GLsizei _width, GLsizei _height, GLsizei _depth, InternalFormat _format, GLsizei _numMipLevels = 0);

		~Texture();
		// Move but not copy-able
		Texture(Texture&& _rhs);
		Texture(const Texture&) = delete;
		Texture& operator = (Texture&& _rhs);
		Texture& operator = (const Texture& _rhs) = delete;

		// Load a texture from file into a single array layer / cubemap face.
		// For non arrays the _layer parameter is ignored.
		void load(const char* _fileName, GLuint _layer = 0, bool _generateMipMaps = true);

		// _mipLevel: The mipmap to be filled (usually 0).
		// _layer: For CUBE_MAP, ARRAY_2D and ARRAY_CUBE_MAP only, this gives
		//		the array layer+face. In case of cube map arrays there are 6 layers
		//		per cube map! I.e. indices 0 to 5 are the faces of cube map 0.
		//		Use 0 for other texture layouts.
		void setData(GLuint _mipLevel, GLuint _layer, SetDataFormat _format, SetDataType _type, const void* _data);

		// Bind as sampled texture
		void bindAsTexture(GLuint _bindingIndex);

		GLsizei width() const { return m_size[0]; }
		GLsizei height() const { return m_size[1]; }
		GLsizei depth() const { return m_size[2]; }
		GLuint numMipLevels() const { return m_numMipLevels; }
		InternalFormat format() const { return m_format; }

		GLuint glID() { return m_id; }
	private:
		GLuint m_id;
		GLsizei m_size[3];
		Layout m_layout;
		InternalFormat m_format;
		GLuint m_numMipLevels;

		// Helper to allocate the texture memory from layout,
		// format and size parameters.
		void allocateMemory();
	};

} // namespace gpupro