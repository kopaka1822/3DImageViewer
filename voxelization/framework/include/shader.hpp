#pragma once

#include "gl.hpp"

namespace gpupro {

	// A shader is a single compiled GLSL file. They cannot be used directly,
	// but must be bound and linked in a program object.
	class Shader
	{
	public:
		enum class Type
		{
			VERTEX = GL_VERTEX_SHADER,
			FRAGMENT = GL_FRAGMENT_SHADER,
			TESS_EVALUATION = GL_TESS_EVALUATION_SHADER,
			TESS_CONTROL = GL_TESS_CONTROL_SHADER,
			GEOMETRY = GL_GEOMETRY_SHADER,
			COMPUTE = GL_COMPUTE_SHADER
		};

		Shader(Type _type);
		// Shortcut: equivalent to Shader(_type) and then loadFromFile(_fileName).
		Shader(Type _type, const char* _fileName);
		~Shader();
		// Move but not copy-able
		Shader(Shader&& _rhs);
		Shader(const Shader&) = delete;
		Shader& operator = (Shader&& _rhs);
		Shader& operator = (const Shader&) = delete;

		// Load from source code and compile.
		// This is called by loadFromFile indirectly.
		void loadFromSource(const char* _source, const char* _debugName = nullptr);
		// Reads the file in memory and calls loadFromSource.
		void loadFromFile(const char* _fileName);

		GLuint glID() { return m_id; }
		GLenum type() const { return m_type; }
	private:
		GLuint m_id;
		GLenum m_type;
	};

} // namespace gpupro