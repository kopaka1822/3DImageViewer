#include "../../shared/demowindow.hpp"
#include <gpuproframework.hpp>
#include <glm/vec3.hpp>
#include <glm/mat4x4.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <iostream>
#include <vector>
#include <chrono>
#include <gli/gli.hpp>
#include "DialogOpenFile.h"

using namespace gpupro;
using namespace glm;

struct TransformUniforms
{
	mat4 viewProjection;
	vec3 cameraPosition;
	float shadownTresh;
};

static vec3 s_camPos;
static vec3 s_camDir;
static bool s_wDown = false;
static bool s_sDown = false;
static bool s_aDown = false;
static bool s_dDown = false;
static bool s_spaceDown = false;
static bool s_shiftDown = false;
static float s_discardThresh = 0.01f;
static void keyFunc(GLFWwindow * _window, int _key, int, int _action, int)
{
	if(_action == GLFW_PRESS)
	{
		switch(_key)
		{
			case GLFW_KEY_ESCAPE: glfwSetWindowShouldClose(_window, GLFW_TRUE); break;
			case GLFW_KEY_W: s_wDown = true; break;
			case GLFW_KEY_S: s_sDown = true; break;
			case GLFW_KEY_A: s_aDown = true; break;
			case GLFW_KEY_D: s_dDown = true; break;
			case GLFW_KEY_SPACE: s_spaceDown = true; break;
			case GLFW_KEY_LEFT_SHIFT: s_shiftDown = true; break;
			case GLFW_KEY_R: s_discardThresh = std::max(s_discardThresh - 0.01f, 0.0f); break;
			case GLFW_KEY_T: s_discardThresh = std::min(s_discardThresh + 0.01f, 0.99f); break;
		}
	}
	else if(_action == GLFW_RELEASE)
	{
		switch (_key)
		{
		case GLFW_KEY_W: s_wDown = false; break;
		case GLFW_KEY_S: s_sDown = false; break;
		case GLFW_KEY_A: s_aDown = false; break;
		case GLFW_KEY_D: s_dDown = false; break;
		case GLFW_KEY_SPACE: s_spaceDown = false; break;
		case GLFW_KEY_LEFT_SHIFT: s_shiftDown = false; break;
		}
	}
}

void tick(float dt)
{
	if(s_wDown)
		s_camPos += dt * s_camDir;
	if (s_sDown)
		s_camPos -= dt * s_camDir;
	if (s_aDown)
		s_camPos -= dt * normalize(cross(s_camDir, vec3(0.0f, 1.0f, 0.0f)));
	if (s_dDown)
		s_camPos += dt * normalize(cross(s_camDir, vec3(0.0f, 1.0f, 0.0f)));
	if (s_spaceDown)
		s_camPos += dt * vec3(0.0f, 1.0f, 0.0f);
	if (s_shiftDown)
		s_camPos -= dt * vec3(0.0f, 1.0f, 0.0f);
}

static float s_camTheta = 0.5f;
static float s_camPhi = 0.0f;

static void mouseFunc(GLFWwindow * _window, double _x, double _y)
{
	static double oldX, oldY;
	if(glfwGetMouseButton(_window, GLFW_MOUSE_BUTTON_1) == GLFW_PRESS)
	{
		float deltaX = float(_x - oldX);
		float deltaY = float(_y - oldY);

		// rotate
		auto side = normalize(cross(s_camDir, vec3(0.0f, -1.0f, 0.0f)));
		auto v =  glm::rotate(mat4(), deltaY * 0.01f, side) * vec4(s_camDir,1.0f);
		v = glm::rotate(mat4(), deltaX * 0.01f, vec3(0.0f, -1.0f, 0.0f)) * v;

		s_camDir = vec3(v.x, v.y, v.z);

		s_camPhi -= deltaX * 0.007f;
		s_camTheta = clamp(s_camTheta + deltaY * 0.007f, -1.5f, 1.5f);
	}
	oldX = _x;
	oldY = _y;
}

static float s_camZoom = 4.0f;
static void scrollFunc(GLFWwindow *, double , double _sy)
{
	s_camZoom = clamp(s_camZoom - 0.5f * (float)_sy, 1.0f, 6.0f);
}


int main()
{
	std::cerr
		<< "3D Image Viewer" << std::endl
		<< std::endl
		<< "Key bindings:" << std::endl
		<< "  ESC:          quit program" << std::endl
		<< "  WASD:         move camera" << std::endl
		<< "  Space/Shift:  move camera up/down" << std::endl
		<< "  Mouse:        change camera rotation (press left button)" << std::endl;

	try {
		DemoWindow window(1024, 1024, "3D Image Viewer");
		OGLContext context(OGLContext::DebugSeverity::LOW);
		window.setKeyCallback(keyFunc);
		window.setMouseCallback(mouseFunc);
		window.setScrollCallback(scrollFunc);

		Pipeline showVoxelsPipe;
		showVoxelsPipe.depthStencil.depthTest = true;
		Shader voxelVert(Shader::Type::VERTEX, "shaders/voxel.vert");
		Shader voxelGeom(Shader::Type::GEOMETRY, "shaders/voxel.geom");
		Shader shadingFrag(Shader::Type::FRAGMENT, "shaders/shading.frag");
		Program showVoxelsShader(voxelVert, voxelGeom, shadingFrag);
		showVoxelsPipe.shader = &showVoxelsShader;

		std::string texFilename = "";
		{
			DialogOpenFile ofd = DialogOpenFile("dds,ktx");
			ofd.Show();
			if (!ofd.IsSuccess())
				throw std::exception("no file provided");
			texFilename = ofd.GetName();
		}

		// and a single mip-layer.
		auto gliTex = gli::texture3d(gli::load(texFilename));
		if (gliTex.empty())
			throw std::exception("test tex not found");
		gli::gl gliGL(gli::gl::PROFILE_GL33);
		auto texFormat = gliGL.translate(gliTex.format(), gliTex.swizzles());

		Texture myVoxelTex = Texture(Texture::Layout::TEX_3D, gliTex.extent().x, gliTex.extent().y, gliTex.extent().z,
			static_cast<InternalFormat>(texFormat.Internal), 1);
		{
			
			myVoxelTex.setData(0, 0, SetDataFormat(texFormat.External),
				SetDataType(texFormat.Type), gliTex.data());
		}

		// Create the vertex formats
		VertexFormat vertexFormat({
			{0, 0, 3, VertexAttribute::Type::FLOAT, GL_FALSE, 0, 0},	// Position
			{1, 1, 3, VertexAttribute::Type::FLOAT, GL_FALSE, 0, 0}		// Normal
		});

		float zero[4] = {0.0f};
		SamplerState pointSampler(SamplerState::Filter::NEAREST, SamplerState::Filter::NEAREST, SamplerState::Filter::NEAREST,
			1.0f, SamplerState::DepthCompareFunc::DISABLE, SamplerState::BorderHandling::BORDER, zero);
		showVoxelsPipe.samplerState[0] = &pointSampler;

		// Create a uniform buffers
		Buffer transformUBO(Buffer::Type::UNIFORM, sizeof(TransformUniforms), 1, Buffer::Usage::SUB_DATA_UPDATE);
		TransformUniforms transformUniforms;
		
		s_camPos = vec3(float(gliTex.extent().x) / 2.0f,
			float(gliTex.extent().y / 2.0f),
			float(gliTex.extent().z) * 2.0f);
		s_camDir = vec3(0.0f, 0.0f, -1.0f);
		// Main loop
		glClearColor(0.0f, 0.3f, 0.3375f, 1.0f);

		while(window.isOpen())
		{
			auto time_start = std::chrono::high_resolution_clock::now();		
			transformUniforms.viewProjection = glm::perspective(40.0f * 3.1415926f / 180.0f, 1.0f, 0.1f, 100.0f) * 
				glm::lookAt(s_camPos, s_camPos + s_camDir, vec3(0.0f, 1.0f, 0.0f));
			transformUniforms.cameraPosition = s_camPos;
			transformUniforms.shadownTresh = s_discardThresh;
			transformUBO.subDataUpdate(0, sizeof(TransformUniforms), &transformUniforms);

			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

			myVoxelTex.bindAsTexture(0);

			transformUBO.bindAsUniformBuffer(0);
			context.setState(showVoxelsPipe);
			glDrawArrays(GL_POINTS, 0, gliTex.extent().x * gliTex.extent().y * gliTex.extent().z);

			// Input handling
			window.handleEventsAndPresent();	
			auto time_end = std::chrono::high_resolution_clock::now();
			tick(float(std::chrono::duration_cast<std::chrono::milliseconds>(time_end - time_start).count()) / 100.0f);

			std::cerr << "discard threshold (R- T+): " << s_discardThresh << "        \r";
		}
	} catch(std::exception _ex) {
		std::cerr << "ERR: " << _ex.what();
		return 1;
	}

	return 0;
}