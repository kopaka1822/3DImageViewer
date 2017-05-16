#include "demowindow.hpp"
#include <iostream>

static void errorCallbackGLFW(int error, const char* description)
{
	std::cerr << "ERR: GLFW error, code " << error << " desc: \"" << description << "\"\n";
}

DemoWindow::DemoWindow(unsigned _width, unsigned _height, const char* _title) :
	m_open(true)
{
	std::cerr << "INF: Initializing GLFW ...\n";
	glfwSetErrorCallback(errorCallbackGLFW);
	if(!glfwInit()) throw std::exception("Cannot initialize GLFW!\n");

	std::cerr << "INF: Creating window and context...\n";
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 4);
	glfwWindowHint(GLFW_SAMPLES, 8);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	m_windowHandle = glfwCreateWindow(_width, _height, _title, nullptr, nullptr);
	if(!m_windowHandle) throw std::exception("Window creation failed!");

	glfwMakeContextCurrent(m_windowHandle);
	// Enable V-Sync
	glfwSwapInterval(1);
}

DemoWindow::~DemoWindow()
{
	if(m_windowHandle)
		glfwDestroyWindow(m_windowHandle);
	glfwTerminate();
}

void DemoWindow::handleEventsAndPresent()
{
	m_open = !glfwWindowShouldClose(m_windowHandle);

	if(m_open)
	{
		// Submit all commands. Otherwise, it is possible that even commands
		// from the next frame are handled in parallel, which causes wired
		// synchronization issues (eg. in persistent mapped buffers).
		glFlush();

		glfwSwapBuffers(m_windowHandle);
		glfwPollEvents();
	}
}

void DemoWindow::setKeyCallback(GLFWkeyfun _func)
{
	glfwSetKeyCallback(m_windowHandle, _func);
}

void DemoWindow::setMouseCallback(GLFWcursorposfun _func)
{
	glfwSetCursorPosCallback(m_windowHandle, _func);
}

void DemoWindow::setScrollCallback(GLFWscrollfun _func)
{
	glfwSetScrollCallback(m_windowHandle, _func);
}
