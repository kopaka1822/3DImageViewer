#pragma once

#include "program.hpp"
#include "vertexformat.hpp"

#include "gl.hpp"

namespace gpupro {

	struct RasterizerState
	{
		enum class CullMode
		{
			NONE = GL_NONE,
			FRONT = GL_FRONT,
			BACK = GL_BACK,
		};

		enum class FillMode
		{
			SOLID = GL_FILL,
			WIREFRAME = GL_LINE,
			POINTS = GL_POINT
		};

		enum class Winding
		{
			CW = GL_CW,
			CCW = GL_CCW
		};

		CullMode cullMode = CullMode::NONE;
		Winding frontFaceWinding = Winding::CCW;
		FillMode fillMode = FillMode::SOLID;
		float lineWidth = 1.0f;
		bool discard = false;			///< Discard primitives after transform feedback.
		bool colorWrite = true;			///< Uses glColorMask to enable/disable writes to the current frame buffer.
		bool dithering = true;			///< Dither color components or indices. This has an effect only if output target has a low bit-depth.
	};

	struct DepthStencilState
	{
		// Comparison functions for depth and stencil buffer
		enum class CmpFunc {
			LESS = GL_LESS,
			LESS_EQUAL = GL_LEQUAL,
			GREATER = GL_GREATER,
			GREATER_EQUAL = GL_GEQUAL,
			EQUAL = GL_EQUAL,
			NOT_EQUAL = GL_NOTEQUAL,
			NEVER = GL_NEVER,
			ALWAYS = GL_ALWAYS,
		};

		enum class StencilOp {
			KEEP = GL_KEEP,
			ZERO = GL_ZERO,
			REPLACE = GL_REPLACE,
			INCREMENT = GL_INCR_WRAP,	///< Increment. Restart from zero on overflow
			DECREMENT = GL_DECR_WRAP,	///< Decrement. Restart from maximum on underflow
			INC_SAT = GL_INCR,			///< Increment. Clamp to maximum on overflow
			DEC_SAT = GL_DECR,			///< Decrement. Clamp to 0 on underflow
			INVERT = GL_INVERT,			///< Flipp all bits
		};

		// Enable z-test.
		bool depthTest = false;
		// Set the comparison function for z-tests.
		CmpFunc depthCmpFunc = CmpFunc::LESS;
		// Enable depth writes. Otherwise z-buffer is used read only.
		bool depthWrite = true;
		// Disable both: stencil testing and updates
		bool stencilTest = false;
		// Stencil buffer comparison for front and back faces.
		CmpFunc stencilCmpFuncFront = CmpFunc::ALWAYS;
		CmpFunc stencilCmpFuncBack = CmpFunc::ALWAYS;
		// Reference values for front and back faces
		GLint stencilRefFront = 0;
		GLint stencilRefBack = 0;
		// Stencil buffer operations for front and back faces.
		// stencilFail: Did not pass stencil test.
		// zfailOpFront: Passed stencil test, but failed in z-test.
		// passOpFront: Passed both tests.
		StencilOp stencilFailOpFront = StencilOp::KEEP, zfailOpFront = StencilOp::KEEP, passOpFront = StencilOp::KEEP;
		StencilOp stencilFailOpBack = StencilOp::KEEP, zfailOpBack = StencilOp::KEEP, passOpBack = StencilOp::KEEP;
	};

	struct BlendState
	{
		enum class BlendOp {
			ADD = GL_FUNC_ADD,
			SUBTRACT = GL_FUNC_SUBTRACT,
			REVERSE_SUBTRACT = GL_FUNC_REVERSE_SUBTRACT,
			MIN = GL_MIN,
			MAX = GL_MAX
		};

		enum class LogicOp {
			CLEAR = GL_CLEAR,
			SET_ONE = GL_SET,
			COPY = GL_COPY,
			COPY_INV = GL_COPY_INVERTED,
			NOOP = GL_NOOP,
			INVERT = GL_INVERT,
			AND = GL_AND,
			NAND = GL_NAND,
			OR = GL_OR,
			NOR = GL_NOR,
			XOR = GL_XOR,
			EQUIV = GL_EQUIV,
			AND_INV_DST = GL_AND_REVERSE,
			AND_INV_SRC = GL_AND_INVERTED,
			OR_INV_DST = GL_OR_REVERSE,
			OR_INV_SRC = GL_OR_INVERTED
		};

		enum class BlendFactor {
			ZERO = GL_ZERO,
			ONE = GL_ONE,
			SRC_ALPHA = GL_SRC_ALPHA,
			INV_SRC_ALPHA = GL_ONE_MINUS_SRC_ALPHA,
			DST_ALPHA = GL_DST_ALPHA,
			INV_DST_ALPHA = GL_ONE_MINUS_DST_ALPHA,
			SRC_COLOR = GL_SRC_COLOR,
			INV_SRC_COLOR = GL_ONE_MINUS_SRC_COLOR,
			DST_COLOR = GL_DST_COLOR,
			INV_DST_COLOR = GL_ONE_MINUS_DST_COLOR,
		};

		// Logic and alphablending exclude each other globally for all buffers.
		enum class BlendMode {
			DISABLE,
			BLEND,
			LOGIC
		};

		// One blend state per different draw buffer.
		// GL_MAX_DRAW_BUFFERS is 8 on most modern GPUs.
		struct {
			BlendFactor srcColorFactor = BlendFactor::ONE, srcAlphaFactor = BlendFactor::ONE;
			BlendFactor dstColorFactor = BlendFactor::ZERO, dstAlphaFactor = BlendFactor::ZERO;
			BlendOp colorBlendOp = BlendOp::ADD;
			BlendOp alphaBlendOp = BlendOp::ADD;
		} buf[8];
		// There are also some properties, which are globally shared between
		// all draw-buffers.
		BlendMode enableBlending = BlendMode::DISABLE;
		LogicOp logicOp = LogicOp::COPY;
		bool alphaToCoverage = false;	///< Use alpha values to generate sub-pixel coverage masks.
	};

	// Sampler states are real resources which must be allocated...
	// Create some sampler objects globally and share them everywhere.
	// You will probably never need more than 3 different samplers.
	class SamplerState
	{
	public:
		enum struct BorderHandling {
			REPEAT = GL_REPEAT,
			CLAMP = GL_CLAMP_TO_EDGE,
			BORDER = GL_CLAMP_TO_BORDER,
			MIRROR = GL_MIRRORED_REPEAT
		};

		enum struct Filter {
			NONE,				///< No filtering is only valid for mip-filter (i.e. mip-mapping can be disabled)
			NEAREST = GL_NEAREST,
			LINEAR = GL_LINEAR
		};

		// Use depth comparison for textures with a depth-format (shadow mapping).
		// For all other textures choose DISABLE.
		enum struct DepthCompareFunc {
			LESS = GL_LESS,
			LESS_EQUAL = GL_LEQUAL,
			GREATER = GL_GREATER,
			GREATER_EQUAL = GL_GEQUAL,
			EQUAL = GL_EQUAL,
			NOT_EQUAL = GL_NOTEQUAL,
			NEVER = GL_NEVER,
			ALWAYS = GL_ALWAYS,
			DISABLE
		};

		// The constructor creates the entire sampler object with all parameters.
		// The arguments are sorted for importance and the defaults are that
		// of OpenGL.
		// _maxAnisotropy: A number between 1.0f and GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT. 1.0f disables
		//		anisotropic filtering.
		SamplerState(Filter _minFilter = Filter::NEAREST,
			Filter _magFilter = Filter::LINEAR,
			Filter _mipFilter = Filter::LINEAR,
			float _maxAnisotropy = 1.0f,
			DepthCompareFunc _depthCmpFunc = DepthCompareFunc::DISABLE,
			BorderHandling _borderHandling = BorderHandling::REPEAT,
			float _borderColor[4] = nullptr
		);
		~SamplerState();
		// Move but not copy-able
		SamplerState(SamplerState&& _rhs);
		SamplerState(const SamplerState&) = delete;
		SamplerState& operator = (SamplerState&& _rhs);
		SamplerState& operator = (const SamplerState& _rhs) = delete;

		GLuint glID() { return m_id; }
	private:
		GLuint m_id;
		// The following states are not required for execution.
		// They only expose the state for debugging reasons.
#ifdef DEBUG
		Filter m_minFilter;
		Filter m_magFilter;
		Filter m_mipFilter;
		DepthCompareFunc m_depthCmpFunc;
		// It is possible to use different handlers in different dimensions.
		// Since there are practically no usecases this framework simplifies
		// this down to one setting.
		BorderHandling m_borderHandling;
#endif
	};

	// A pipeline is a collection of the entire OpenGL state.
	// Further, it bundles a shader, a vertex format together
	// with the state.
	//
	// The reasons for the monolithic state object are:
	//	* Robustness in large projects against human made mistakes.
	//	  When rendering in multiple passes each pass has its own fixed
	//    state. Changing other passes will not influence its behavior.
	//	* Simplicity
	//	  You don't need to find all those different glEnable, glMask...
	//	  commands. Also, you have an overview of the standard values which
	//	  are set in OpenGL initially.
	//  * Performance (theoretical).
	//	  The single state object could be validated outside the render loop
	//	  and maybe even off-line (install time) making its binding at runtime
	//	  cheaper. Unfortunately, this is not yet supported by OpenGL.
	//	  Therefore, this is simulated only in the framework. The
	//	  OGLContext::setState() will do as few as possible real state changes
	//	  on GPU side. However, each state is tested if it changed since the
	//	  previous state. All together this can be faster or slower than a raw
	//	  implementation with many states.
	//	* API upward compatibility DX12 and Vulkan follow the same concept.
	struct Pipeline
	{
		RasterizerState rasterizer;
		DepthStencilState depthStencil;
		BlendState blendState;
		// 64 textures is very high. It might be that your GPU does not support
		// that many. Have a look at GL_MAX_COMBINED_TEXTURE_IMAGE_UNITS.
		SamplerState* samplerState[64] = {nullptr};
		Program* shader = nullptr;
		VertexFormat* vertexFormat = nullptr;
	};

	// Compute shaders ignore most of the state. Therefore, the ComputePipeline
	// is a more lightweight structure to be used with compute shaders.
	struct ComputePipeline
	{
		SamplerState* samplerState[64] = {nullptr};
		Program* shader = nullptr;
	};

} // namespace gpupro