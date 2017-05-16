#include "framebuffer.hpp"

#include <iostream>



gpupro::Renderbuffer::Renderbuffer(GLsizei _width, GLsizei _height, InternalFormat _format) :
	m_format(_format)
{
	glGenRenderbuffers(1, &m_id);
	glBindRenderbuffer(GL_RENDERBUFFER, m_id);
	glRenderbufferStorage(GL_RENDERBUFFER, static_cast<GLenum>(_format), _width, _height);
}

gpupro::Renderbuffer::~Renderbuffer()
{
	glDeleteRenderbuffers(1, &m_id);
}

gpupro::Renderbuffer::Renderbuffer(Renderbuffer&& _rhs) :
	m_id(_rhs.m_id),
	m_format(_rhs.m_format)
{
	_rhs.m_id = 0;
}

gpupro::Renderbuffer& gpupro::Renderbuffer::operator=(Renderbuffer&& _rhs)
{
	glDeleteRenderbuffers(1, &m_id);

	m_id = _rhs.m_id;
	m_format = _rhs.m_format;
	_rhs.m_id = 0;

	return *this;
}






gpupro::Framebuffer::Framebuffer()
{
	glGenFramebuffers(1, &m_id);
}

gpupro::Framebuffer::~Framebuffer()
{
	glDeleteFramebuffers(1, &m_id);
}

gpupro::Framebuffer::Framebuffer(Framebuffer&& _rhs) :
	m_id(_rhs.m_id),
	m_drawBuffers(move(_rhs.m_drawBuffers))
{
	_rhs.m_id = 0;
}

gpupro::Framebuffer& gpupro::Framebuffer::operator=(Framebuffer&& _rhs)
{
	glDeleteFramebuffers(1, &m_id);

	m_id = _rhs.m_id;
	m_drawBuffers = move(_rhs.m_drawBuffers);
	_rhs.m_id = 0;
	return *this;
}

void gpupro::Framebuffer::attachDepthTarget(Renderbuffer& _target)
{
	if(!isDepthFormat(_target.format()))
	{
		std::cerr << "Cannot attach the texture as depth target. It has a wrong format.\n";
		return;
	}

	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, m_id);
	glFramebufferRenderbuffer(GL_DRAW_FRAMEBUFFER, isStencilFormat(_target.format()) ? GL_DEPTH_STENCIL_ATTACHMENT : GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, _target.glID());
}

void gpupro::Framebuffer::attachDepthTarget(Texture& _target, GLuint _mipLevel)
{
	if(!isDepthFormat(_target.format()))
	{
		std::cerr << "Cannot attach the texture as depth target. It has a wrong format.\n";
		return;
	}

	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, m_id);
	glFramebufferTexture(GL_DRAW_FRAMEBUFFER, isStencilFormat(_target.format()) ? GL_DEPTH_STENCIL_ATTACHMENT : GL_DEPTH_ATTACHMENT, _target.glID(), _mipLevel);
}

void gpupro::Framebuffer::attachDepthTarget(Texture& _target, GLuint _layer, GLuint _mipLevel)
{
	if(!isDepthFormat(_target.format()))
	{
		std::cerr << "Cannot attach the texture as depth target. It has a wrong format.\n";
		return;
	}

	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, m_id);
	glFramebufferTextureLayer(GL_DRAW_FRAMEBUFFER, isStencilFormat(_target.format()) ? GL_DEPTH_STENCIL_ATTACHMENT : GL_DEPTH_ATTACHMENT, _target.glID(), _mipLevel, _layer);
}

void gpupro::Framebuffer::attachColorTarget(GLuint _index, Texture& _target, GLuint _mipLevel)
{
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, m_id);
	glFramebufferTexture(GL_DRAW_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + _index, _target.glID(), _mipLevel);

	// To enable all the color targets we need a call to glDrawBuffers.
	// Since this must be done for all targets at the same time, collect the
	// current buffer.
	m_drawBuffers.push_back(GL_COLOR_ATTACHMENT0 + _index);
}

void gpupro::Framebuffer::attachColorTarget(GLuint _index, Texture& _target, GLuint _layer, GLuint _mipLevel)
{
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, m_id);
	glFramebufferTextureLayer(GL_DRAW_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + _index, _target.glID(), _mipLevel, _layer);

	// To enable all the color targets we need a call to glDrawBuffers.
	// Since this must be done for all targets at the same time, collect the
	// current buffer.
	m_drawBuffers.push_back(GL_COLOR_ATTACHMENT0 + _index);
}

void gpupro::Framebuffer::validate()
{
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, m_id);
	GLenum status = glCheckFramebufferStatus(GL_DRAW_FRAMEBUFFER);
	switch(status)
	{
	case GL_FRAMEBUFFER_COMPLETE:
		std::cerr << "INF: Framebuffer " << m_id << " is complete.\n";
		break;
	case GL_FRAMEBUFFER_UNDEFINED:
		std::cerr << "ERR: There is no back buffer nor another framebuffer.\n";
		return;
	case GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT:
		std::cerr << "ERR: At least one of the attachments is incomplete.\n";
		return;
	case GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT:
		std::cerr << "ERR: There are no attachments to this framebuffer.\n";
		return;
	case GL_FRAMEBUFFER_UNSUPPORTED:
		std::cerr << "ERR: An internal format or the combination of formats is invalid.\n";
		return;
	default:
		// There are some more errors which are less common...
		std::cerr << "ERR: Framebuffer incomplete.\n";
		return;
	}

	// Enable all the attached targets from the list.
	glDrawBuffers(static_cast<GLsizei>(m_drawBuffers.size()), m_drawBuffers.data());
}

void gpupro::Framebuffer::bind()
{
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, m_id);
}
