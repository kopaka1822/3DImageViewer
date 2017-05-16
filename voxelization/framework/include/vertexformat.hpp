#pragma once

#include "gl.hpp"
#include <vector>

namespace gpupro {

	// A vertex attribute defines how a (part) of a buffer is bound to
	// stream inputs.
	struct VertexAttribute
	{
		// Possible element types.
		enum class Type
		{
			INT8 = GL_BYTE,
			UINT8 = GL_UNSIGNED_BYTE,
			INT16 = GL_SHORT,
			UINT16 = GL_UNSIGNED_SHORT,
			INT32 = GL_INT,
			UINT32 = GL_UNSIGNED_INT,

			FIXED = GL_FIXED,

			FLOAT = GL_FLOAT,
			HALF = GL_HALF_FLOAT,
			DOUBLE = GL_DOUBLE,

			INT_2_10_10_10 = GL_INT_2_10_10_10_REV,
			UINT_2_10_10_10 = GL_UNSIGNED_INT_10_10_10_2,
			UINT_10F_11F_11F = GL_UNSIGNED_INT_10F_11F_11F_REV,
		};

		// Index which is visible in the vertex shader (stream index).
		GLuint attributIndex;
		// Which vertex buffer is going to be used?
		GLuint vboBindingIndex;
		// The number of values per vertex.
		GLint numComponents;
		// The type from Type
		Type type;
		// If GL_TRUE than integers are normalized to [-1,1] (signed) or [0,1] (unsigned).
		// Otherwise glVertexAttribIFormat is used and data remains an integer.
		// !! This is different from the usual specification, where GL_FALSE would convert
		// integers to float (glVertexAttribFormat). However, this is considered highly
		// unexpected behavior (ever OpenGL programmer I ever met got wired bugs out of this).
		GLboolean normalized;
		// Offset within each vertex of the buffer
		GLuint offset;
		// The divisor is required for instancing. It tells how many
		// instances are drawn between advances of the attribute index.
		// 0: use for every instance (e.g. vertices)
		// 1: use one per instance (e.g. transformation)
		// x: use one for every x instances (e.g. don't know an application^^).
		GLuint divisor;
	};

	// A VertexArrayObject is a bundle of vertex buffers including binding semantics.
	// However, there is a second interpretation since (4.3) which separates the vertex
	// format from the real buffer binding.
	// A VertexFormat contains binding semantics only. The buffers must be bound later. 
	class VertexFormat
	{
	public:
		VertexFormat() : m_id(0) {}
		VertexFormat(const std::vector<VertexAttribute>& _attributes);
		~VertexFormat();
		// Move but not copy-able
		VertexFormat(VertexFormat&& _rhs);
		VertexFormat(const VertexFormat&) = delete;
		VertexFormat& operator = (VertexFormat&& _rhs);
		VertexFormat& operator = (const VertexFormat&) = delete;

		GLuint glID() { return m_id; }
	private:
		GLuint m_id;
	};

} // namespace gpupro