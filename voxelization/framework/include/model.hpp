#pragma once

#include "objloader.hpp"
#include "buffer.hpp"

namespace gpupro {
	// The model is a helper class which takes data from a loader
	// (currently there is only the objloader) and handles the binding
	// of the data.
	//
	// The vertex structure of a model is divided into 3 buffers:
	// Positions: vec3
	// Tangent-Spaces: vec3 | vec3 | vec3
	// Texture coordinates: vec2
	// During binding a set of them can be chosen.
	class Model
	{
	public:
		Model(const OBJLoader& _loader);

		enum class DrawPrimitiveType {
			TRIANGLES = GL_TRIANGLES,
			POINTS = GL_POINTS,
			LINES = GL_LINES,
			PATCHES = GL_PATCHES
		};

		// Binds all vertex buffers and the index buffer.
		// _posBindIdx: Binding slot of the positions-vertex buffer.
		//		A negative number causes the buffer to not be bound.
		// _tsBindIdx: Binding slot of the tangent-space-vertex buffer.
		//		A negative number causes the buffer to not be bound.
		// _texBindIdx: Binding slot of the texCoord-vertex buffer.
		//		A negative number causes the buffer to not be bound.
		void bind(int _posBindIdx, int _tsBindIdx, int _texBindIdx);

		// Call glDrawElements for the entire internal vertex buffer.
		void draw(DrawPrimitiveType _primType = DrawPrimitiveType::TRIANGLES) const;

		const glm::vec3& boundingBoxMin() const { return m_bbMin; }
		const glm::vec3& boundingBoxMax() const { return m_bbMax; }
	private:
		Buffer m_positions;
		Buffer m_tangentSpaces;
		Buffer m_texCoords;
		Buffer m_indices;

		glm::vec3 m_bbMin;
		glm::vec3 m_bbMax;
	};

} // namespace gpupro
