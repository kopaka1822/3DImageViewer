#include "model.hpp"

using namespace glm;

gpupro::Model::Model(const OBJLoader& _loader) :
	m_positions(Buffer::Type::VERTEX, static_cast<GLuint>(sizeof(vec3)), _loader.getNumVertices(), Buffer::Usage(), _loader.getPositions()),
	m_tangentSpaces(Buffer::Type::VERTEX, static_cast<GLuint>(sizeof(vec3)*3), _loader.getNumVertices(), Buffer::Usage(), _loader.getTangentSpaces()),
	m_texCoords(Buffer::Type::VERTEX, static_cast<GLuint>(sizeof(vec2)), _loader.getNumVertices(), Buffer::Usage(), _loader.getTexCoords()),
	m_indices(Buffer::Type::INDEX, 4, _loader.getNumIndices(), Buffer::Usage(), _loader.getIndices())
{
	m_bbMin = m_bbMax = _loader.getPositions()[0];
	for(unsigned i = 1; i < _loader.getNumVertices(); ++i)
	{
		m_bbMin = min(m_bbMin, _loader.getPositions()[i]);
		m_bbMax = max(m_bbMax, _loader.getPositions()[i]);
	}
}

void gpupro::Model::bind(int _posBindIdx, int _tsBindIdx, int _texBindIdx)
{
	if(_posBindIdx >= 0)
		m_positions.bindAsVertexBuffer(_posBindIdx);
	if(_tsBindIdx >= 0)
		m_tangentSpaces.bindAsVertexBuffer(_tsBindIdx);
	if(_texBindIdx >= 0)
		m_texCoords.bindAsVertexBuffer(_texBindIdx);
	m_indices.bindAsIndexBuffer();
}

void gpupro::Model::draw(DrawPrimitiveType _primType) const
{
	glDrawElements(static_cast<GLenum>(_primType), m_indices.numElements(), GL_UNSIGNED_INT, nullptr);
}
