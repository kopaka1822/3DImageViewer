#include "format.hpp"


bool gpupro::isDepthFormat(InternalFormat _format)
{
	switch(_format)
	{
	case InternalFormat::DEPTH24_STENCIL8:
	case InternalFormat::DEPTH32F_STENCIL8:
	case InternalFormat::DEPTH_COMPONENT16:
	case InternalFormat::DEPTH_COMPONENT24:
	case InternalFormat::DEPTH_COMPONENT32F:
		return true;
	}
	return false;
}

bool gpupro::isStencilFormat(InternalFormat _format)
{
	switch(_format)
	{
	case InternalFormat::DEPTH24_STENCIL8:
	case InternalFormat::DEPTH32F_STENCIL8:
	case InternalFormat::STENCIL_INDEX8:
		return true;
	}
	return false;
}

bool gpupro::isSignedFormat(InternalFormat _format)
{
	switch(_format)
	{
	case InternalFormat::R8S:
	case InternalFormat::R16S:
	case InternalFormat::RG8S:
	case InternalFormat::RG16S:
	case InternalFormat::RGB8S:
	case InternalFormat::RGB16S:
	case InternalFormat::RGBA8S:
	case InternalFormat::RGBA16S:
		return true;
	}
	return false;
}