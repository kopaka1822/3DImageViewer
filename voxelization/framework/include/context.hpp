#pragma once

#include "pipeline.hpp"

namespace gpupro {

	// A context is an interface to the graphics driver which receives
	// all GPU based commands.
	// In OpenGL there is only one global single threaded context. You
	// do not need a context class, but using one can help to structure
	// code and improve performance. Further, it helps to understand
	// other modern APIs like DX and Vulkan.
	//
	// Using this context class is only possible after successful window
	// creation.
	class OGLContext
	{
	public:
		enum class DebugSeverity
		{
			NOTIFICATION = 0,	///< Report everything
			LOW			 = 1,	///< Report a lot of things including performance warnings
			MEDIUM		 = 2,	///< Report only real problems
			HIGH		 = 3	///< Report errors only
		};

		// The constructor enables the Debug extension (former KHR_DEBUG,
		// core since 4.3).
		OGLContext(DebugSeverity _dbgLevel);

		void setState(Pipeline& _pipeline);
		void setState(ComputePipeline& _pipeline);
	private:
		struct {
			RasterizerState rasterizer;
			DepthStencilState depthStencil;
			BlendState blendState;
			// 64 textures is very high. It might be that your GPU does not support
			// that many. Have a look at GL_MAX_COMBINED_TEXTURE_IMAGE_UNITS.
			SamplerState* samplerState[64] = {nullptr};
		} m_currentState;
	};

} // namespace gpupro