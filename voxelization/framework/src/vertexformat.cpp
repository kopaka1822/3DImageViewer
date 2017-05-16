#include "vertexformat.hpp"

static bool isIntegerType(gpupro::VertexAttribute::Type _t)
{
	switch(_t)
	{
	case gpupro::VertexAttribute::Type::INT8:
	case gpupro::VertexAttribute::Type::UINT8:
	case gpupro::VertexAttribute::Type::INT16:
	case gpupro::VertexAttribute::Type::UINT16:
	case gpupro::VertexAttribute::Type::INT32:
	case gpupro::VertexAttribute::Type::UINT32:
		return true;
	default:
		return false;
	}
}

gpupro::VertexFormat::VertexFormat(const std::vector<VertexAttribute>& _attributes)
{
	glGenVertexArrays(1, &m_id);
	glBindVertexArray(m_id);

	for(auto& attr : _attributes)
	{
		glEnableVertexAttribArray(attr.attributIndex);
		glVertexAttribDivisor(attr.attributIndex, attr.divisor);
		if(attr.type == VertexAttribute::Type::DOUBLE)
			glVertexAttribLFormat(attr.attributIndex, attr.numComponents, (GLenum)attr.type, attr.offset);
		else if(!attr.normalized && isIntegerType(attr.type))
			glVertexAttribIFormat(attr.attributIndex, attr.numComponents, (GLenum)attr.type, attr.offset);
		else
			glVertexAttribFormat(attr.attributIndex, attr.numComponents, (GLenum)attr.type, attr.normalized, attr.offset);
		// For an unknown reason (driver bug?) this must be called AFTER glVertexAttrib*Format!
		// Otherwise it is overwritten and expects attribIndex == vboIndex.
		glVertexAttribBinding(attr.attributIndex, attr.vboBindingIndex);
	}
}

gpupro::VertexFormat::~VertexFormat()
{
	glDeleteVertexArrays(1, &m_id);
}

gpupro::VertexFormat::VertexFormat(VertexFormat&& _rhs) :
	m_id(_rhs.m_id)
{
	_rhs.m_id = 0;
}

gpupro::VertexFormat& gpupro::VertexFormat::operator=(VertexFormat&& _rhs)
{
	glDeleteVertexArrays(1, &m_id);

	m_id = _rhs.m_id;
	_rhs.m_id = 0;

	return *this;
}
