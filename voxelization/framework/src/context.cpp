#include "context.hpp"
#include "gl.hpp"
#include <iostream>
#include <string>

static void glDebugOutput(GLenum _source, GLenum _type, GLuint _id, GLenum _severity, GLsizei _length, const GLchar* _message, const void* _userParam)
{
	std::string debSource, debType, debSev;

	if (_source == GL_DEBUG_SOURCE_API)
		debSource = "OpenGL";
	else if (_source == GL_DEBUG_SOURCE_WINDOW_SYSTEM)
		debSource = "Windows";
	else if (_source == GL_DEBUG_SOURCE_SHADER_COMPILER)
		debSource = "Shader Compiler";
	else if (_source == GL_DEBUG_SOURCE_THIRD_PARTY)
		debSource = "Third Party";
	else if (_source == GL_DEBUG_SOURCE_APPLICATION)
		debSource = "Application";
	else if (_source == GL_DEBUG_SOURCE_OTHER)
		debSource = "Other";

	if (_type == GL_DEBUG_TYPE_ERROR)
		debType = "error";
	else if (_type == GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR)
		debType = "deprecated behavior";
	else if (_type == GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR)
		debType = "undefined behavior";
	else if (_type == GL_DEBUG_TYPE_PORTABILITY)
		debType = "portability";
	else if (_type == GL_DEBUG_TYPE_PERFORMANCE)
		debType = "performance";
	else if (_type == GL_DEBUG_TYPE_OTHER)
		debType = "message";

	if (_severity == GL_DEBUG_SEVERITY_HIGH)
		debSev = "high";
	else if (_severity == GL_DEBUG_SEVERITY_MEDIUM)
		debSev = "medium";
	else if (_severity == GL_DEBUG_SEVERITY_LOW)
		debSev = "low";
	else if (_severity == GL_DEBUG_SEVERITY_NOTIFICATION)
		debSev = "note";

	std::string logMessage = debSource + ": " + debType + "(" + debSev + ") " + std::to_string(_id) + ": " + _message;
	if (_type == GL_DEBUG_TYPE_ERROR || _type == GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR)
		throw std::exception(logMessage.c_str());
	else if (_type == GL_DEBUG_TYPE_PERFORMANCE)
		std::cerr << "INF: " << logMessage.c_str() << '\n';
	else
		std::cerr << "WAR: " << logMessage.c_str() << '\n';
}

gpupro::OGLContext::OGLContext(DebugSeverity _dbgLevel)
{
	if(!gladLoadGL())
		throw std::exception("Cannot initialize Glad/load gl-function pointers!\n");
	std::cerr << "INF: Loaded GL-context is version " << GLVersion.major << '.' << GLVersion.minor << '\n';

	// Disable or enable the different levels
	glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DEBUG_SEVERITY_NOTIFICATION, 0, nullptr, _dbgLevel <= DebugSeverity::NOTIFICATION ? GL_TRUE : GL_FALSE);
	glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DEBUG_SEVERITY_LOW, 0, nullptr, _dbgLevel <= DebugSeverity::LOW ? GL_TRUE : GL_FALSE);
	glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DEBUG_SEVERITY_MEDIUM, 0, nullptr, _dbgLevel <= DebugSeverity::MEDIUM ? GL_TRUE : GL_FALSE);
	glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DEBUG_SEVERITY_HIGH, 0, nullptr, _dbgLevel <= DebugSeverity::HIGH ? GL_TRUE : GL_FALSE);
	glEnable(GL_DEBUG_OUTPUT);
	glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
	glDebugMessageCallback(&glDebugOutput, nullptr);

	// Enable seamless cube map sampling which is always a good idea since
	// it is core (3.2).
	glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);
}

void gpupro::OGLContext::setState(Pipeline& _pipeline)
{
	// ***** Rasterizer state *************************************************
	if(m_currentState.rasterizer.cullMode != _pipeline.rasterizer.cullMode)
	{
		if(_pipeline.rasterizer.cullMode == RasterizerState::CullMode::NONE)
			glDisable(GL_CULL_FACE);
		else {
			glEnable(GL_CULL_FACE);
			glCullFace(static_cast<GLenum>(_pipeline.rasterizer.cullMode));
		}
		m_currentState.rasterizer.cullMode = _pipeline.rasterizer.cullMode;
	}
	if(m_currentState.rasterizer.frontFaceWinding != _pipeline.rasterizer.frontFaceWinding)
	{
		glFrontFace(static_cast<GLenum>(_pipeline.rasterizer.frontFaceWinding));
		m_currentState.rasterizer.frontFaceWinding = _pipeline.rasterizer.frontFaceWinding;
	}
	if(m_currentState.rasterizer.fillMode != _pipeline.rasterizer.fillMode)
	{
		glPolygonMode(GL_FRONT_AND_BACK, static_cast<GLenum>(_pipeline.rasterizer.fillMode));
		m_currentState.rasterizer.fillMode = _pipeline.rasterizer.fillMode;
	}
	if(m_currentState.rasterizer.lineWidth != _pipeline.rasterizer.lineWidth)
	{
		glLineWidth(_pipeline.rasterizer.lineWidth);
		m_currentState.rasterizer.lineWidth = _pipeline.rasterizer.lineWidth;
	}
	if(m_currentState.rasterizer.discard != _pipeline.rasterizer.discard)
	{
		if(_pipeline.rasterizer.discard)
			glEnable(GL_RASTERIZER_DISCARD);
		else
			glDisable(GL_RASTERIZER_DISCARD);
		m_currentState.rasterizer.discard = _pipeline.rasterizer.discard;
	}
	if(m_currentState.rasterizer.colorWrite != _pipeline.rasterizer.colorWrite)
	{
		glColorMask(_pipeline.rasterizer.colorWrite, _pipeline.rasterizer.colorWrite, _pipeline.rasterizer.colorWrite, _pipeline.rasterizer.colorWrite);
		m_currentState.rasterizer.colorWrite = _pipeline.rasterizer.colorWrite;
	}
	if(m_currentState.rasterizer.dithering != _pipeline.rasterizer.dithering)
	{
		if(_pipeline.rasterizer.dithering)
			glEnable(GL_DITHER);
		else
			glDisable(GL_DITHER);
		m_currentState.rasterizer.dithering = _pipeline.rasterizer.dithering;
	}

	// ***** Depth-stencil state **********************************************
	if(m_currentState.depthStencil.depthTest != _pipeline.depthStencil.depthTest)
	{
		if(_pipeline.depthStencil.depthTest)
			glEnable(GL_DEPTH_TEST);
		else
			glDisable(GL_DEPTH_TEST);
		m_currentState.depthStencil.depthTest = _pipeline.depthStencil.depthTest;
	}
	if(m_currentState.depthStencil.depthTest &&
		m_currentState.depthStencil.depthCmpFunc != _pipeline.depthStencil.depthCmpFunc)
	{
		glDepthFunc(static_cast<GLenum>(_pipeline.depthStencil.depthCmpFunc));
		m_currentState.depthStencil.depthCmpFunc = _pipeline.depthStencil.depthCmpFunc;
	}
	if(m_currentState.depthStencil.depthWrite != _pipeline.depthStencil.depthWrite)
	{
		glDepthMask(_pipeline.depthStencil.depthWrite);
		m_currentState.depthStencil.depthWrite = _pipeline.depthStencil.depthWrite;
	}
	if(m_currentState.depthStencil.stencilTest != _pipeline.depthStencil.stencilTest)
	{
		if(_pipeline.depthStencil.stencilTest)
			glEnable(GL_STENCIL_TEST);
		else
			glDisable(GL_STENCIL_TEST);
		m_currentState.depthStencil.stencilTest = _pipeline.depthStencil.stencilTest;
	}
	if(m_currentState.depthStencil.stencilTest)
	{
		if(m_currentState.depthStencil.stencilCmpFuncFront != _pipeline.depthStencil.stencilCmpFuncFront
			|| m_currentState.depthStencil.stencilRefFront != _pipeline.depthStencil.stencilRefFront)
		{
			glStencilFuncSeparate(GL_FRONT, static_cast<GLenum>(_pipeline.depthStencil.stencilCmpFuncFront), _pipeline.depthStencil.stencilRefFront, 0xffffffff);
			m_currentState.depthStencil.stencilCmpFuncFront = _pipeline.depthStencil.stencilCmpFuncFront;
			m_currentState.depthStencil.stencilRefFront = _pipeline.depthStencil.stencilRefFront;
		}
		if(m_currentState.depthStencil.stencilCmpFuncBack != _pipeline.depthStencil.stencilCmpFuncBack
			|| m_currentState.depthStencil.stencilRefBack != _pipeline.depthStencil.stencilRefBack)
		{
			glStencilFuncSeparate(GL_BACK, static_cast<GLenum>(_pipeline.depthStencil.stencilCmpFuncBack), _pipeline.depthStencil.stencilRefBack, 0xffffffff);
			m_currentState.depthStencil.stencilCmpFuncBack = _pipeline.depthStencil.stencilCmpFuncBack;
			m_currentState.depthStencil.stencilRefBack = _pipeline.depthStencil.stencilRefBack;
		}
		if(m_currentState.depthStencil.stencilFailOpFront != _pipeline.depthStencil.stencilFailOpFront
			|| m_currentState.depthStencil.zfailOpFront != _pipeline.depthStencil.zfailOpFront
			|| m_currentState.depthStencil.passOpFront != _pipeline.depthStencil.passOpFront)
		{
			glStencilOpSeparate(GL_FRONT, static_cast<GLenum>(_pipeline.depthStencil.stencilFailOpFront),
				static_cast<GLenum>(_pipeline.depthStencil.zfailOpFront),
				static_cast<GLenum>(_pipeline.depthStencil.passOpFront));
			m_currentState.depthStencil.stencilFailOpFront = _pipeline.depthStencil.stencilFailOpFront;
			m_currentState.depthStencil.zfailOpFront = _pipeline.depthStencil.zfailOpFront;
			m_currentState.depthStencil.passOpFront = _pipeline.depthStencil.passOpFront;
		}
		if(m_currentState.depthStencil.stencilFailOpBack != _pipeline.depthStencil.stencilFailOpBack
			|| m_currentState.depthStencil.zfailOpBack != _pipeline.depthStencil.zfailOpBack
			|| m_currentState.depthStencil.passOpBack != _pipeline.depthStencil.passOpBack)
		{
			glStencilOpSeparate(GL_BACK, static_cast<GLenum>(_pipeline.depthStencil.stencilFailOpBack),
				static_cast<GLenum>(_pipeline.depthStencil.zfailOpBack),
				static_cast<GLenum>(_pipeline.depthStencil.passOpBack));
			m_currentState.depthStencil.stencilFailOpBack = _pipeline.depthStencil.stencilFailOpBack;
			m_currentState.depthStencil.zfailOpBack = _pipeline.depthStencil.zfailOpBack;
			m_currentState.depthStencil.passOpBack = _pipeline.depthStencil.passOpBack;
		}
	}

	// ***** Blend state ******************************************************
	if(m_currentState.blendState.enableBlending != _pipeline.blendState.enableBlending)
	{
		if(_pipeline.blendState.enableBlending == BlendState::BlendMode::BLEND) {
			glEnable(GL_BLEND);
			glDisable(GL_COLOR_LOGIC_OP);
		} else if(_pipeline.blendState.enableBlending == BlendState::BlendMode::LOGIC)
			glEnable(GL_COLOR_LOGIC_OP);
		else {
			glDisable(GL_BLEND);
			glDisable(GL_COLOR_LOGIC_OP);
		}
		m_currentState.blendState.enableBlending = _pipeline.blendState.enableBlending;
	}
	if(m_currentState.blendState.enableBlending == BlendState::BlendMode::BLEND)
	{
		for(int i = 0; i < 8; ++i)
		{
			if(m_currentState.blendState.buf[i].colorBlendOp != _pipeline.blendState.buf[i].colorBlendOp
				|| m_currentState.blendState.buf[i].alphaBlendOp != _pipeline.blendState.buf[i].alphaBlendOp)
			{
				glBlendEquationSeparatei(i, static_cast<GLenum>(_pipeline.blendState.buf[i].colorBlendOp), static_cast<GLenum>(_pipeline.blendState.buf[i].alphaBlendOp));
				m_currentState.blendState.buf[i].colorBlendOp = _pipeline.blendState.buf[i].colorBlendOp;
				m_currentState.blendState.buf[i].alphaBlendOp = _pipeline.blendState.buf[i].alphaBlendOp;
			}
			if(m_currentState.blendState.buf[i].srcColorFactor != _pipeline.blendState.buf[i].srcColorFactor
				|| m_currentState.blendState.buf[i].srcAlphaFactor != _pipeline.blendState.buf[i].srcAlphaFactor
				|| m_currentState.blendState.buf[i].dstColorFactor != _pipeline.blendState.buf[i].dstColorFactor
				|| m_currentState.blendState.buf[i].dstAlphaFactor != _pipeline.blendState.buf[i].dstAlphaFactor)
			{
				glBlendFuncSeparatei(i,
					static_cast<GLenum>(_pipeline.blendState.buf[i].srcColorFactor), static_cast<GLenum>(_pipeline.blendState.buf[i].dstColorFactor),
					static_cast<GLenum>(_pipeline.blendState.buf[i].srcAlphaFactor), static_cast<GLenum>(_pipeline.blendState.buf[i].dstAlphaFactor));
				m_currentState.blendState.buf[i].srcColorFactor = _pipeline.blendState.buf[i].srcColorFactor;
				m_currentState.blendState.buf[i].srcAlphaFactor = _pipeline.blendState.buf[i].srcAlphaFactor;
				m_currentState.blendState.buf[i].dstColorFactor = _pipeline.blendState.buf[i].dstColorFactor;
				m_currentState.blendState.buf[i].dstAlphaFactor = _pipeline.blendState.buf[i].dstAlphaFactor;
			}
		}
	}
	if(m_currentState.blendState.logicOp != _pipeline.blendState.logicOp)
	{
		glLogicOp(static_cast<GLenum>(_pipeline.blendState.logicOp));
		m_currentState.blendState.logicOp = _pipeline.blendState.logicOp;
	}
	if(m_currentState.blendState.alphaToCoverage != _pipeline.blendState.alphaToCoverage)
	{
		if(_pipeline.blendState.alphaToCoverage)
			glEnable(GL_SAMPLE_ALPHA_TO_COVERAGE);
		else
			glDisable(GL_SAMPLE_ALPHA_TO_COVERAGE);
		m_currentState.blendState.alphaToCoverage = _pipeline.blendState.alphaToCoverage;
	}

	// ***** Sampler states ***************************************************
	for(int i = 0; i < 64; ++i)
	{
		if(m_currentState.samplerState[i] != _pipeline.samplerState[i])
		{
			glBindSampler(i, _pipeline.samplerState[i] ? _pipeline.samplerState[i]->glID() : 0);
			m_currentState.samplerState[i] = _pipeline.samplerState[i];
		}
	}

	// ***** Shader program ***************************************************
	if(_pipeline.shader) glUseProgram(_pipeline.shader->glID());

	static VertexFormat dummyVertexFormat({{}});
	if(_pipeline.vertexFormat) glBindVertexArray(_pipeline.vertexFormat->glID());
	else glBindVertexArray(dummyVertexFormat.glID());
}

void gpupro::OGLContext::setState(ComputePipeline & _pipeline)
{
	// ***** Sampler states ***************************************************
	for(int i = 0; i < 64; ++i)
	{
		if(m_currentState.samplerState[i] != _pipeline.samplerState[i])
		{
			glBindSampler(i, _pipeline.samplerState[i] ? _pipeline.samplerState[i]->glID() : 0);
			m_currentState.samplerState[i] = _pipeline.samplerState[i];
		}
	}

	// ***** Shader program ***************************************************
	if(_pipeline.shader) glUseProgram(_pipeline.shader->glID());
}
