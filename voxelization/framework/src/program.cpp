#include "program.hpp"
#include "shader.hpp"
#include <iostream>

gpupro::Program::Program()
{
	m_id = glCreateProgram();
}

gpupro::Program::Program(Shader & _compShader) :
	Program()
{
	attach(_compShader);
	link();
}

gpupro::Program::Program(Shader & _vertShader, Shader & _fragShader) :
	Program()
{
	attach(_vertShader);
	attach(_fragShader);
	link();
}

gpupro::Program::Program(Shader & _vertShader, Shader & _geomShader, Shader & _fragShader) :
	Program()
{
	attach(_vertShader);
	attach(_geomShader);
	attach(_fragShader);
	link();
}

gpupro::Program::Program(Shader & _vertShader, Shader & _tessEvaShader, Shader & _tessContrShader, Shader & _fragShader) :
	Program()
{
	attach(_vertShader);
	attach(_tessEvaShader);
	attach(_tessContrShader);
	attach(_fragShader);
	link();
}

gpupro::Program::Program(Shader & _vertShader, Shader & _tessEvaShader, Shader & _tessContrShader, Shader & _geomShader, Shader & _fragShader) :
	Program()
{
	attach(_vertShader);
	attach(_tessEvaShader);
	attach(_tessContrShader);
	attach(_geomShader);
	attach(_fragShader);
	link();
}

gpupro::Program::~Program()
{
	glDeleteProgram(m_id);
}

gpupro::Program::Program(Program&& _rhs) :
	m_id(_rhs.m_id)
{
	_rhs.m_id = 0;
}

gpupro::Program& gpupro::Program::operator=(Program&& _rhs)
{
	glDeleteProgram(m_id);

	m_id = _rhs.m_id;
	_rhs.m_id = 0;
	return *this;
}

void gpupro::Program::attach(Shader& _shader)
{
	glAttachShader(m_id, _shader.glID());
}

void gpupro::Program::link()
{
	// Link all attached shaders (if possible)
	glLinkProgram(m_id);
	// Check success
	GLint isLinked = 0;
	glGetProgramiv(m_id, GL_LINK_STATUS, &isLinked);
	if(isLinked == GL_FALSE)
	{
		// Read out the error message.
		GLint length = 0;
		glGetProgramiv(m_id, GL_INFO_LOG_LENGTH, &length);
		std::string errorLog;
		errorLog.reserve(length);
		glGetProgramInfoLog(m_id, length, &length, &errorLog[0]);
		throw std::exception(errorLog.c_str());
	} else {
		std::cerr << "INF: Successfully linked program " << m_id << "\n";
	}
}

void gpupro::Program::loadFromBinary(GLenum _binaryFormat, const std::vector<unsigned char>& _binary)
{
	glProgramBinary(m_id, _binaryFormat, _binary.data(), static_cast<GLsizei>(_binary.size()));
}

std::vector<unsigned char> gpupro::Program::getBinary(GLenum& _binaryFormat)
{
	// Get the memory requirements and reserve a vector.
	GLint binarySize = 0;
	glGetProgramiv(m_id, GL_PROGRAM_BINARY_LENGTH, &binarySize);
	std::vector<unsigned char> data;
	data.resize(binarySize);

	// Get the data
	glGetProgramBinary(m_id, static_cast<GLsizei>(data.size()), nullptr, &_binaryFormat, data.data());
	return std::move(data);
}
