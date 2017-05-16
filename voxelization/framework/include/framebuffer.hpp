#pragma once

#include "texture.hpp"

#include <vector>

namespace gpupro {
	// A render buffer is an optimized render target. Unlike textures
	// it cannot be sampled later.
	// Use renderbuffers if you don't need access to the drawn data.
	// E.g. the depth buffer in a framebuffer.
	class Renderbuffer
	{
	public:
		Renderbuffer(GLsizei _width, GLsizei _height, InternalFormat _format);
		~Renderbuffer();
		// Move but not copy-able
		Renderbuffer(Renderbuffer&& _rhs);
		Renderbuffer(const Renderbuffer&) = delete;
		Renderbuffer& operator = (Renderbuffer&& _rhs);
		Renderbuffer& operator = (const Renderbuffer& _rhs) = delete;

		InternalFormat format() const { return m_format; }

		GLuint glID() { return m_id; }
	private:
		GLuint m_id;
		InternalFormat m_format;
	};

	// A frame buffer is a collection of render-targets.
	class Framebuffer
	{
	public:
		Framebuffer();
		~Framebuffer();
		// Move but not copy-able
		Framebuffer(Framebuffer&& _rhs);
		Framebuffer(const Framebuffer&) = delete;
		Framebuffer& operator = (Framebuffer&& _rhs);
		Framebuffer& operator = (const Framebuffer& _rhs) = delete;

		// Attach a renderbuffer as the target. Using a renderbuffer can
		// be faster, but does not allow to read the data later.
		void attachDepthTarget(Renderbuffer& _target);
		// Attach a standard 2D texture as depth target. Use this if you
		// want to read the data later.
		void attachDepthTarget(Texture& _target, GLuint _mipLevel = 0);
		// Attach an array or cube-map texture as depth target.
		void attachDepthTarget(Texture& _target, GLuint _layer, GLuint _mipLevel);
		// Attach a standard 2D texture
		void attachColorTarget(GLuint _index, Texture& _target, GLuint _mipLevel = 0);
		// Attach a layer of an array or a cube-map.
		void attachColorTarget(GLuint _index, Texture& _target, GLuint _layer, GLuint _mipLevel);

		// Check frame buffer completeness once after all attachments are bound.
		// Also, calls glDrawBuffers() to enable the attachments.
		void validate();

		// Bind the framebuffer
		void bind();

		GLuint glID() { return m_id; }
	private:
		GLuint m_id;
		std::vector<GLenum> m_drawBuffers;
	};

} // namespace gpupro