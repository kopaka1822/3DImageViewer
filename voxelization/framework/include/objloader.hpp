#pragma once

#include <glm/glm.hpp>
#include <vector>

namespace gpupro {

	class OBJLoader
	{
	public:
		struct TangentSpace
		{
			glm::vec3 normal;
			glm::vec3 tangent;
			glm::vec3 bitangent;
		};

		void load(const char* _fileName, bool _computeTangentSpace);

		unsigned getNumVertices() const					{ return static_cast<unsigned>(m_positions.size()); }
		unsigned getNumIndices() const					{ return static_cast<unsigned>(m_indices.size()); }
		const glm::vec3* getPositions() const			{ return m_positions.data(); }
		const TangentSpace* getTangentSpaces() const	{ return m_tangentSpaces.data(); }
		const glm::vec2* getTexCoords() const			{ return m_texCoords.data(); }
		const unsigned* getIndices() const				{ return m_indices.data(); }

	private:
		std::vector<glm::vec3> m_positions;
		std::vector<TangentSpace> m_tangentSpaces;
		std::vector<glm::vec2> m_texCoords;
		std::vector<unsigned> m_indices;

		void computeTangentSpace();
	};
} // namespace gpupro