#pragma once

#include <gl.hpp>
#include <GLFW/glfw3.h>

class DemoWindow
{
public:
	DemoWindow(unsigned _width, unsigned _height, const char* _title);
	~DemoWindow();

	bool isOpen() { return m_open; }
	void handleEventsAndPresent();

	void setKeyCallback(GLFWkeyfun _func);
	void setMouseCallback(GLFWcursorposfun _func);
	void setScrollCallback(GLFWscrollfun _func);
private:
	struct GLFWwindow* m_windowHandle;
	bool m_open;
};