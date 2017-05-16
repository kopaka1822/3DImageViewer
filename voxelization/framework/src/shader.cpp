#include "shader.hpp"

#include <string>
#include <vector>
#include <iostream>

gpupro::Shader::Shader(Type _type)
{
	m_id = glCreateShader(static_cast<GLenum>(_type));
	m_type = static_cast<GLenum>(_type);
}

gpupro::Shader::Shader(Type _type, const char* _fileName) :
	Shader(_type)
{
	loadFromFile(_fileName);
}

gpupro::Shader::~Shader()
{
	glDeleteShader(m_id);
}

gpupro::Shader::Shader(Shader&& _rhs) :
	m_id(_rhs.m_id),
	m_type(_rhs.m_type)
{
	_rhs.m_id = 0;
}

gpupro::Shader& gpupro::Shader::operator=(Shader&& _rhs)
{
	glDeleteShader(m_id);

	m_id = _rhs.m_id;
	m_type = _rhs.m_type;
	_rhs.m_id = 0;
	return *this;
}

void gpupro::Shader::loadFromSource(const char* _source, const char* _debugName)
{
	// Attach one or multiple strings as source code.
	glShaderSource(m_id, 1, &_source, nullptr);

	// Compile
	glCompileShader(m_id);
	// Check success
	GLint isCompiled = 0;
	glGetShaderiv(m_id, GL_COMPILE_STATUS, &isCompiled);
	if(isCompiled == GL_FALSE)
	{
		// Read out the error message.
		GLint length = 0;
		glGetShaderiv(m_id, GL_INFO_LOG_LENGTH, &length);
		std::string errorLog;
		errorLog.resize(length);
		glGetShaderInfoLog(m_id, length, &length, &errorLog[0]);
		if(_debugName)
			errorLog = "Failed to compile " + std::string(_debugName) + '\n' + errorLog;
		else
			errorLog = "Failed to compile shader " + std::to_string(m_id) + '\n' + errorLog;
		throw std::exception(errorLog.c_str());
	} else {
		std::cerr << "INF: Successfully compiled " << (_debugName ? _debugName : "shader") << "\n";
	}
}

void gpupro::Shader::loadFromFile(const char* _fileName)
{
	// Open the file
	FILE* file = fopen(_fileName, "rb");
	if(!file) throw std::exception(("Cannot open shader file: " + std::string(_fileName)).c_str());

	// Get file size and allocate memory
	fseek(file, 0, SEEK_END);
	unsigned length = ftell(file);
	fseek(file, 0, SEEK_SET);
	std::string source;
	source.resize(length + 1);

	// Read file and add 0-termination
	fread(&source[0], length, 1, file);
	source[length] = 0;
	fclose(file);

	loadFromSource(source.c_str(), _fileName);
}
