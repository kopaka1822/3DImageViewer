#pragma once

#include "gl.hpp"
#include <vector>
#include <initializer_list>

namespace gpupro {

	// A program is a fixed set of shaders linked together.
	class Program
	{
	public:
		enum TransformFeedback
		{
			NONE,
			INTERLEAVED = GL_INTERLEAVED_ATTRIBS,
			SEPERATE_ATTRIBS = GL_SEPARATE_ATTRIBS
		};

		Program();
		// Shortcut construction: Attach and link immediately.
		Program(class Shader& _compShader, TransformFeedback _feedback = TransformFeedback::NONE, std::initializer_list<std::string> _fbVaryings = {});
		Program(class Shader& _vertShader, class Shader& _fragShader, TransformFeedback _feedback = TransformFeedback::NONE, std::initializer_list<std::string> _fbVaryings = {});
		Program(class Shader& _vertShader, class Shader& _geomShader, class Shader& _fragShader, TransformFeedback _feedback = TransformFeedback::NONE, std::initializer_list<std::string> _fbVaryings = {});
		Program(class Shader& _vertShader, class Shader& _tessEvaShader, class Shader& _tessContrShader, class Shader& _fragShader, TransformFeedback _feedback = TransformFeedback::NONE, std::initializer_list<std::string> _fbVaryings = {});
		Program(class Shader& _vertShader, class Shader& _tessEvaShader, class Shader& _tessContrShader, class Shader& _geomShader, class Shader& _fragShader, TransformFeedback _feedback = TransformFeedback::NONE, std::initializer_list<std::string> _fbVaryings = {});
		~Program();
		// Move but not copy-able
		Program(Program&& _rhs);
		Program(const Program&) = delete;
		Program& operator = (Program&& _rhs);
		Program& operator = (const Program&) = delete;

		// Add a compiled shader object to this program.
		void attach(class Shader& _shader);
		// Link all the shaders now.
		void link();
		// Aplly transform feedback if wished
		void applyFeedback(TransformFeedback _feedback, std::initializer_list<std::string> _fbVaryings) const;

		// Load from cached binary data (core since 4.1). This includes an
		// implicit linking.
		// The binary blob is driver dependent!
		void loadFromBinary(GLenum _binaryFormat, const std::vector<unsigned char>& _binary);
		// Load a shader from a binary blob which was compiled
		// for the very same driver.
		// _binaryFormat: Second return value which specifies the format
		//		of the data.
		std::vector<unsigned char> getBinary(GLenum& _binaryFormat);

		GLuint glID() { return m_id; }
	private:
		GLuint m_id;
	};

} // namespace gpupro