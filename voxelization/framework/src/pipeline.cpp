#include "pipeline.hpp"

gpupro::SamplerState::SamplerState(
	Filter _minFilter,
	Filter _magFilter,
	Filter _mipFilter,
	float _maxAnisotropy,
	DepthCompareFunc _depthCmpFunc,
	BorderHandling _borderHandling,
	float _borderColor[4]
)
#ifdef DEBUG
	: m_minFilter(_minFilter),
	m_magFilter(_magFilter),
	m_mipFilter(_mipFilter),
	m_depthCmpFunc(_depthCmpFunc),
	m_borderHandling(_borderHandling)
#endif
{
	glGenSamplers(1, &m_id);

	glSamplerParameteri(m_id, GL_TEXTURE_WRAP_R, static_cast<GLint>(_borderHandling));
	glSamplerParameteri(m_id, GL_TEXTURE_WRAP_S, static_cast<GLint>(_borderHandling));
	glSamplerParameteri(m_id, GL_TEXTURE_WRAP_T, static_cast<GLint>(_borderHandling));

	if(_mipFilter ==  Filter::NONE)
		glSamplerParameteri(m_id, GL_TEXTURE_MIN_FILTER, static_cast<GLint>(_minFilter));
	else
		glSamplerParameteri(m_id, GL_TEXTURE_MIN_FILTER, _minFilter == Filter::NEAREST ?
			(_mipFilter == Filter::NEAREST ? GL_NEAREST_MIPMAP_NEAREST : GL_NEAREST_MIPMAP_LINEAR) : 
			(_mipFilter == Filter::NEAREST ? GL_LINEAR_MIPMAP_NEAREST : GL_LINEAR_MIPMAP_LINEAR)
		);
	glSamplerParameteri(m_id, GL_TEXTURE_MAG_FILTER, static_cast<GLint>(_magFilter));

	if(_maxAnisotropy > 1.0f)
		glSamplerParameterf(m_id, GL_TEXTURE_MAX_ANISOTROPY_EXT, _maxAnisotropy);

	if(_depthCmpFunc != DepthCompareFunc::DISABLE)
	{
		glSamplerParameteri(m_id, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_REF_TO_TEXTURE);
		glSamplerParameteri(m_id, GL_TEXTURE_COMPARE_FUNC, static_cast<GLint>(_depthCmpFunc));
	}

	if(_borderColor)
		glSamplerParameterfv(m_id, GL_TEXTURE_BORDER_COLOR, _borderColor);
}

gpupro::SamplerState::~SamplerState()
{
	glDeleteSamplers(1, &m_id);
}

gpupro::SamplerState::SamplerState(SamplerState&& _rhs) :
	m_id(_rhs.m_id)
#ifdef DEBUG
	, m_minFilter(_rhs.m_minFilter),
	m_magFilter(_rhs.m_magFilter),
	m_mipFilter(_rhs.m_mipFilter),
	m_depthCmpFunc(_rhs.m_depthCmpFunc),
	m_borderHandling(_rhs.m_borderHandling)
#endif
{
	_rhs.m_id = 0;
}

gpupro::SamplerState& gpupro::SamplerState::operator=(SamplerState&& _rhs)
{
	glDeleteSamplers(1, &m_id);

	m_id = _rhs.m_id;
#ifdef DEBUG
	m_minFilter = _rhs.m_minFilter;
	m_magFilter = _rhs.m_magFilter;
	m_mipFilter = _rhs.m_mipFilter;
	m_depthCmpFunc = _rhs.m_depthCmpFunc;
	m_borderHandling = _rhs.m_borderHandling;
#endif
	_rhs.m_id = 0;

	return *this;
}
